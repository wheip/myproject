#include "runtask.h"
#include <QThread>
#include <QBuffer>
#include <QtConcurrent>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

RunTask::RunTask() {
    m_database = new Database("runtask");
    m_executeTask = new ExecuteTask();
    m_deviceManager = JYDeviceManager::getInstance();
    m_timer = new QTimer();
    connect(m_timer, &QTimer::timeout, this, [this]() {
        IRImageData imageData = g_camera.getImageData();
        if(imageData.image.empty()) {
            m_timer->stop();
            return;
        }
        m_timer->stop();
        int stepId = m_current_run_step_id;
        int deviceId = m_device_id;

        QtConcurrent::run([stepId, deviceId, imageData, taskName = m_imagetask_table_name]() {
            Database db("saveimage");

            Image tempImage;
            tempImage.step_id = stepId;

            std::vector<uchar> bufVec;

            cv::Mat imageToEncode;
            if (imageData.image.depth() != CV_8U) {
                imageData.image.convertTo(imageToEncode, CV_8U);
            } else {
                imageToEncode = imageData.image;
            }

            try {
                if (!cv::imencode(".jpg", imageToEncode, bufVec)) {
                    qDebug() << "cv::imencode failed: returned false";
                    return;
                }
            } catch (const cv::Exception &e) {
                qDebug() << "cv::imencode exception:" << e.what();
                return;
            }
            QByteArray byteArray(reinterpret_cast<const char*>(bufVec.data()), bufVec.size());
            tempImage.image_data = byteArray;

            QByteArray tempByteArray(reinterpret_cast<const char*>(imageData.tempData),
                                     imageData.tempWidth * imageData.tempHeight * sizeof(uint16_t));
            tempImage.temp_data = tempByteArray;
            tempImage.task_table_name = taskName;
            tempImage.temp_width = imageData.tempWidth;
            tempImage.temp_height = imageData.tempHeight;

            QString TableName = QString::number(deviceId) + "$$image";
            QString ErrorInfo;
            if (!db.insert_image(TableName, tempImage, ErrorInfo)) {
                qDebug() << "图像插入失败" << ErrorInfo;
            }
        });
    });

}

RunTask::~RunTask() {
    delete m_database;
    delete m_executeTask;
    delete m_timer;
    disconnect();
}

void RunTask::SetSaveDataPath(QString& path) {
    m_executeTask->SetsaveData(3, path, true);
    m_imagetask_table_name = path.split("$$").at(2);
}

void RunTask::Signalconnect() {
    QObject::connect(m_executeTask, &ExecuteTask::taskCompleted, this, [this]() {
        m_cv.notify_all();
    });
    QObject::connect(&g_camera, &Camera::Image_data, this, &RunTask::TemptureMonitor);
    QObject::connect(m_executeTask, &ExecuteTask::taskStatusChanged, this, &RunTask::ExecuteTaskStateChanged);
}

void RunTask::disconnect() {
    QObject::disconnect(m_executeTask, nullptr, this, nullptr);
    QObject::disconnect(&g_camera, nullptr, this, nullptr);
}

void RunTask::RunTestTask(int& device_id, QString& taskId) {
    m_device_id = device_id;
    m_taskId = taskId;
    m_steps.clear();
    QString errorMsg;
    if(!m_deviceManager->SelfTestAllDevices(errorMsg)) {
        QMessageBox::warning(nullptr, "错误", errorMsg);
        return;
    }
    QString TableName = QString::number(m_device_id) + "$$step";
    QString sql = "select * from " + TableName + " where test_task_id = '" + m_taskId + "';";
    QList<QMap<QString, QVariant>> result = m_database->selectQuery(sql);
    for (QMap<QString, QVariant> res : result) {
        Step step;
        step.id = res["id"].toInt();
        step.test_task_id = res["test_task_id"].toString();
        step.collecttime = res["collecttime"].toDouble();
        step.step_number = res["step_number"].toInt();
        step.continue_step = res["continue_step"].toBool();
        step.isthermometry = res["isthermometry"].toBool();
        step.thermometry_pause_time = res["thermometry_pause_time"].toDouble();
        m_steps.push_back(step);
    }
    Signalconnect(); // 连接信号槽
    m_interrupt = false;
    QString filepath = QString::number(m_device_id) + "$$" + m_taskId + "$$" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    SetSaveDataPath(filepath);
    QThread* taskThread = QThread::create([this]() {
        for (Step step : m_steps) {
            emit StateChanged(10, "开始执行步骤[" + QString::number(step.step_number) + "]");
            RunStep(step);
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cv.wait(lock);
            if(m_interrupt) break;
            emit StateChanged(11, "步骤[" + QString::number(step.step_number) + "]执行完成");
        }
        emit StateChanged(12, "任务执行完成");
        emit taskCompleted();
    });
    taskThread->start();
    connect(taskThread, &QThread::finished, taskThread, &QObject::deleteLater);
    connect(taskThread, &QThread::finished, this, [this]() {
        disconnect(); //断开所有信号
        {std::lock_guard<std::mutex> lock(m_status_mutex);
        m_taskStatus = RunTaskStatus::Stop;}
    });
}

void RunTask::RunStep(Step& step) {
    {std::lock_guard<std::mutex> lock(m_status_mutex);
    m_taskStatus = RunTaskStatus::Running;}
    QVector<TaskConfig> taskConfigs = GengerateTaskConfig(step);
    m_executeTask->configureTask(taskConfigs);
    if(!m_executeTask->startAllTasks()) {
        QString ErrorMessage = m_executeTask->getErrorMessage();
        QMetaObject::invokeMethod(this, "HandleError", Qt::QueuedConnection, Q_ARG(QString, ErrorMessage));
        return;
    }
    QMetaObject::invokeMethod(this, "ConnectWire", Qt::QueuedConnection, Q_ARG(Step, step));
    std::unique_lock<std::mutex> lock(m_connectwire_mutex);
    m_connectwire_cv.wait(lock);
    if(!m_executeTask->triggerAllTasks()) {
        QString ErrorMessage = m_executeTask->getErrorMessage();
        QMetaObject::invokeMethod(this, "HandleError", Qt::QueuedConnection, Q_ARG(QString, ErrorMessage));
        return;
    }
    m_current_run_step_id = step.id;
    if(step.isthermometry) {
        QMetaObject::invokeMethod(this, "begin_collect_image", Qt::QueuedConnection, Q_ARG(double, step.thermometry_pause_time * step.collecttime));
    }
}

void RunTask::HandleError(const QString& errorMessage) {
    QMessageBox::warning(nullptr, "错误", errorMessage);
    {std::lock_guard<std::mutex> lock(m_status_mutex);
    m_taskStatus = RunTaskStatus::Stop;}
    m_executeTask->stopAllTasks();
    m_interrupt = true;
    m_cv.notify_all();
    emit StateChanged(13, "任务中断");
}

void RunTask::begin_collect_image(double time) {
    m_timer->start(time * 10);
}

QVector<TaskConfig> RunTask::GengerateTaskConfig(Step step) {
    QVector<TaskConfig> taskConfigs;
    QString TableName = QString::number(m_device_id) + "$$pxie5711";
    std::vector<PXIe5711Waveform> waveforms;
    m_database->get_pxie5711waveform(TableName, "step_id = " + QString::number(step.id), waveforms);
    if(waveforms.size() != 0) {
        TaskConfig taskConfig5711;
        taskConfig5711.deviceType = DeviceType::PXIe5711;
        taskConfig5711.collectTime = step.collecttime;
        taskConfig5711.parameters = waveforms;
        taskConfig5711.enabled = true;
        taskConfigs.push_back(taskConfig5711);
    }

    TableName = QString::number(m_device_id) + "$$pxie5320";
    std::vector<PXIe5320Waveform> waveforms5322;
    m_database->get_pxie5320waveform(TableName, "step_id = " + QString::number(step.id) + " AND device = '5322'", waveforms5322);
    if(waveforms5322.size() != 0) {
        TaskConfig taskConfig5322;
        taskConfig5322.deviceType = DeviceType::PXIe5322;
        taskConfig5322.collectTime = step.collecttime;
        taskConfig5322.parameters = waveforms5322;
        taskConfig5322.enabled = true;
        taskConfigs.push_back(taskConfig5322);
    }

    std::vector<PXIe5320Waveform> waveforms5323;
    m_database->get_pxie5320waveform(TableName, "step_id = " + QString::number(step.id) + " AND device = '5323'", waveforms5323);
    if(waveforms5323.size() != 0) {
        TaskConfig taskConfig5323;
        taskConfig5323.deviceType = DeviceType::PXIe5323;
        taskConfig5323.collectTime = step.collecttime;
        taskConfig5323.parameters = waveforms5323;
        taskConfig5323.enabled = true;
        taskConfigs.push_back(taskConfig5323);
    }

    TableName = QString::number(m_device_id) + "$$pxie8902";
    std::vector<Data8902> data8902;
    m_database->get_8902data(TableName, "step_id = " + QString::number(step.id), data8902);
    if(data8902.size() != 0) {
        TaskConfig taskConfig8902;
        taskConfig8902.deviceType = DeviceType::PXIe8902;
        taskConfig8902.collectTime = step.collecttime;
        taskConfig8902.parameters = data8902;
        taskConfig8902.enabled = true;
        taskConfigs.push_back(taskConfig8902);
    }

    return taskConfigs;
}


void RunTask::InterruptTask() {
    m_executeTask->stopAllTasks();
    m_interrupt = true;
    m_cv.notify_all();
    emit StateChanged(13, "任务中断");
}

void RunTask::TemptureMonitor(const IRImageData& imageData) {
    if(imageData.tempData == nullptr) {
        return;
    }
    float maxTemp = -273.15f;
    std::vector<uint16_t> tempData(imageData.tempData, imageData.tempData + imageData.tempWidth * imageData.tempHeight);
    for(uint16_t temp : tempData) {
        float currentTemp = 0.1f * temp;
        maxTemp = std::max(maxTemp, currentTemp);
    }
    if(maxTemp > 100) {
        InterruptTask();
    }
}

void RunTask::ExecuteTaskStateChanged(TaskStatus state, const QString& message)
{
    emit StateChanged(static_cast<int>(state), message);
}

void RunTask::ConnectWire(Step step) {
    if(step.continue_step) {
        m_connectwire_cv.notify_all();
    }
    else{
        TaskConnectWire dialog;
        dialog.setStep(step.id);
        dialog.setDevice(m_device_id);
        if(dialog.exec() == QDialog::Accepted) {
            m_connectwire_cv.notify_all();
        } else {
            m_connectwire_cv.notify_all();
        }

    }
}
