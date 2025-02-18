#include "executetask.h"
#include <QThread>
#include <memory>
#include <thread>
#include <algorithm>

ExecuteTask::ExecuteTask(const QString taskid, const QString TableName, bool single_step, Step step_signal)
    : taskid(taskid)
    , TableName(TableName)
    , connectionName("ConnectionTask_" + QString::number((quintptr)QThread::currentThreadId()))
    , database(connectionName, nullptr)
    , single_step(single_step)
    , step_signal(step_signal)
{
    device_id = TableName.split("$$")[0];
    initializeDevices();
    loadTaskData(taskid);
    setupDeviceThreads();
    connectDeviceSignals();
    startMonitoringThread();
    taskManager = std::make_shared<TaskManager>(4, TableName);
}

void ExecuteTask::initializeDevices() {
    pxi5711 = std::make_shared<PXIe5711>(nullptr);
    pxi5322 = std::make_shared<PXIe5320>(nullptr, 5322);
    pxi5323 = std::make_shared<PXIe5320>(nullptr, 5323);
    pxi8902 = std::make_shared<PXIe8902>(nullptr);
}

void ExecuteTask::loadTaskData(const QString& taskid) {
    QString table_name_testtask = device_id + "$$TestTask";
    QString ErrorMessage;
    if(!database.get_testtask(table_name_testtask, QString("id = '%1'").arg(taskid), testtask, ErrorMessage))
    {
        qDebug() << ErrorMessage;
        return;
    }
    if (!testtask.empty()) {
        connection_image_data = testtask[0].connection_image_data;
    }
    QString table_name_step = device_id + "$$Step";
    database.get_step(table_name_step, QString("test_task_id = '%1'").arg(taskid), step);
    std::sort(step.begin(), step.end(), [](const Step& a, const Step& b) {
        return a.step_number < b.step_number;
    });
}

void ExecuteTask::setupDeviceThreads() {
    pxi5711->moveToThread(&pxi5711thread);
    pxi5322->moveToThread(&pxi5322thread);
    pxi5323->moveToThread(&pxi5323thread);
    pxi8902->moveToThread(&pxi8902thread);
    pxi5711thread.start();
    pxi5322thread.start();
    pxi5323thread.start();
    pxi8902thread.start();
}

void ExecuteTask::device_StateChanged(const QString& state, int numb) {
    emit StateChanged(state, numb);
}

void ExecuteTask::connectDeviceSignals() {
    connectDeviceSignals_5711();
    connectDeviceSignals_5322();
    connectDeviceSignals_5323();
    connectDeviceSignals_8902();
}

void ExecuteTask::startMonitoringThread() {
    timerthread = QThread::create([this]() {
        while (continueExecution) {
            this->isTaskComplete();
            QThread::msleep(20);
        }
    });
    connect(timerthread, &QThread::finished, timerthread, &QObject::deleteLater);
    timerthread->start();
}

void ExecuteTask::connectDeviceSignals_5711() {
    if (!QObject::connect(pxi5711.get(), &PXIe5711::DeviceReady, this, [this]() {
        emit StateChanged("Device5711 Ready", 0);
        is5711Ready = true;
        cv_5711.notify_one();
    })) {
        qDebug() << "Failed to connect 5711 DeviceReady signal";
    }
    if (!QObject::connect(pxi5711.get(), &PXIe5711::WaveformGenerated, this, [this]() {
        emit StateChanged("Waveform Generated", 0);
        is5711Generated = true;
    })) {
        qDebug() << "Failed to connect 5711 WaveformGenerated signal";
    }
    if (!QObject::connect(pxi5711.get(), &PXIe5711::StateChanged, this, [this](const QString& state, int numb) {
        device_StateChanged(state, numb);
    })) {
        qDebug() << "Failed to connect 5711 StateChanged signal";
    }
}

void ExecuteTask::connectDeviceSignals_5322() {
    if (!QObject::connect(pxi5322.get(), &PXIe5320::DeviceReady, this, [this]() {
        emit StateChanged("Device5322 Ready", 0);
        is5322Ready = true;
        cv_5711.notify_one();
    })) {
        qDebug() << "Failed to connect 5322 DeviceReady signal";
    }
    if (!QObject::connect(pxi5322.get(), &PXIe5320::signalAcquisitionData, this, [this](const auto& data, int serial_number) { 
        SlotAcquisitionData_5322(data, serial_number); 
        step_5322collected_time ++;
        int emit_time = (std::min(step_5322collected_time, step_5323collected_time) > 0) ? std::min(step_5322collected_time, step_5323collected_time) : std::max(step_5322collected_time, step_5323collected_time);
        emit StateChanged("step_collectedtime", emit_time);
    })) {
        qDebug() << "Failed to connect 5322 signalAcquisitionData signal";
    }
    if (!QObject::connect(pxi5322.get(), &PXIe5320::CompleteAcquisition, this, [this]() {
        emit StateChanged("Waveform Acquired", 0);
        is5322Acquired = true;
    })) {
        qDebug() << "Failed to connect 5322 CompleteAcquisition signal";
    }
    if (!QObject::connect(pxi5322.get(), &PXIe5320::StateChanged, this, [this](const QString& state, int numb) {
        device_StateChanged(state, numb);
    })) {
        qDebug() << "Failed to connect 5322 StateChanged signal";
    }
}

void ExecuteTask::connectDeviceSignals_5323() {
    if (!QObject::connect(pxi5323.get(), &PXIe5320::DeviceReady, this, [this]() {
        emit StateChanged("Device5323 Ready", 0);
        is5323Ready = true;
        cv_5711.notify_one();
    })) {
        qDebug() << "Failed to connect 5323 DeviceReady signal";
    }
    if (!QObject::connect(pxi5323.get(), &PXIe5320::signalAcquisitionData, this, [this](const auto& data, int serial_number) { 
        SlotAcquisitionData_5323(data, serial_number); 
        step_5323collected_time ++;
        int emit_time = (std::min(step_5322collected_time, step_5323collected_time) > 0) ? std::min(step_5322collected_time, step_5323collected_time) : std::max(step_5322collected_time, step_5323collected_time);
        emit StateChanged("step_collectedtime", emit_time);
    })) {
        qDebug() << "Failed to connect 5323 signalAcquisitionData signal";
    }
    if (!QObject::connect(pxi5323.get(), &PXIe5320::CompleteAcquisition, this, [this]() {
        emit StateChanged("Waveform Acquired", 0);
        is5323Acquired = true;
    })) {
        qDebug() << "Failed to connect 5323 CompleteAcquisition signal";
    }
    if (!QObject::connect(pxi5323.get(), &PXIe5320::StateChanged, this, [this](const QString& state, int numb) {
        device_StateChanged(state, numb);
    })) {
        qDebug() << "Failed to connect 5323 StateChanged signal";
    }
}

void ExecuteTask::connectDeviceSignals_8902() {
    if (!QObject::connect(pxi8902.get(), &PXIe8902::DeviceReady, this, [this]() {
        emit StateChanged("Device8902 Ready", 0);
        is8902Ready = true;
        cv_5711.notify_one();
    })) {
        qDebug() << "Failed to connect 8902 DeviceReady signal";
    }
    if (!QObject::connect(pxi8902.get(), &PXIe8902::signalAcquisitionData, this, [this](const auto& data, int serial_number) { 
        SlotAcquisitionData_8902(data, serial_number); 
    })) {
        qDebug() << "Failed to connect 8902 signalAcquisitionData signal";
    }
    if (!QObject::connect(pxi8902.get(), &PXIe8902::CompleteAcquisition, this, [this]() {
        emit StateChanged("Waveform Acquired", 0);
        is8902Acquired = true;
    })) {
        qDebug() << "Failed to connect 8902 CompleteAcquisition signal";
    }
    if (!QObject::connect(pxi8902.get(), &PXIe8902::StateChanged, this, [this](const QString& state, int numb) {
        device_StateChanged(state, numb);
    })) {
        qDebug() << "Failed to connect 8902 StateChanged signal";
    }
}

ExecuteTask::~ExecuteTask()
{
    continueExecution = false;

    pxi5711waveform.clear();
    pxi5322waveform.clear();
    pxi5323waveform.clear();
    database.disconnect();
    if (timerthread) {
        timerthread->quit();
        timerthread->wait();
        timerthread->deleteLater();
    }
    if (RunTaskThread) {
        RunTaskThread->quit();
        RunTaskThread->wait();
        RunTaskThread->deleteLater();
    }
    
    if (pxi5711thread.isRunning()) {
        pxi5711thread.quit();
        pxi5711thread.wait();
        pxi5711thread.deleteLater();
    }
    if (pxi5322thread.isRunning()) {
        pxi5322thread.quit();
        pxi5322thread.wait();
        pxi5322thread.deleteLater();
    }
    if (pxi5323thread.isRunning()) {
        pxi5323thread.quit();
        pxi5323thread.wait();
        pxi5323thread.deleteLater();
    }
    if (pxi8902thread.isRunning()) {
        pxi8902thread.quit();
        pxi8902thread.wait();
        pxi8902thread.deleteLater();
    }
}

void ExecuteTask::isTaskComplete()
{
    if (is5711Generated && is5322Acquired && is5323Acquired && is8902Acquired && taskCompleted && taskManager->isFinished(taskCompleted)) {
        if(!isInterrupted)
        {
            emit StateChanged("Step Finished", step.size());
            qDebug() << "任务完成";
        }else
        {
            emit StateChanged("Step Interrupted", 0);
        }
        continueExecution = false;
        emit TaskFinished();
    }
}

void ExecuteTask::WaveformAcquisition(Step& s)
{
    QString table_name_pxie5711 = device_id + "$$PXIe5711";
    database.get_pxie5711waveform(table_name_pxie5711, QString("step_id = '%1'").arg(s.id), pxi5711waveform);
    QString table_name_pxie5320 = device_id + "$$PXIe5320";
    database.get_pxie5320waveform(table_name_pxie5320, QString("step_id = '%1' AND device = 5322").arg(s.id), pxi5322waveform);
    database.get_pxie5320waveform(table_name_pxie5320, QString("step_id = '%1' AND device = 5323").arg(s.id), pxi5323waveform);
    QString table_name_pxie8902 = device_id + "$$PXIe8902";
    database.get_8902data(table_name_pxie8902, QString("step_id = '%1'").arg(s.id), pxi8902waveform);
}

void ExecuteTask::WaveformGeneration(Step& s)
{
    if (pxi5711waveform.empty() && pxi5322waveform.empty() && pxi5323waveform.empty() && pxi8902waveform.empty()) {
        qDebug() << "未添加输出/输入端口。";
        return;
    }

    if(!pxi5711waveform.empty())
    {
        QMetaObject::invokeMethod(pxi5711.get(), "receivewaveform", Qt::QueuedConnection,
                              Q_ARG(std::vector<PXIe5711Waveform>, pxi5711waveform));
    }else
    {
        is5711Ready = true;
        is5711Generated = true;
    }

    auto startAcquisition = [this](std::shared_ptr<PXIe5320>& device, auto& waveform, double collecttime) {
        if (!waveform.empty()) {
            QMetaObject::invokeMethod(device.get(), "StartAcquisition", Qt::QueuedConnection,
                                      Q_ARG(std::vector<PXIe5320Waveform>, waveform),
                                      Q_ARG(double, collecttime));
            emit StateChanged("Device" + QString::number(device->cardID) + "Ready", 0);
        }
        else
        {
            if (device->cardID == 5322) {
                is5322Acquired = true;
                is5322Ready = true;
            } else if (device->cardID == 5323) {
                is5323Acquired = true;
                is5323Ready = true;
            }
        }
    };

    if(!pxi8902waveform.empty())
    {
        QMetaObject::invokeMethod(pxi8902.get(), "StartAcquisition", Qt::QueuedConnection,
                              Q_ARG(std::vector<Data8902>, pxi8902waveform),
                              Q_ARG(double, s.collecttime));
    }else
    {
        is8902Acquired = true;
        is8902Ready = true;
    }

    startAcquisition(pxi5322, pxi5322waveform, s.collecttime);
    startAcquisition(pxi5323, pxi5323waveform, s.collecttime);

    std::unique_lock<std::mutex> lock(mtx_5711);
    cv_5711.wait(lock, [this] { return (is5711Ready && is5322Ready && is5323Ready && is8902Ready) || isInterrupted; });
    if(!isInterrupted)
    {
        if(!is5711Generated)
            QMetaObject::invokeMethod(pxi5711.get(), "SendSoftTrigger", Qt::QueuedConnection);
        if(!is5322Acquired)
            QMetaObject::invokeMethod(pxi5322.get(), "SendSoftTrigger", Qt::QueuedConnection);
        if(!is5323Acquired)
            QMetaObject::invokeMethod(pxi5323.get(), "SendSoftTrigger", Qt::QueuedConnection);
        if(!is8902Acquired)
            QMetaObject::invokeMethod(pxi8902.get(), "SendSoftTrigger", Qt::QueuedConnection);
        emit GetInfraredImage(s);
    }

    is5711Ready = false;
    is5322Ready = false;
    is5323Ready = false;
    is8902Ready = false;


    pxi5711waveform.clear();
    pxi5322waveform.clear();
    pxi5323waveform.clear();
    pxi8902waveform.clear();
}
void ExecuteTask::SlotAcquisitionData_5322(const std::vector<PXIe5320Waveform> collectdata, int serial_number)
{
    if(!continueExecution) return;
    qDebug() << "5322 Waveform Acquired";
    taskManager->addTask(collectdata, serial_number);
    emit StateChanged("5322 Waveform Acquired", 0);
}

void ExecuteTask::SlotAcquisitionData_5323(const std::vector<PXIe5320Waveform> collectdata, int serial_number)
{
    if(!continueExecution) return;
    qDebug() << "5323 Waveform Acquired";
    taskManager->addTask(collectdata, serial_number);
    emit StateChanged("5323 Waveform Acquired", 0);
}

void ExecuteTask::SlotAcquisitionData_8902(const std::vector<PXIe5320Waveform> collectdata, int serial_number)
{
    if(!continueExecution) return;
    qDebug() << "8902 Waveform Acquired";
    taskManager->addTask(collectdata, serial_number);
    emit StateChanged("8902 Waveform Acquired", 0);
}

void ExecuteTask::StartTask()
{
    emit StateChanged("State Init", static_cast<int>(step.size()));
    RunTaskThread = QThread::create([this]() {
        if (!continueExecution) return;
        if(single_step) // 单步执行
        {
            if(is5711Generated && is5322Acquired && is5323Acquired && is8902Acquired)
            {
                if(!step_signal.continue_step) {
                    paused = true;
                    std::unique_lock<std::mutex> lock(mtx);
                    cv.wait(lock, [this] { return !paused; });
                }

                if(!isInterrupted)
                {
                    step_5322collected_time = 0;
                    step_5323collected_time = 0;

                    emit StateChanged("step_collecttime", step_signal.collecttime);
                    is5711Generated = false;
                    is5322Acquired = false;
                    is5323Acquired = false;
                    is8902Acquired = false;


                    WaveformAcquisition(step_signal);
                    WaveformGeneration(step_signal);
                }
            }
        }else{ // 执行整个任务
            int i = 0;
            while (i < step.size() && continueExecution) {

                if(isInterrupted) break;

                if(is5711Generated && is5322Acquired && is5323Acquired && is8902Acquired)
                {
                    emit StateChanged("Step Finished", i);

                    if(!step[i].continue_step) {
                        paused = true;
                        emit StateChanged("Connect Wire:" + step[i].id, i);
                        std::unique_lock<std::mutex> lock(mtx);
                        cv.wait(lock, [this] { return !paused; });
                    }

                    if(isInterrupted) break;

                    emit StateChanged("Step Begin", i + 1);
                    is5711Generated = false;
                    is5322Acquired = false;
                    is5323Acquired = false;
                    is8902Acquired = false;


                    step_5322collected_time = 0;
                    step_5323collected_time = 0;

                    emit StateChanged("step_collecttime", step[i].collecttime);
                    WaveformAcquisition(step[i]);
                    WaveformGeneration(step[i]);
                    i++;
                }
            }
        }
        taskCompleted = true;
    });
    RunTaskThread->start();
}

void ExecuteTask::InterruptTask()
{
    isInterrupted = true;
    if(!paused)
    {
        QMetaObject::invokeMethod(pxi5322.get(), "InterruptAcquisition", Qt::QueuedConnection);
        QMetaObject::invokeMethod(pxi5323.get(), "InterruptAcquisition", Qt::QueuedConnection);
        QMetaObject::invokeMethod(pxi5711.get(), "InterruptAcquisition", Qt::QueuedConnection);
        QMetaObject::invokeMethod(pxi8902.get(), "InterruptAcquisition", Qt::QueuedConnection);
    }
    paused = false;
    cv.notify_one();
    cv_5711.notify_one();
}

void ExecuteTask::ContinueTask()
{
    paused = false;
    cv.notify_one();
}
