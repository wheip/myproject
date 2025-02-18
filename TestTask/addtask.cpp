#include "addtask.h"
#include "ui_addtask.h"
#include <QDateTime>
#include <QFileDialog>
#include <QBuffer>
#include <QMessageBox>
#include <qthread.h>
#include <QStandardItemModel>
#include <QRegExp>
#include <QtConcurrent>
#include <QDesktopServices>

extern bool taskisexecuting;
extern QString current_device_id;

AddTask::AddTask(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AddTask)
    , Waveform({"SineWave", "SquareWave", "TriangleWave"})
    , database("addtask", this)
    , device_id(current_device_id)
{
    ui->setupUi(this);
    this->close();
    InitComboBox();

    QObject::connect(&gDeviceId, &DeviceId::deviceChanged, this, [this]() {
        on_pbrefreshdeviceid_clicked();
    });
    qDebug() << "AddTask constructor";
}

AddTask::~AddTask()
{
    delete ui;
    database.disconnect();
}

void AddTask::InitComboBox()
{
    qDebug() << "InitComboBox";
    ui->comboBox_outputport->clear();
    ui->comboBox_collectport->clear();
    ui->cbdeviceid->clear();
    ui->pbselectlocation->setEnabled(false);
    for(int i = 0; i < 32; i++)
    {
        ui->comboBox_outputport->addItem("端口" + QString::number(i));
    }

    QString currenttime = QDateTime::currentDateTime().toString("yyyy_MM_dd_HH_mm_ss");
    ui->groupBox->setEnabled(true);
    ui->groupBox->setStyleSheet("");
    ui->lineEdit_taskid->setText(currenttime);
    ui->groupBox_3->setEnabled(false);
    ui->groupBox_3->setStyleSheet("background-color: rgb(64, 64, 64);");
    ui->groupBox_digitaloutput->setEnabled(false);
    ui->groupBox_digitaloutput->setStyleSheet("background-color: rgb(64, 64, 64);");
    ui->groupBox_analogoutput->setEnabled(true);
    ui->groupBox_analogoutput->setStyleSheet("");

    ui->comboBox_outputtype->clear();
    for(auto &item : Waveform)
    {
        ui->comboBox_outputtype->addItem(item);
    }

    for(int i = 0; i < 32; i++)
    {
        ui->comboBox_collectport->addItem("端口" + QString::number(i));
    }

    std::vector<Label> label_info;
    QString ErrorMessage;
    if(device_id != "" && !database.get_deviceelement(device_id, "", label_info, ErrorMessage))
    {
        qDebug() << ErrorMessage;
    }
    for(auto &item : label_info)
    {
        ui->cbdeviceid->addItem(QString::number(item.id));
    }

    if(!label_info.empty())
    {
        element_id = label_info[0].id;
        refresh_testtask();
    }

    taskid.clear();
    step_id.clear();
    step_num = 0;
    pxi5711waveform.clear();
    pxi5322waveform.clear();
    pxi5323waveform.clear();
    data8902.clear();
    display_port_info(pxi5711waveform, pxi5322waveform, pxi5323waveform);
    initimage();
    connect(ui->comboBox_outputport, SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateOutputType(int)));

    connect(ui->comboBox_collectdevice, SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateCollectPort(int)));
}

void AddTask::initimage()
{
    if(device_id.isEmpty()) {
        return;
    }
    QFuture<QString> future = QtConcurrent::run([this]() -> QString {
        QString ErrorMessage;
        Database threadDb(QUuid::createUuid().toString(), nullptr);
        if(!threadDb.connect()) {
            return QString("数据库连接失败");
        }
        std::vector<Device> devices;
        QString condition = "id = '" + device_id + "'";
        threadDb.get_device(condition, devices, true);
        if(devices.empty()) {
            return QString("设备查询失败");
        }
        LabeledingImage = QImage::fromData(devices[0].image);
        label_info.clear();
        if(!threadDb.get_deviceelement(device_id, "", label_info, ErrorMessage)) {
            return ErrorMessage;
        }
        threadDb.disconnect();
        return QString("");
    });
    future.waitForFinished(); // 等待future执行完成
    QString errorMessage = future.result();
    if(!errorMessage.isEmpty()) {
        QMessageBox::warning(this, "提示", errorMessage);
    }else{
        updateimage();
    }
}

void AddTask::updateimage()
{
    while(QLayoutItem* item = ui->gridLayout_locationimage->takeAt(0)) {
        if (QWidget* widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }
    labelediting = std::make_unique<LabelEditingWindow>(this, LabeledingImage, label_info, label_info_add, delete_id);
    labelediting->setOnlyViewModel();
    ui->gridLayout_locationimage->addWidget(labelediting.get());
}

void AddTask::UpdateOutputType(int index)
{
    if(ui->comboBox_outputport->currentIndex() <= 15 || ui->comboBox_outputport->currentIndex() >= 28)
    {
        ui->groupBox_digitaloutput->setEnabled(false);
        ui->groupBox_digitaloutput->setStyleSheet("background-color: rgb(64, 64, 64);");
        ui->groupBox_analogoutput->setEnabled(true);
        ui->groupBox_analogoutput->setStyleSheet("");
    }
    else
    {
        ui->comboBox_outputtype->setCurrentIndex(0);
        ui->groupBox_digitaloutput->setEnabled(true);
        ui->groupBox_digitaloutput->setStyleSheet("");
        ui->groupBox_analogoutput->setEnabled(false);
        ui->groupBox_analogoutput->setStyleSheet("background-color: rgb(64, 64, 64);");
    }
}

void AddTask::UpdateCollectPort(int index)
{
    if(ui->comboBox_collectdevice->currentIndex() <= 1)
    {
        int collectportcount = (ui->comboBox_collectdevice->currentText() == "5323") ? 32 : 16;
        ui->comboBox_collectport->clear(); // 清空之前的端口
        for(int i = 0; i < collectportcount; i++)
        {
            ui->comboBox_collectport->addItem("端口" + QString::number(i));
        }
    }else{
        ui->comboBox_collectport->clear();
        ui->comboBox_collectport->addItem("voltage");
        ui->comboBox_collectport->addItem("current");
        ui->comboBox_collectport->addItem("resistance");
    }
}

void AddTask::on_pbcreattask_clicked()
{
    taskid = ui->lineEdit_taskid->text();
    if(taskid.length() > 20)
    {
        QMessageBox::warning(this, "任务ID过长", "任务ID长度不能超过20个字符");
        return;
    }
    QRegExp rx("[a-zA-Z0-9_]+");
    if(!rx.exactMatch(taskid) || taskid.startsWith("test"))
    {
        QMessageBox::warning(this, "任务ID包含非法字符", "任务ID只能由数字、字母和下划线组成,且不能以test开头");
        return;
    }
    ui->pbaddstep->setEnabled(false);
    TestTask testtask;
    testtask.id = taskid;
    testtask.element_id = element_id;
    testtask.connection_image_data = QByteArray();
    QString table_name = device_id + "$$TestTask";
    QString ErrorMessage;
    if(!database.insert_testtask(table_name, testtask, ErrorMessage))
    {
        ui->pbaddstep->setEnabled(true);
        QMessageBox::warning(this, "任务创建失败", ErrorMessage);
        return;
    }
    
    g_FolderCheck.Check_Folder(device_id);
    
    ui->groupBox_3->setEnabled(true);
    ui->groupBox_3->setStyleSheet("");
    ui->groupBox->setEnabled(false);
    ui->groupBox->setStyleSheet("background-color: rgb(64, 64, 64);");

    step_num = 1;
    pxi5711waveform.clear();
    pxi5322waveform.clear();
    pxi5323waveform.clear();
    ui->pbaddstep->setEnabled(true);

    update_treeview();
}


void AddTask::on_pdaddwave_clicked()
{
    PXIe5711Waveform waveform;
    step_id = taskid + "_" + QString::number(step_num);
    waveform.id = step_id + "_" + QString::number(ui->comboBox_outputport->currentIndex());
    waveform.channel = ui->comboBox_outputport->currentIndex();
    waveform.step_id = step_id;
    if(waveform.channel < 16 || waveform.channel >= 28)
    {
        waveform.waveform_type = (ui->spinBox_current->value() > 0) ? "HighLevelWave" : "LowLevelWave";
        waveform.amplitude = abs(ui->spinBox_current->value());
    }
    else
    {
        waveform.waveform_type = ui->comboBox_outputtype->currentText();
        waveform.amplitude = ui->doubleSpinBox_outputvoltage->value();
        waveform.frequency = ui->spinBox_outputfrequency->value();
        waveform.dutyCycle = ui->doubleSpinBox_outputdutycycle->value();
    }
    // 检查重复通道
    {
        QMutexLocker locker(&waveformMutex);
        auto it = std::find_if(pxi5711waveform.begin(), pxi5711waveform.end(), 
            [&waveform](const PXIe5711Waveform& w) {
                return w.channel == waveform.channel;
            });
            
        if(it != pxi5711waveform.end()) {
            auto result = QMessageBox::warning(this, "警告", 
                "通道" + QString::number(waveform.channel) + "已存在。是否覆盖？", 
                QMessageBox::Yes | QMessageBox::No);
                
            if (result != QMessageBox::Yes) {
                return;
            }
            pxi5711waveform.erase(it);
        }
    }

    // 创建异步任务
    QFuture<void> future = QtConcurrent::run([this, waveform]() {
        while(select_location_id == 0)
        {
            QMetaObject::invokeMethod(this, "showLocationSelectionTip", 
                Qt::QueuedConnection,
                Q_ARG(QString, "请在图中选择端口接线位置"));
                
            std::unique_lock<std::mutex> lock(setlocationMutex);
            setlocationCondition.wait(lock);

            QMetaObject::invokeMethod(this, [this](){
                if(currentTip) {
                    currentTip->close();
                    currentTip = nullptr;
                }
            }, Qt::QueuedConnection);
        }

        {
            QMutexLocker locker(&waveformMutex);
            PXIe5711Waveform tempwaveform = waveform;
            tempwaveform.positive_connect_location = select_location_id;
            select_location_id = 0;
            pxi5711waveform.push_back(tempwaveform);
        }
    });

    // 使用智能指针管理 watcher
    auto watcher = new QFutureWatcher<void>(this);
    watcher->setFuture(future);
    
    connect(watcher, &QFutureWatcher<void>::finished, this, [this, watcher]() {
        QMetaObject::invokeMethod(this, [this]() {
            display_port_info(pxi5711waveform, pxi5322waveform, pxi5323waveform);
        }, Qt::QueuedConnection);
        watcher->deleteLater();
    });
}


void AddTask::on_pbaddcollectport_clicked()
{
    if(ui->comboBox_collectdevice->currentIndex() <= 1)
    {
        step_id = taskid + "_" + QString::number(step_num);
        QString collectdevice = (ui->comboBox_collectdevice->currentIndex() == 0) ? "5323" : "5322";
        PXIe5320Waveform waveform;
        waveform.id = step_id + "_" + collectdevice + "_" + QString::number(ui->comboBox_collectport->currentIndex());
        waveform.step_id = step_id;
        waveform.device = (ui->comboBox_collectdevice->currentIndex() == 0) ? 5323 : 5322;
        waveform.port = ui->comboBox_collectport->currentIndex();
        waveform.data = {};

        // 检查重复通道
        {
            QMutexLocker locker(&waveformMutex);
            if(waveform.device == 5322)
            {
                auto it = std::find_if(pxi5322waveform.begin(), pxi5322waveform.end(), 
                    [&waveform](const PXIe5320Waveform& w) {
                        return w.port == waveform.port;
                    });
                if(it != pxi5322waveform.end())
                {
                    auto result = QMessageBox::warning(this, "警告", 
                        "通道" + QString::number(waveform.port) + "已存在。是否覆盖？", 
                        QMessageBox::Yes | QMessageBox::No);
                    if (result != QMessageBox::Yes) {
                        return;
                    }
                    pxi5322waveform.erase(it);
                }
            }
            else if(waveform.device == 5323)
            {
                auto it = std::find_if(pxi5323waveform.begin(), pxi5323waveform.end(), 
                    [&waveform](const PXIe5320Waveform& w) {
                        return w.port == waveform.port;
                    });
                if(it != pxi5323waveform.end())
                {   
                    auto result = QMessageBox::warning(this, "警告", 
                        "通道" + QString::number(waveform.port) + "已存在。是否覆盖？", 
                        QMessageBox::Yes | QMessageBox::No);
                    if (result != QMessageBox::Yes) {
                        return;
                    }
                    pxi5323waveform.erase(it);
                }
            }
        }

        // 创建异步任务
        QFuture<void> future = QtConcurrent::run([this, waveform]() {
            while(select_location_id == 0)
            {
                QMetaObject::invokeMethod(this, "showLocationSelectionTip", 
                    Qt::QueuedConnection,
                    Q_ARG(QString, "请在图中选择端口接线位置"));
                    
                std::unique_lock<std::mutex> lock(setlocationMutex);
                setlocationCondition.wait(lock);

                QMetaObject::invokeMethod(this, [this](){
                    if(currentTip) {
                        currentTip->close();
                        currentTip = nullptr;
                    }
                }, Qt::QueuedConnection);
            }

            {
                QMutexLocker locker(&waveformMutex);
                PXIe5320Waveform tempwaveform = waveform;
                tempwaveform.positive_connect_location = select_location_id;
                select_location_id = 0;
                if(tempwaveform.device == 5322) {
                    pxi5322waveform.push_back(tempwaveform);
                } else if(tempwaveform.device == 5323) {
                    pxi5323waveform.push_back(tempwaveform);
                }
            }
        });

        // 使用智能指针管理 watcher
        auto watcher = new QFutureWatcher<void>(this);
        watcher->setFuture(future);
        
        connect(watcher, &QFutureWatcher<void>::finished, this, [this, watcher]() {
            QMetaObject::invokeMethod(this, [this]() {
                display_port_info(pxi5711waveform, pxi5322waveform, pxi5323waveform);
            }, Qt::QueuedConnection);
            watcher->deleteLater();
        });
    }
    else  // 8902设备
    {
        step_id = taskid + "_" + QString::number(step_num);
        Data8902 data;
        data.id = step_id + "_" + "8902" + "_" + ui->comboBox_collectport->currentText();
        data.step_id = step_id;
        data.test_type = ui->comboBox_collectport->currentText();

        // 创建异步任务
        QFuture<void> future = QtConcurrent::run([this, data]() {
            Data8902 tempdata = data;
            
            // 选择正极位置
            while(select_location_id == 0)
            {
                QMetaObject::invokeMethod(this, "showLocationSelectionTip", 
                    Qt::QueuedConnection,
                    Q_ARG(QString, "请选择正极接线位置"));
                    
                std::unique_lock<std::mutex> lock(setlocationMutex);
                setlocationCondition.wait(lock);

                QMetaObject::invokeMethod(this, [this](){
                    if(currentTip) {
                        currentTip->close();
                        currentTip = nullptr;
                    }
                }, Qt::QueuedConnection);
            }
            tempdata.positive_connect_location = select_location_id;
            select_location_id = 0;

            // 选择负极位置
            while(select_location_id == 0)
            {
                QMetaObject::invokeMethod(this, "showLocationSelectionTip", 
                    Qt::QueuedConnection,
                    Q_ARG(QString, "请选择负极接线位置"));
                    
                std::unique_lock<std::mutex> lock(setlocationMutex);
                setlocationCondition.wait(lock);

                QMetaObject::invokeMethod(this, [this](){
                    if(currentTip) {
                        currentTip->close();
                        currentTip = nullptr;
                    }
                }, Qt::QueuedConnection);
            }
            tempdata.negative_connect_location = select_location_id;
            select_location_id = 0;

            {
                QMutexLocker locker(&waveformMutex);
                data8902.push_back(tempdata);
            }
        });

        // 使用智能指针管理 watcher
        auto watcher = new QFutureWatcher<void>(this);
        watcher->setFuture(future);
        
        connect(watcher, &QFutureWatcher<void>::finished, this, [this, watcher]() {
            QMetaObject::invokeMethod(this, [this]() {
                display_port_info(pxi5711waveform, pxi5322waveform, pxi5323waveform);
            }, Qt::QueuedConnection);
            watcher->deleteLater();
        });
    }
}

void AddTask::showLocationSelectionTip(const QString& message) {
    QLabel* tip = new QLabel(nullptr, Qt::ToolTip | Qt::FramelessWindowHint);
    tip->setAttribute(Qt::WA_DeleteOnClose);
    tip->setAttribute(Qt::WA_TranslucentBackground);
    
    // 设置样式
    tip->setStyleSheet(
        "QLabel {"
        "   background-color: rgba(50, 50, 50, 153);"  // 设置透明度为60%
        "   color: black;" 
        "   padding: 15px;"
        "   border-radius: 10px;"
        "   font-size: 18px;" 
        "   border: 2px solid transparent;" 
        "}"
        "QLabel:hover {"
        "   border: 2px solid red;" 
        "}"
    );
    
    tip->setText(message);
    tip->adjustSize();

    // 将提示框移动到屏幕中央偏上位置
    QRect screenGeometry = QApplication::primaryScreen()->geometry();
    int x = (screenGeometry.width() - tip->width()) / 2;
    int y = screenGeometry.height() / 3;
    tip->move(x, y);
    
    // 添加淡入效果
    QGraphicsOpacityEffect* opacity = new QGraphicsOpacityEffect(tip);
    tip->setGraphicsEffect(opacity);
    QPropertyAnimation* animation = new QPropertyAnimation(opacity, "opacity");
    animation->setDuration(200);
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->start(QAbstractAnimation::DeleteWhenStopped);

    // 添加边框闪烁效果
    QPropertyAnimation* borderAnimation = new QPropertyAnimation(tip, "styleSheet");
    borderAnimation->setDuration(500);
    borderAnimation->setLoopCount(-1);  // 无限循环
    borderAnimation->setKeyValueAt(0, "QLabel { border: 2px solid red; }");
    borderAnimation->setKeyValueAt(0.5, "QLabel { border: 2px solid transparent; }");
    borderAnimation->setKeyValueAt(1, "QLabel { border: 2px solid red; }");
    borderAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    
    tip->show();
    
    if(currentTip) {
        currentTip->close();
    }
    currentTip = tip;
    ui->pbselectlocation->setEnabled(true);
}

void AddTask::on_pbaddstep_clicked()
{
    if(pxi5711waveform.empty() && pxi5322waveform.empty() && pxi5323waveform.empty() && data8902.empty())
    {
        QMessageBox::warning(this, "错误", "没有波形数据，无法保存");
        return;
    }else if(pxi5322waveform.empty() && pxi5323waveform.empty())
    {
        QMessageBox::warning(this, "错误", "必须含有采集端口！");
        return;
    }
    ui->pbaddstep->setEnabled(false);
    Step step;
    step.id = step_id;
    step.test_task_id = taskid;
    step.step_number = step_num;
    QTime collecttime = ui->Steptime->time();
    step.collecttime = collecttime.second() + collecttime.msec() / 1000.0;
    step.continue_step = (ui->comboBox_ispaused->currentText() == "是") ? false : true;
    step.isthermometry = (ui->comboBox_isthermometry->currentText() == "是") ? true : false;
    step.thermometry_pause_time = ui->doubleSpinBox_pausetime->value();
    QString table_name_step = device_id + "$$Step";
    database.insert_step(table_name_step, step);

    for(auto &waveform : pxi5711waveform)
    {
        QString table_name_pxie5711 = device_id + "$$PXIe5711";
        database.insert_pxie5711waveform(table_name_pxie5711, waveform);
    }
    for(auto &waveform : pxi5322waveform)
    {
        QString table_name_pxie5320 = device_id + "$$PXIe5320";
        database.insert_pxie5320waveform(table_name_pxie5320, waveform);
    }
    for(auto &waveform : pxi5323waveform)
    {
        QString table_name_pxie5320 = device_id + "$$PXIe5320";
        database.insert_pxie5320waveform(table_name_pxie5320, waveform);
    }
    for(auto &data : data8902)
    {
        QString table_name_8902 = device_id + "$$pxie8902";
        database.insert_8902data(table_name_8902, data);
    }
    ui->pbaddstep->setEnabled(true);
    step_num++;
    pxi5711waveform.clear();
    pxi5322waveform.clear();
    pxi5323waveform.clear();
    data8902.clear();
    display_port_info(pxi5711waveform, pxi5322waveform, pxi5323waveform);
    update_treeview();
}


void AddTask::on_pbaddagain_clicked()
{
    pxi5711waveform.clear();
    pxi5322waveform.clear();
    pxi5323waveform.clear();
    display_port_info(pxi5711waveform, pxi5322waveform, pxi5323waveform);
}

void AddTask::display_port_info(std::vector<PXIe5711Waveform> &pxi5711waveform_display, std::vector<PXIe5320Waveform> &pxi5322waveform_display, std::vector<PXIe5320Waveform> &pxi5323waveform_display)
{
    clearTableWidgets(ui->Tableoutputport);
    clearTableWidgets(ui->Tablecollectport);
    ui->Tableoutputport->setRowCount(pxi5711waveform_display.size());
    ui->Tablecollectport->setRowCount(pxi5322waveform_display.size() + pxi5323waveform_display.size());
    ui->Tableoutputport->setColumnCount(7);
    ui->Tablecollectport->setColumnCount(4);
    QStringList header;
    header << "ID" << "通道" << "波形类型" << "幅值" << "频率" << "占空比" << "步骤ID" << "更新" << "删除";
    ui->Tableoutputport->setColumnCount(header.size());
    ui->Tableoutputport->setHorizontalHeaderLabels(header);
    ui->Tableoutputport->setContextMenuPolicy(Qt::CustomContextMenu);
    header.clear();
    header << "ID" << "设备" << "端口" << "步骤ID" << "删除";
    ui->Tablecollectport->setColumnCount(header.size());
    ui->Tablecollectport->setHorizontalHeaderLabels(header);
    ui->Tablecollectport->setContextMenuPolicy(Qt::CustomContextMenu);
    int row = 0;
    for(auto &waveform : pxi5711waveform_display)
    {
        ui->Tableoutputport->setItem(row, 0, new QTableWidgetItem(waveform.id));
        ui->Tableoutputport->item(row, 0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->Tableoutputport->setItem(row, 1, new QTableWidgetItem(QString::number(waveform.channel)));
        ui->Tableoutputport->item(row, 1)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->Tableoutputport->setItem(row, 3, new QTableWidgetItem(QString::number(waveform.amplitude)));
        ui->Tableoutputport->setItem(row, 4, new QTableWidgetItem(QString::number(waveform.frequency)));
        ui->Tableoutputport->setItem(row, 5, new QTableWidgetItem(QString::number(waveform.dutyCycle)));
        ui->Tableoutputport->setItem(row, 6, new QTableWidgetItem(waveform.step_id));
        ui->Tableoutputport->item(row, 6)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

        QComboBox *comboBox = new QComboBox();
        comboBox->addItems({"LowLevelWave", "HighLevelWave", "SineWave", "SquareWave", "TriangleWave"});
        comboBox->setCurrentText(waveform.waveform_type);
        ui->Tableoutputport->setCellWidget(row, 2, comboBox);

        QPushButton *buttonUpdate = new QPushButton("更新");
        QPushButton *buttonDelete = new QPushButton("删除");
        ui->Tableoutputport->setCellWidget(row, 7, buttonUpdate);
        ui->Tableoutputport->setCellWidget(row, 8, buttonDelete);
        connect(buttonUpdate, &QPushButton::clicked, this, [this, &waveform, row]() {
            waveform.id = ui->Tableoutputport->item(row, 0)->text();
            waveform.channel = ui->Tableoutputport->item(row, 1)->text().toInt();
            QComboBox *combo = qobject_cast<QComboBox*>(ui->Tableoutputport->cellWidget(row, 2));
            waveform.waveform_type = combo->currentText();
            if(ui->Tableoutputport->item(row, 3)->text().toDouble() > 10 || ui->Tableoutputport->item(row, 3)->text().toDouble() < 0)
            {
                QMessageBox::warning(this, "幅值输入错误", "幅值应在0-10之间");
                return;
            }
            waveform.amplitude = ui->Tableoutputport->item(row, 3)->text().isEmpty() ? 0 : ui->Tableoutputport->item(row, 3)->text().toDouble();
            waveform.frequency = ui->Tableoutputport->item(row, 4)->text().isEmpty() ? 0 : ui->Tableoutputport->item(row, 4)->text().toDouble();
            waveform.dutyCycle = ui->Tableoutputport->item(row, 5)->text().isEmpty() ? 0 : ui->Tableoutputport->item(row, 5)->text().toDouble();
            waveform.step_id = ui->Tableoutputport->item(row, 6)->text();
        });

        connect(buttonDelete, &QPushButton::clicked, this, [this, &pxi5711waveform_display, &waveform, row]() {
            auto it = std::find_if(pxi5711waveform_display.begin(), pxi5711waveform_display.end(), [&waveform](const PXIe5711Waveform& w) {
                return w.id == waveform.id;
            });
            if(it != pxi5711waveform_display.end())
            {
                pxi5711waveform_display.erase(it);
                display_port_info(pxi5711waveform, pxi5322waveform, pxi5323waveform);
            }
        });
        row++;
    }
    int collectrow = 0;
    for(auto &waveform : pxi5322waveform_display)
    {
        ui->Tablecollectport->setItem(collectrow, 0, new QTableWidgetItem(waveform.id));
        ui->Tablecollectport->item(collectrow, 0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->Tablecollectport->setItem(collectrow, 1, new QTableWidgetItem("5322"));
        ui->Tablecollectport->item(collectrow, 1)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->Tablecollectport->setItem(collectrow, 2, new QTableWidgetItem(QString::number(waveform.port)));
        ui->Tablecollectport->item(collectrow, 2)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->Tablecollectport->setItem(collectrow, 3, new QTableWidgetItem(waveform.step_id));
        ui->Tablecollectport->item(collectrow, 3)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        QPushButton *buttonDelete = new QPushButton("删除");
        ui->Tablecollectport->setCellWidget(collectrow, 4, buttonDelete);
        connect(buttonDelete, &QPushButton::clicked, this, [this, &pxi5322waveform_display, &waveform, collectrow]() {
            auto it = std::find_if(pxi5322waveform_display.begin(), pxi5322waveform_display.end(), [&waveform](const PXIe5320Waveform& w) {
                return w.id == waveform.id;
            });
            if(it != pxi5322waveform_display.end())
            {
                pxi5322waveform_display.erase(it);
                display_port_info(pxi5711waveform, pxi5322waveform, pxi5323waveform);
            }
        });
        collectrow++;
    }
    for(auto &waveform : pxi5323waveform_display)
    {
        ui->Tablecollectport->setItem(collectrow, 0, new QTableWidgetItem(waveform.id));
        ui->Tablecollectport->item(collectrow, 0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->Tablecollectport->setItem(collectrow, 1, new QTableWidgetItem("5323"));
        ui->Tablecollectport->item(collectrow, 1)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->Tablecollectport->setItem(collectrow, 2, new QTableWidgetItem(QString::number(waveform.port)));
        ui->Tablecollectport->item(collectrow, 2)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->Tablecollectport->setItem(collectrow, 3, new QTableWidgetItem(waveform.step_id));
        ui->Tablecollectport->item(collectrow, 3)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        QPushButton *buttonDelete = new QPushButton("删除");
        ui->Tablecollectport->setCellWidget(collectrow, 4, buttonDelete);
        connect(buttonDelete, &QPushButton::clicked, this, [this, &pxi5323waveform_display, &waveform, collectrow]() {
            auto it = std::find_if(pxi5323waveform_display.begin(), pxi5323waveform_display.end(), [&waveform](const PXIe5320Waveform& w) {
                return w.id == waveform.id;
            });
            if(it != pxi5323waveform_display.end())
            {
                pxi5323waveform_display.erase(it);
                display_port_info(pxi5711waveform, pxi5322waveform, pxi5323waveform);
            }
        });
        collectrow++;
    }
}

void AddTask::on_pbsave_clicked()
{
    if(!pxi5711waveform.empty() || !pxi5322waveform.empty() || !pxi5323waveform.empty())
    {
        QMessageBox::StandardButton result = QMessageBox::question(this, "提示", "当前步骤未保存，是否忽略？", QMessageBox::Yes | QMessageBox::No);
        if(result == QMessageBox::No)
        {
            return;
        }
    }
    InitComboBox();
}

void AddTask::update_treeview()
{
    for(int i = 0; i < ui->treeWidget_taskinfo->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *item = ui->treeWidget_taskinfo->topLevelItem(i);
        item->treeWidget()->disconnect();
    }
    ui->treeWidget_taskinfo->clear();
    ui->treeWidget_taskinfo->setHeaderLabels(QStringList() << "任务id");

    if (taskid.isEmpty() || device_id.isEmpty()) {
        QMessageBox::warning(this, "错误", "任务ID或设备ID为空，无法更新视图。");
        return;
    }

    QTreeWidgetItem *taskItem = new QTreeWidgetItem(ui->treeWidget_taskinfo);
    taskItem->setText(0, taskid);
    taskItem->setExpanded(true);  // 展开任务节点

    std::vector<Step> steps;
    QString condition = QString("test_task_id = '%1'").arg(taskid);
    QString table_name_step = device_id + "$$Step";
    if(database.get_step(table_name_step, condition, steps))
    {
        for(auto &step : steps)
        {
            QTreeWidgetItem *stepItem = new QTreeWidgetItem(taskItem);
            stepItem->setText(0, "步骤" + QString::number(step.step_number));
            stepItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            setupStepContextMenu(stepItem, step, table_name_step, taskid);

            QTreeWidgetItem *pxie5711Item = new QTreeWidgetItem(stepItem);
            pxie5711Item->setText(0, "5711");

            QTreeWidgetItem *pxie5322Item = new QTreeWidgetItem(stepItem);
            pxie5322Item->setText(0, "5322");

            QTreeWidgetItem *pxie5323Item = new QTreeWidgetItem(stepItem);
            pxie5323Item->setText(0, "5323");

            populateWaveformItems(pxie5711Item, step, device_id + "$$pxie5711", true);
            populateWaveformItems(pxie5322Item, step, device_id + "$$pxie5320", false);
        }
    }
    else {
        QMessageBox::warning(this, "错误", "无法获取步骤信息。");
    }
    ui->treeWidget_taskinfo->addTopLevelItem(taskItem);
}

void AddTask::setupStepContextMenu(QTreeWidgetItem *item, const Step &step, const QString &tableName, const QString taskid_tree)
{
    item->treeWidget()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(item->treeWidget(), &QTreeWidget::customContextMenuRequested, this, [this, item, step, tableName, taskid_tree](const QPoint &pos) {
        if (item->treeWidget()->itemAt(pos) == item) {
            QMenu contextMenu(tr("上下文菜单"), this);
            QAction *actionDelete = new QAction("删除", this);
            QAction *actionEdit = new QAction("查看/编辑", this);
            QAction *actionExecute = new QAction("执行", this);
            contextMenu.addAction(actionDelete);
            contextMenu.addAction(actionEdit);
            contextMenu.addAction(actionExecute);
            connect(actionDelete, &QAction::triggered, this, [this, step, tableName, item, actionDelete]() {
                QString ErrorMessage;
                if(!database.delete_step(tableName, step.id, ErrorMessage))
                {
                    QMessageBox::warning(this, "删除步骤失败", ErrorMessage);
                }
                else
                {
                    update_treeview();
                }
            });
            connect(actionEdit, &QAction::triggered, this, [this, step, tableName, item, actionEdit]() {
                edit_step(step, tableName);
            });
            connect(actionExecute, &QAction::triggered, this, [this, step, tableName, item, actionExecute, taskid_tree]() {
                SingleStepRun(step, tableName);
            });
            contextMenu.exec(item->treeWidget()->viewport()->mapToGlobal(pos));
        }
    });
}

void AddTask::populateWaveformItems(QTreeWidgetItem *parentItem, const Step &step, const QString &tableName, bool is5711)
{
    std::vector<PXIe5711Waveform> pxi5711waveforms;
    std::vector<PXIe5320Waveform> pxi5320waveforms;
    QString condition = QString("step_id = '%1'").arg(step.id);
    if (is5711 && database.get_pxie5711waveform(tableName, condition, pxi5711waveforms))
    {
        for(auto &waveform : pxi5711waveforms)
        {
            QTreeWidgetItem *waveformItem = new QTreeWidgetItem(parentItem);
            waveformItem->setText(0, QString::number(waveform.channel) + " " + waveform.waveform_type + " " + QString::number(waveform.amplitude));
        }
    }
    else if (!is5711 && database.get_pxie5320waveform(tableName, condition, pxi5320waveforms))
    {
        for(auto &waveform : pxi5320waveforms)
        {
            QTreeWidgetItem *waveformItem = new QTreeWidgetItem((waveform.device == 5322) ? parentItem : parentItem->parent()->child(2));
            waveformItem->setText(0, "端口" + QString::number(waveform.port));
        }
    }
}

void AddTask::edit_step(const Step &step, const QString &tableName)
{
    clearTableWidgets(ui->Tableoutputport);
    clearTableWidgets(ui->Tablecollectport);
    setupTableHeaders();
    auto waveforms = loadWaveforms(step, tableName);
    if (waveforms.first.empty() && waveforms.second.empty()) return;

    populateTableOutputPort(waveforms.first, tableName);
    populateTableCollectPort(waveforms.second, tableName);
}

void AddTask::clearTableWidgets(QTableWidget *table)
{
    table->clear();
    for (int i = 0; i < table->rowCount(); ++i) {
        for (int j = 0; j < table->columnCount(); ++j) {
            QWidget *widget = table->cellWidget(i, j);
            if (widget) {
                table->removeCellWidget(i, j);
                delete widget;
            }
        }
    }
    table->setRowCount(0);
}

std::pair<std::vector<PXIe5711Waveform>, std::vector<PXIe5320Waveform>> AddTask::loadWaveforms(const Step &step, const QString &tableName)
{
    std::vector<PXIe5711Waveform> pxi5711waveforms;
    std::vector<PXIe5320Waveform> pxi5320waveforms;
    QString condition = QString("step_id = '%1'").arg(step.id);
    QString device_id = tableName.split("$$")[0];
    QString table_name_pxie5711 = device_id + "$$pxie5711";
    QString table_name_pxie5320 = device_id + "$$pxie5320";

    if (database.get_pxie5711waveform(table_name_pxie5711, condition, pxi5711waveforms) &&
        database.get_pxie5320waveform(table_name_pxie5320, condition, pxi5320waveforms))
    {
        return {pxi5711waveforms, pxi5320waveforms};
    }
    return {{}, {}};
}

void AddTask::setupTableHeaders()
{
    QStringList headerOutput = {"ID", "通道", "波形类型", "幅值", "频率", "占空比", "步骤ID", "更新", "删除"};
    QStringList headerCollect = {"ID", "设备", "端口", "步骤ID", "删除"};

    ui->Tableoutputport->setRowCount(0);
    ui->Tablecollectport->setRowCount(0);
    ui->Tableoutputport->setColumnCount(headerOutput.size());
    ui->Tablecollectport->setColumnCount(headerCollect.size());
    ui->Tableoutputport->setHorizontalHeaderLabels(headerOutput);
    ui->Tablecollectport->setHorizontalHeaderLabels(headerCollect);
    ui->Tableoutputport->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->Tablecollectport->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->Tableoutputport->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->Tablecollectport->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->Tableoutputport->horizontalHeader()->setStretchLastSection(true);
    ui->Tablecollectport->horizontalHeader()->setStretchLastSection(true);
}

void AddTask::populateTableOutputPort(const std::vector<PXIe5711Waveform>& waveforms, const QString &tableName)
{
    int row = 0;
    for (const auto &waveform : waveforms)
    {
        populateOutputRow(row, waveform, tableName);
        row++;
    }
}

void AddTask::populateTableCollectPort(const std::vector<PXIe5320Waveform>& waveforms, const QString &tableName)
{
    int row = 0;
    for (const auto &waveform : waveforms)
    {
        populateCollectRow(row, waveform, tableName);
        row++;
    }
}

void AddTask::populateOutputRow(int row, const PXIe5711Waveform &waveform, const QString &tableName)
{
    ui->Tableoutputport->insertRow(row);
    ui->Tableoutputport->setItem(row, 0, new QTableWidgetItem(waveform.id));
    ui->Tableoutputport->item(row, 0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    ui->Tableoutputport->setItem(row, 1, new QTableWidgetItem(QString::number(waveform.channel)));
    ui->Tableoutputport->item(row, 1)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    ui->Tableoutputport->setItem(row, 3, new QTableWidgetItem(QString::number(waveform.amplitude)));
    ui->Tableoutputport->setItem(row, 4, new QTableWidgetItem(QString::number(waveform.frequency)));
    ui->Tableoutputport->setItem(row, 5, new QTableWidgetItem(QString::number(waveform.dutyCycle)));
    ui->Tableoutputport->setItem(row, 6, new QTableWidgetItem(waveform.step_id));
    ui->Tableoutputport->item(row, 6)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QComboBox *comboBox = new QComboBox();
    comboBox->addItems({"LowLevelWave", "HighLevelWave", "SineWave", "SquareWave", "TriangleWave"});
    comboBox->setCurrentText(waveform.waveform_type);
    ui->Tableoutputport->setCellWidget(row, 2, comboBox);

    QPushButton *buttonUpdate = new QPushButton("更新");
    QPushButton *buttonDelete = new QPushButton("删除");
    ui->Tableoutputport->setCellWidget(row, 7, buttonUpdate);
    ui->Tableoutputport->setCellWidget(row, 8, buttonDelete);
    setupOutputPortConnections(row, waveform, tableName);
}

void AddTask::populateCollectRow(int row, const PXIe5320Waveform &waveform, const QString &tableName)
{
    ui->Tablecollectport->insertRow(row);
    ui->Tablecollectport->setItem(row, 0, new QTableWidgetItem(waveform.id));
    ui->Tablecollectport->item(row, 0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    ui->Tablecollectport->setItem(row, 1, new QTableWidgetItem(QString::number(waveform.device)));
    ui->Tablecollectport->item(row, 1)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    ui->Tablecollectport->setItem(row, 2, new QTableWidgetItem(QString::number(waveform.port)));
    ui->Tablecollectport->item(row, 2)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    ui->Tablecollectport->setItem(row, 3, new QTableWidgetItem(waveform.step_id));
    ui->Tablecollectport->item(row, 3)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QPushButton *buttonDelete = new QPushButton("删除");
    ui->Tablecollectport->setCellWidget(row, 4, buttonDelete);
    setupCollectPortConnections(row, waveform, tableName);
}

void AddTask::setupOutputPortConnections(int row, const PXIe5711Waveform &waveform, const QString &tableName)
{
    QPushButton *buttonUpdate = qobject_cast<QPushButton*>(ui->Tableoutputport->cellWidget(row, 7));
    QPushButton *buttonDelete = qobject_cast<QPushButton*>(ui->Tableoutputport->cellWidget(row, 8));
    connect(buttonUpdate, &QPushButton::clicked, this, [this, waveform, row, tableName]() { updateWaveform(waveform, row, tableName); });
    connect(buttonDelete, &QPushButton::clicked, this, [this, waveform, tableName]() { deleteWaveform(waveform, tableName); });
}

void AddTask::setupCollectPortConnections(int row, const PXIe5320Waveform &waveform, const QString &tableName)
{
    QPushButton *buttonDelete = qobject_cast<QPushButton*>(ui->Tablecollectport->cellWidget(row, 4));
    connect(buttonDelete, &QPushButton::clicked, this, [this, waveform, tableName]() { deleteWaveform(waveform, tableName); });
}

void AddTask::updateWaveform(const PXIe5711Waveform &waveform, int row, const QString &tableName)
{
    PXIe5711Waveform waveform_update;
    waveform_update.id = ui->Tableoutputport->item(row, 0)->text();
    waveform_update.channel = ui->Tableoutputport->item(row, 1)->text().toInt();
    QComboBox *combo = qobject_cast<QComboBox*>(ui->Tableoutputport->cellWidget(row, 2));
    waveform_update.waveform_type = combo->currentText();
    if(ui->Tableoutputport->item(row, 3)->text().toDouble() > 10 || ui->Tableoutputport->item(row, 3)->text().toDouble() < 0)
    {
        QMessageBox::warning(this, "幅值输入错误", "幅值应在0-10之间");
        return;
    }
    waveform_update.amplitude = ui->Tableoutputport->item(row, 3)->text().isEmpty() ? 0 : ui->Tableoutputport->item(row, 3)->text().toDouble();
    waveform_update.frequency = ui->Tableoutputport->item(row, 4)->text().isEmpty() ? 0 : ui->Tableoutputport->item(row, 4)->text().toDouble();
    waveform_update.dutyCycle = ui->Tableoutputport->item(row, 5)->text().isEmpty() ? 0 : ui->Tableoutputport->item(row, 5)->text().toDouble();
    waveform_update.step_id = ui->Tableoutputport->item(row, 6)->text();
    QString table_name_pxie5711 = tableName.split("$$")[0] + "$$pxie5711";
    if(!database.update_pxie5711waveform(table_name_pxie5711, waveform_update))
    {
        return;
    }
    Step step;
    step.id = waveform_update.step_id;
    edit_step(step, tableName);
    QSystemTrayIcon *trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/image/OIP.jpg"));
    trayIcon->show();
    trayIcon->showMessage("更新通知", "波形已成功更新！", QSystemTrayIcon::Information, 1000);
    delete trayIcon;
}

template<typename T>
void AddTask::deleteWaveform(const T &waveform, const QString &tableName)
{
    if constexpr (std::is_same_v<T, PXIe5711Waveform>)
    {
        QString table_name_pxie5711 = tableName.split("$$")[0] + "$$pxie5711";
        if(database.delete_pxie5711waveform(table_name_pxie5711, waveform.id))
        {
            Step step;
            step.id = waveform.step_id;
            edit_step(step, tableName);
        }
    }
    else if constexpr (std::is_same_v<T, PXIe5320Waveform>)
    {
        QString table_name_pxie5320 = tableName.split("$$")[0] + "$$pxie5320";
        if(database.delete_pxie5320waveform(table_name_pxie5320, waveform.id))
        {
            Step step;
            step.id = waveform.step_id;
            edit_step(step, tableName);
        }
    }
}

void AddTask::on_pbrefreshdeviceid_clicked()
{
    QString ErrorMessage;
    label_info.clear();
    if(!database.get_deviceelement(device_id, "", label_info, ErrorMessage))
    {
        QMessageBox::warning(this, "刷新失败", ErrorMessage);
        return;
    }
    ui->cbdeviceid->clear();
    for(auto &item : label_info)
    {
        ui->cbdeviceid->addItem(QString::number(item.id));
    }
    if(!label_info.empty()) element_id = label_info[0].id;

    QString table_name_testtask = device_id + "$$TestTask";
    std::vector<TestTask> testtask;
    if(!database.get_testtask(table_name_testtask, "element_id = " + QString::number(element_id), testtask, ErrorMessage))
    {
        return;
    }
    ui->cbtesttaskid->clear();
    for(auto &item : testtask)
    {
        ui->cbtesttaskid->addItem(item.id);
    }
    initimage();
}

void AddTask::refresh_testtask()
{
    QString table_name_testtask = device_id + "$$TestTask";
    std::vector<TestTask> testtask;
    QString ErrorMessage;
    if(!database.get_testtask(table_name_testtask, "element_id = " + QString::number(element_id), testtask, ErrorMessage))
    {
        return;
    }
    ui->cbtesttaskid->clear();
    for(auto &item : testtask)
    {
        ui->cbtesttaskid->addItem(item.id);
    }
    if(!testtask.empty())
    {
        viewtask_id = testtask[0].id;
    }
}

void AddTask::on_cbdeviceid_activated(int index)
{
    element_id = ui->cbdeviceid->itemText(index).toInt();
    refresh_testtask();
}


void AddTask::on_cbtesttaskid_activated(int index)
{
    viewtask_id = ui->cbtesttaskid->itemText(index);
}


void AddTask::on_pbdelete_clicked()
{
    if(viewtask_id.isEmpty())
    {
        QMessageBox::warning(this, "提示", "任务为空");
        return;
    }
    QString table_name_testtask = device_id + "$$TestTask";
    QFuture<QString> deleteFuture = QtConcurrent::run([this, table_name_testtask, viewtask_id = this->viewtask_id]() -> QString {
        Database DeleteDb = Database("testtask_delete", nullptr);
        QString ErrorMessage;
        if(!DeleteDb.delete_testtask(table_name_testtask, viewtask_id, ErrorMessage))
        {
            DeleteDb.disconnect();
            return ErrorMessage;
        }
        DeleteDb.disconnect();
        g_FolderCheck.Delete_Folder("./CollectData/" + device_id + "/" + viewtask_id);
        return "";
    });
    QFutureWatcher<QString> *watcher = new QFutureWatcher<QString>();
    watcher->setFuture(deleteFuture);
    QObject::connect(watcher, &QFutureWatcher<QString>::finished, [this, watcher]() {
        QString ErrorMessage = watcher->future().result();
        if(!ErrorMessage.isEmpty())
        {
            QMessageBox::warning(this, "删除失败", ErrorMessage);
        }
        else
        {
            QMessageBox::information(this, "提示", "任务" + viewtask_id + "已删除");
            refresh_testtask();
        }
        watcher->deleteLater();
    });
}


void AddTask::on_pbvieworedit_clicked()
{
    taskid = ui->cbtesttaskid->currentText();
    element_id = ui->cbdeviceid->currentText().toInt();
    update_treeview();
}

void AddTask::SingleStepRun(const Step &step, const QString &tableName)
{
    if(!RunStep_id.isEmpty())
    {
        QMessageBox::warning(this, "提示", "任务正在执行中:" + RunStep_id);
        return;
    }
    if(taskisexecuting)
    {
        QMessageBox::warning(this, "提示", "设备正在运行中,请先停止当前运行任务");
        return;
    }
    taskisexecuting = true;
    RunStep_id = step.id;
    QString TableName_execute = tableName.split("$$")[0] + "$$" + step.test_task_id + "$$" + "step" + QString::number(step.step_number) + "_" + QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss");
    taskThread = std::make_shared<QThread>();
    localExecuteTask = std::make_shared<ExecuteTask>(step.test_task_id, TableName_execute, true, step);

    connect(localExecuteTask.get(), &ExecuteTask::TaskFinished, this, [this]() {
        taskThread->quit();
        taskThread->wait();
    });
    connect(localExecuteTask.get(), &ExecuteTask::StateChanged, this, [this](QString state, int time) {
        if(state == "step_collecttime")
        {
            step_collectedtime = time;
            ui->progressBar_singlestep->setValue(0);
        }else if(state == "step_collectedtime")
        {
            double progressValue = qRound((static_cast<double>(time) / static_cast<double>(step_collectedtime)) * 10000) / 100.0;
            ui->progressBar_singlestep->setValue(progressValue);
        }
    });
    localExecuteTask->moveToThread(taskThread.get());
    connect(localExecuteTask.get(), &ExecuteTask::TaskFinished, this, [this]() {
        taskThread->quit();
        taskThread->wait();
    });
    connect(taskThread.get(), &QThread::finished, this, [this]() {
        auto localTask = std::move(localExecuteTask);
        localTask.reset();
        RunStep_id.clear();
        taskisexecuting = false;
        emit UpdateMysqlDataSignal();
    });
    taskThread->start();
    localExecuteTask->StartTask();
}

void AddTask::on_pbsaveandrun_clicked()
{
    if(pxi5711waveform.empty() && pxi5322waveform.empty() && pxi5323waveform.empty())
    {
        QMessageBox::warning(this, "错误", "没有波形数据，无法保存");
        return;
    }
    ui->pbsaveandrun->setEnabled(false);
    Step step;
    step.id = step_id;
    step.test_task_id = taskid;
    step.step_number = step_num;
    QTime collecttime = ui->Steptime->time();
    step.collecttime = collecttime.second() + collecttime.msec() / 1000.0;
    step.continue_step = (ui->comboBox_ispaused->currentText() == "是") ? false : true;
    QString table_name_step = device_id + "$$Step";
    database.insert_step(table_name_step, step);

    for(auto &waveform : pxi5711waveform)
    {
        QString table_name_pxie5711 = device_id + "$$PXIe5711";
        database.insert_pxie5711waveform(table_name_pxie5711, waveform);
    }
    for(auto &waveform : pxi5322waveform)
    {
        QString table_name_pxie5320 = device_id + "$$PXIe5320";
        database.insert_pxie5320waveform(table_name_pxie5320, waveform);
    }
    for(auto &waveform : pxi5323waveform)
    {
        QString table_name_pxie5320 = device_id + "$$PXIe5320";
        database.insert_pxie5320waveform(table_name_pxie5320, waveform);
    }
    for(auto &data : data8902)
    {
        QString table_name_8902 = device_id + "$$8902";
        database.insert_8902data(table_name_8902, data);
    }
    
    SingleStepRun(step, table_name_step);
    ui->pbsaveandrun->setEnabled(true);
    step_num++;
    pxi5711waveform.clear();
    pxi5322waveform.clear();
    pxi5323waveform.clear();
    display_port_info(pxi5711waveform, pxi5322waveform, pxi5323waveform);
    update_treeview();
}

void AddTask::on_pbcancel_singlestep_clicked()
{
    if(RunStep_id.isEmpty())
    {
        return;
    }
    localExecuteTask->InterruptTask();
}


void AddTask::on_pbshowpcbimage_clicked()
{
    QString ErrorMessage;
    std::vector<Device> devices;
    label_info_selectid.clear();
    if(!database.get_device("id = '" + device_id + "'", devices, true))
    {
        return;
    }
    if(!database.get_deviceelement(device_id, "", label_info_selectid, ErrorMessage))
    {
        QMessageBox::warning(this, "设备元素查询失败", ErrorMessage);
        return;
    }

    LabeledingImage_selectid = QImage::fromData(devices[0].image);
    labelediting_selectid = std::make_shared<LabelEditingWindow>(nullptr, LabeledingImage_selectid, label_info_selectid, label_info_add_selectid, delete_id_selectid);
    labelediting_selectid->setSelectModel();
    connect(labelediting_selectid.get(), &LabelEditingWindow::window_close, this, [this]() {
        int select_id = labelediting_selectid->getSelectedLabelId();
        for(int i = 0; i < ui->cbdeviceid->count(); i++)
        {
            QString text = ui->cbdeviceid->itemText(i);
            if(text.toInt() == select_id)
            {
                ui->cbdeviceid->setCurrentIndex(i);
                on_cbdeviceid_activated(i);
                break;
            }
        }
    });
    labelediting_selectid->show();
}


void AddTask::on_pbselectlocation_clicked()
{
    select_location_id = labelediting->getSelectedLabelId();
    setlocationCondition.notify_one();
    ui->pbselectlocation->setEnabled(false);
}

