#include "managetask.h"
#include "ui_managetask.h"
#include <QStandardItem>
#include <QMessageBox>
#include <QGraphicsLayout>
#include <QDebug>
#include <QStackedWidget>
#include <QtConcurrent>

extern bool taskisexecuting;
extern QString current_device_id;

ManageTask::ManageTask(QMainWindow *parent)
    : parent(parent)
    , ui(new Ui::ManageTask)
    , database("managetask", this)
    , infraredcamera(Camera::getInstance(nullptr, true))
    , device_id(current_device_id)
{
    ui->setupUi(this);
    
    stackedWidget_connectwire = new QStackedWidget(this);
    
    taskConnectWire = std::make_shared<TaskConnectWire>(this, stackedWidget_connectwire);
    taskConnectWire->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    stackedWidget_connectwire->addWidget(taskConnectWire.get());

    stackedWidget_connectwire->addWidget(ui->widget_2);
    stackedWidget_connectwire->setCurrentIndex(1);
    ui->gridLayout_wireconnect->addWidget(stackedWidget_connectwire, 0, 0);
    ComboBoxInit();

    irimagedisplay_temp = new IRImageDisplay(this);
    irimagedisplay = new IRImageDisplay(this);

    ui->gridLayout_image->addWidget(irimagedisplay_temp, 0, 0);
    ui->gridLayout_iconimage->addWidget(irimagedisplay, 0, 0);
    irimagedisplay_temp->show();
    irimagedisplay->show();
    QObject::connect(&infraredcamera, &Camera::Image_rgb, this, &ManageTask::saveinfraredframe);
    QObject::connect(&infraredcamera, &Camera::signal_close, this, [this]() {
        is_infraredcamera_save = false;
    });
    QObject::connect(&infraredcamera, &Camera::Image_ir_hypertherm, this, &ManageTask::saveinfraredframe_hypertherm);
    QObject::connect(&gDeviceId, &DeviceId::deviceChanged, this, [this]() {
        on_pbrefresh_clicked();
    });
    on_pbrefresh_clicked();
}

ManageTask::~ManageTask()
{
    delete ui;
    leds.clear();
    displaydata5322.clear();
    displaydata5323.clear();
    displaydata8902.clear();
    graph8902.clear();
    graph5322.clear();
    graph5323.clear();
    ui->treeWidget->clear();
    database.disconnect();
}

void ManageTask::ComboBoxInit()
{
    ui->comboBox_taskid->setCurrentIndex(0);

    currentTaskId = ui->comboBox_taskid->currentText();

    initializeCharts();
}

void ManageTask::initializeCharts()
{
    ui->showOrHideBtn->setText("<<");
    customPlot5322 = new UESTCQCustomPlot(this);
    customPlot5323 = new UESTCQCustomPlot(this);
    customPlot8902 = new UESTCQCustomPlot(this);

    ui->gridLayout_5322->addWidget(customPlot5322, 0, 0);
    ui->gridLayout_5323->addWidget(customPlot5323, 0, 0);
    ui->gridLayout_8902->addWidget(customPlot8902, 0, 0);

    QTimer::singleShot(0, this, [this]() {
        customPlot5322->setHorizontalScrollBar(ui->horizontalScrollBar_5322);
        customPlot5323->setHorizontalScrollBar(ui->horizontalScrollBar_5323);
        customPlot8902->setHorizontalScrollBar(ui->horizontalScrollBar_8902);
    });

    ui->pb8902expand->setIcon(QIcon(":/image/expand.png"));
    ui->pb5322expand->setIcon(QIcon(":/image/expand.png"));
    ui->pb5323expand->setIcon(QIcon(":/image/expand.png"));

    ui->pb8902expand->setIconSize(QSize(20, 20));
    ui->pb5322expand->setIconSize(QSize(20, 20));
    ui->pb5323expand->setIconSize(QSize(20, 20));
}

void ManageTask::updateTaskComboBox()
{
    std::vector<TestTask> tasks;
    QString table_name = device_id + "$$TestTask";
    QString ErrorMessage;
    if(!database.get_testtask(table_name, "element_id = " + QString::number(element_id), tasks, ErrorMessage))
    {
        QMessageBox::warning(this, "任务查询失败", ErrorMessage);
        return;
    }
    ui->comboBox_taskid->clear();
    if(tasks.empty()) return;
    currentTasks = tasks;
    for (const auto& task : tasks) {
        ui->comboBox_taskid->addItem(task.id);
    }
    currentTaskId = tasks[0].id;
}

void ManageTask::on_comboBox_taskid_activated(int index)
{
    currentTaskId = ui->comboBox_taskid->itemText(index);
}

void ManageTask::setupTreeWidget(const std::vector<Step>& steps)
{
    disconnect(ui->treeWidget, &QTreeWidget::customContextMenuRequested, this, nullptr);
    disconnect(ui->treeWidget, &QTreeWidget::itemChanged, this, nullptr);
    ui->treeWidget->clear();
    ui->treeWidget->setColumnCount(5);
    ui->treeWidget->setHeaderLabels(QStringList() << tr("任务") << tr("测试时间") << tr("步骤") << tr("设备") << tr("端口"));

    std::vector<QString> TableNames = g_FolderCheck.Get_Folder_list("./CollectData/" + device_id + "/" + currentTaskId);
    
    QTreeWidgetItem *rootItem = new QTreeWidgetItem(ui->treeWidget);
    rootItem->setText(0, currentTaskId);
    ui->treeWidget->addTopLevelItem(rootItem);

    for (int i = 0; i < TableNames.size(); ++i)
    {
        QTreeWidgetItem *timeItem = new QTreeWidgetItem(rootItem);

        QString displayText;
        if(i == 0)
        {
            displayText = "参考数据";
            display_time_id_map[TableNames[i]] = displayText;
        }else{
            displayText = "测试数据" + QString::number(i);
            display_time_id_map[TableNames[i]] = displayText;
        }

        displayText = displayText + " " + "(" + TableNames[i] + ")";
        timeItem->setText(1, displayText);
        timeItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
        timeItem->setCheckState(1, Qt::Unchecked);
        setupStepContextMenu(timeItem, device_id + "/" + currentTaskId + "/" + TableNames[i]);

        for (const auto& step : steps)
        {
            QTreeWidgetItem *stepItem = createStepItem(step);
            timeItem->addChild(stepItem);
            populateStepItem(stepItem, step, device_id + "/" + currentTaskId + "/" + TableNames[i]);
        }
    }

    connect(ui->treeWidget, &QTreeWidget::itemChanged, this, [this](QTreeWidgetItem* item, int column) {
        if(column == 1) { // timeItem或stepItem被选中
            Qt::CheckState state = item->checkState(column);
            // 遍历所有子节点
            for(int i = 0; i < item->childCount(); ++i) {
                QTreeWidgetItem* child = item->child(i);
                if(child->text(5) == tr("红外图像")) {
                    child->setCheckState(5, state);
                    continue;
                }
                // 设置stepItem的选中状态
                child->setCheckState(2, state);

                for(int j = 0; j < child->childCount(); ++j) {
                    QTreeWidgetItem* deviceItem = child->child(j);
                    deviceItem->setCheckState(3, state);
                    // 遍历所有波形节点
                    for(int k = 0; k < deviceItem->childCount(); ++k) {
                        QTreeWidgetItem* waveformItem = deviceItem->child(k);
                        waveformItem->setCheckState(4, state);
                    }
                }
            }
        }
        else if(column == 2) { // stepItem被选中
            Qt::CheckState state = item->checkState(column);
            // 遍历所有设备节点
            for(int i = 0; i < item->childCount(); ++i) {
                QTreeWidgetItem* deviceItem = item->child(i);
                deviceItem->setCheckState(3, state);
                // 遍历所有波形节点
                for(int j = 0; j < deviceItem->childCount(); ++j) {
                    QTreeWidgetItem* waveformItem = deviceItem->child(j);
                    waveformItem->setCheckState(4, state);
                }
            }
        }else if(column == 3)
        {
            Qt::CheckState state = item->checkState(column);
            for(int i = 0; i < item->childCount(); ++i) {
                QTreeWidgetItem* waveformItem = item->child(i);
                waveformItem->setCheckState(4, state);
            }
        }
    });
}

void ManageTask::setupStepContextMenu(QTreeWidgetItem *item, const QString &tableName)
{
    item->treeWidget()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(item->treeWidget(), &QTreeWidget::customContextMenuRequested, this, [this, item, tableName](const QPoint &pos) {
        QTreeWidgetItem* selectedItem = item->treeWidget()->itemAt(pos);
        if (selectedItem && selectedItem == item) {
            QMenu contextMenu(tr("上下文菜单"), this);
            QAction *actionDelete = new QAction(tr("删除"), this);
            contextMenu.addAction(actionDelete);
            connect(actionDelete, &QAction::triggered, this, [this, item, tableName]() {
                if (g_FolderCheck.Delete_Folder("./CollectData/" + tableName)) {
                    on_pbViewTask_clicked();
                }
                else{
                    QMessageBox::warning(this, "删除失败", "文件夹删除失败");
                }
            });
            contextMenu.exec(item->treeWidget()->viewport()->mapToGlobal(pos));
        }
    });
}

QTreeWidgetItem* ManageTask::createStepItem(const Step& step)
{
    QTreeWidgetItem *stepItem = new QTreeWidgetItem();
    stepItem->setText(2, tr("步骤%1 采样时间%2s").arg(step.step_number).arg(QString::number(step.collecttime)));
    stepItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
    stepItem->setCheckState(2, Qt::Unchecked);
    return stepItem;
}

void ManageTask::populateStepItem(QTreeWidgetItem* stepItem, const Step& step, const QString& TableName)
{
    std::vector<PXIe5320Waveform> waveforms;
    std::vector<Data8902> waveforms8902;

    QString table_name_pxie5320 = device_id + "$$PXIe5320";
    database.get_pxie5320waveform(table_name_pxie5320, "step_id = '" + step.id + "'", waveforms);
    QString table_name_pxie8902 = device_id + "$$PXIe8902";
    database.get_8902data(table_name_pxie8902, "step_id = '" + step.id + "'", waveforms8902);

    QTreeWidgetItem *pxie8902Item = new QTreeWidgetItem(stepItem, QStringList() << "" << "" << "" << "万用表");
    QTreeWidgetItem *pxie5322Item = new QTreeWidgetItem(stepItem, QStringList() << "" << "" << "" << "数字输入");
    QTreeWidgetItem *pxie5323Item = new QTreeWidgetItem(stepItem, QStringList() << "" << "" << "" << "模拟输入");
    QTreeWidgetItem *irItem = new QTreeWidgetItem(stepItem, QStringList() << "" << "" << "" << "红外图像");

    pxie8902Item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
    pxie5322Item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
    pxie5323Item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
    irItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
    irItem->setData(0, Qt::UserRole, QVariant::fromValue(QPair<QString, QString>(step.id, TableName)));


    pxie8902Item->setCheckState(3, Qt::Unchecked);
    pxie5322Item->setCheckState(3, Qt::Unchecked);
    pxie5323Item->setCheckState(3, Qt::Unchecked);
    irItem->setCheckState(3, Qt::Unchecked);


    addWaveformsToTree(pxie5322Item, waveforms, 5322, TableName);
    addWaveformsToTree(pxie5323Item, waveforms, 5323, TableName);
    addWaveforms8902ToTree(pxie8902Item, waveforms8902, TableName);
    connectIRItem(irItem);
}

void ManageTask::connectIRItem(QTreeWidgetItem* item)
{
    connect(ui->treeWidget, &QTreeWidget::itemChanged, this, [this, item](QTreeWidgetItem* changedItem, int column) {
        if (changedItem == item && column == 3) {
            if(changedItem->checkState(column) == Qt::Checked)
            {
                QString tableName = item->data(0, Qt::UserRole).value<QPair<QString, QString>>().second;
                QString stepId = item->data(0, Qt::UserRole).value<QPair<QString, QString>>().first;
                handleIRItemChange(tableName, stepId);
            }
        }
    });
}

void ManageTask::handleIRItemChange(const QString& tableName, const QString& stepId)
{
    QString table_name_irimage = tableName.split("/")[0] + "$$image";
    QString ErrorMessage;
    std::vector<Image> irimage;
    if(display_time_id_map[tableName.split("/")[2]].contains("参考数据"))
        irimagedisplay_temp->clear();
    else
        irimagedisplay->clear();
    QString table_name = tableName.split("/")[1] + "_" + tableName.split("/")[2];
    if(!database.get_image(table_name_irimage, "task_table_name = '" + table_name + "' AND step_id = '" + stepId + "'", irimage, ErrorMessage))
    {
        QMessageBox::warning(this, "红外图像查询失败", ErrorMessage);
        return;
    }
    if(irimage.empty()) return;
    QImage qimage = QImage::fromData(irimage[0].image_data);
    std::vector<uint16_t> tempVector(reinterpret_cast<uint16_t*>(irimage[0].temp_data.data()),
                                                 reinterpret_cast<uint16_t*>(irimage[0].temp_data.data() + irimage[0].temp_data.size()));
    if(display_time_id_map[tableName.split("/")[2]].contains("参考数据"))
    {
        irimagedisplay_temp->setImage(qimage, tempVector, irimage[0].temp_width, irimage[0].temp_height);
    }
    else
    {
        irimagedisplay->setImage(qimage, tempVector, irimage[0].temp_width, irimage[0].temp_height);
    }
}

void ManageTask::addWaveformsToTree(QTreeWidgetItem* parentItem, const std::vector<PXIe5320Waveform>& waveforms, int deviceType, const QString& TableName)
{
    for(const auto& waveform : waveforms)
    {
        if(waveform.device == deviceType)
        {
            QTreeWidgetItem *waveformItem = createWaveformItem(waveform, TableName);
            parentItem->addChild(waveformItem);
            connectWaveformItem(waveformItem);
        }
    }
}

void ManageTask::addWaveforms8902ToTree(QTreeWidgetItem* parentItem, const std::vector<Data8902>& waveforms8902, const QString& TableName)
{
    for(const auto& waveform : waveforms8902)
    {
        QTreeWidgetItem *waveformItem = createWaveform8902Item(waveform, TableName);
        parentItem->addChild(waveformItem);
        connectWaveformItem(waveformItem);
    }
}

QTreeWidgetItem* ManageTask::createWaveformItem(const PXIe5320Waveform& waveform, const QString& tableName)
{
    QTreeWidgetItem *waveformItem = new QTreeWidgetItem();
    waveformItem->setText(4, tr("通道%1").arg(waveform.port));
    waveformItem->setFlags(waveformItem->flags() | Qt::ItemIsUserCheckable);
    waveformItem->setCheckState(4, Qt::Unchecked);
    waveformItem->setData(0, Qt::UserRole, QVariant::fromValue(QPair<QString, QString>(waveform.id, tableName)));
    return waveformItem;
}

QTreeWidgetItem* ManageTask::createWaveform8902Item(const Data8902& waveform, const QString& tableName)
{
    QTreeWidgetItem *waveformItem = new QTreeWidgetItem();
    waveformItem->setText(4, tr(waveform.test_type.toStdString().c_str()));
    waveformItem->setFlags(waveformItem->flags() | Qt::ItemIsUserCheckable);
    waveformItem->setCheckState(4, Qt::Unchecked);
    waveformItem->setData(0, Qt::UserRole, QVariant::fromValue(QPair<QString, QString>(waveform.id, tableName)));
    return waveformItem;
}

void ManageTask::connectWaveformItem(QTreeWidgetItem* item)
{
    connect(ui->treeWidget, &QTreeWidget::itemChanged, this, [this, item](QTreeWidgetItem* changedItem, int column) {
        if (changedItem == item && column == 4) {
            handleWaveformItemChange(changedItem);
        }
    });
}

void ManageTask::handleWaveformItemChange(QTreeWidgetItem* item)
{
    // 禁用勾选框直到数据加载完成
    item->setFlags(item->flags() & ~Qt::ItemIsUserCheckable);
    QString waveformId = item->data(0, Qt::UserRole).value<QPair<QString, QString>>().first;
    QString tableName = item->data(0, Qt::UserRole).value<QPair<QString, QString>>().second;
    int deviceType = waveformId.split("_").at(waveformId.split("_").size() - 2).toInt();
    if (item->checkState(4) == Qt::Checked) {
        int serial_number = 0;
        if(deviceType == 5322)
        {
            serial_number = 0;
        }
        else if(deviceType == 5323)
        {
            serial_number = 0;
        }
        else if(deviceType == 8902)
        {
            serial_number = 0;
        }

        displayWaveform(waveformId, tableName, deviceType);
    } else {
        undisplayWaveform(waveformId, tableName, deviceType);
    }
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
}

void ManageTask::undisplayWaveform(const QString& waveformid, const QString& tableName, const int deviceType)
{
    if(path_graph_map.find(tableName.split("/")[2] + "_" + waveformid) == path_graph_map.end()) return;
    if(deviceType == 5322)
    {
        std::vector<QCPGraph*> graphs = {path_graph_map[tableName.split("/")[2] + "_" + waveformid]};
        customPlot5322->removeLine(graphs);
        graph5322.erase(std::remove(graph5322.begin(), graph5322.end(), path_graph_map[tableName.split("/")[2] + "_" + waveformid]), graph5322.end());
    }
    else if(deviceType == 5323)
    {
        std::vector<QCPGraph*> graphs = {path_graph_map[tableName.split("/")[2] + "_" + waveformid]};
        customPlot5323->removeLine(graphs);
        graph5323.erase(std::remove(graph5323.begin(), graph5323.end(), path_graph_map[tableName.split("/")[2] + "_" + waveformid]), graph5323.end());
    }else if(deviceType == 8902)
    {
        std::vector<QCPGraph*> graphs = {path_graph_map[tableName.split("/")[2] + "_" + waveformid]};
        customPlot8902->removeLine(graphs);
        graph8902.erase(std::remove(graph8902.begin(), graph8902.end(), path_graph_map[tableName.split("/")[2] + "_" + waveformid]), graph8902.end());
    }
    path_graph_map.erase(tableName.split("/")[2] + "_" + waveformid);
}

void ManageTask::displayWaveform(const QString& waveformid, const QString& path, const int deviceType)
{
    if(path_graph_map.find(path.split("/")[2] + "_" + waveformid) != path_graph_map.end()) return;
    QString filepath = "./CollectData/" + path + "/" + QString::number(deviceType) + "/" + waveformid + ".mmap";

    if(!QFile::exists(filepath))
    {
        QMessageBox::warning(this, "文件不存在", "文件不存在");
        return;
    }

    if(deviceType == 5322)
    {
        auto* newGraph = customPlot5322->addLine(filepath);
        newGraph->setName(display_time_id_map[path.split("/")[2]] + "  步骤" + waveformid.split("_").at(waveformid.split("_").size() - 3) + ":" + "端口" + waveformid.split("_").last());
        path_graph_map[path.split("/")[2] + "_" + waveformid] = newGraph;
        graph5322.push_back(newGraph);
    }
    else if(deviceType == 5323)
    {
        auto* newGraph = customPlot5323->addLine(filepath);
        newGraph->setName(display_time_id_map[path.split("/")[2]] + "  步骤" + waveformid.split("_").at(waveformid.split("_").size() - 3) + ":" + "端口" + waveformid.split("_").last());
        path_graph_map[path.split("/")[2] + "_" + waveformid] = newGraph;
        graph5323.push_back(newGraph);
    }else if(deviceType == 8902)
    {
        auto* newGraph = customPlot8902->addLine(filepath);
        newGraph->setName(display_time_id_map[path.split("/")[2]] + "  步骤" + waveformid.split("_").at(waveformid.split("_").size() - 3) + ":" + "类型" + waveformid.split("_").last());
        path_graph_map[path.split("/")[2] + "_" + waveformid] = newGraph;
        graph8902.push_back(newGraph);
    }
}

void ManageTask::StateChanged(const QString& state, int step)
{
    if (state == tr("State Init"))
    {
        leds.clear();
        while (QLayoutItem* item = ui->horizontalLayout_TaskProgress->takeAt(0)) {
            delete item->widget(); // Delete widget
            delete item; // Delete layout item
        }
        for(int i = 0; i < step; i++)
        {
            auto *led = new LED(this, tr("步骤%1").arg(i + 1));
            led->set_led_status("ready");
            ui->horizontalLayout_TaskProgress->addWidget(led);
            leds.push_back(led);
        }
    }
    else if (state == "Step Begin")
    {
        leds[step - 1]->set_led_status("running");
    }
    else if (state == "Step Finished")
    {
        if(step > 0)
        {
            leds[step - 1]->set_led_status("finished");
        }
    }
    else if (state == "Step Interrupted")
    {
        QMessageBox::warning(this, "提示", "任务" + currentTaskId + "被中断", QMessageBox::Ok);
    }

    else if(state.startsWith("Connect Wire:"))
    {
        QString step_id = state.split(":").at(1);
        taskConnectWire->setStep(step_id);
        stackedWidget_connectwire->setCurrentIndex(0);
        parent->statusBar()->showMessage(state);
    }else if(state == "getinfraredimage")
    {
        infraredcamera.on_pbsave_clicked();
    }
    else if(step == -1)
    {
        QMessageBox::warning(this, "错误", state, QMessageBox::Ok);
    }else
    {
        parent->statusBar()->showMessage(state.toStdString().c_str());
    }
}

void ManageTask::on_pbViewTask_clicked()
{
    if(!graph5322.empty()) 
    {
        customPlot5322->removeLine(graph5322);
        graph5322.clear();
    }
    if(!graph5323.empty()) 
    {
        customPlot5323->removeLine(graph5323);
        graph5323.clear();
    }
    if(!graph8902.empty()) 
    {
        customPlot8902->removeLine(graph8902);
        graph8902.clear();
    }
    path_graph_map.clear();
    irimagedisplay_temp->clear();
    irimagedisplay->clear();

    if(currentTaskId.isEmpty() || device_id.isEmpty()) return;
    std::vector<Step> steps;
    QString table_name_step = device_id + "$$Step";
    database.get_step(table_name_step, "test_task_id = '" + currentTaskId + "'", steps);
    if(steps.empty()) return;
    setupTreeWidget(steps);
}


void ManageTask::on_pdRuntask_clicked()
{
    if(isTaskRunning)
    {
        QMessageBox::warning(this, "提示", "任务" + current_run_task_id + "正在运行，请先结束任务", QMessageBox::Ok);
        return;
    }
    ui->pdRuntask->setEnabled(false);
    current_run_task_id = currentTaskId;

    std::vector<Step> steps;
    QString table_name_step = device_id + "$$Step";
    database.get_step(table_name_step, QString("test_task_id = '%1'").arg(currentTaskId), steps);
    if(steps.empty())
    {
        ui->pdRuntask->setEnabled(true);
        return;
    }
    if(taskisexecuting)
    {
        ui->pdRuntask->setEnabled(true);
        QMessageBox::warning(this, "提示", "设备正在运行中,请先停止当前运行任务");
        return;
    }
    // if(!g_ch340->writeString("FF01FF"))
    // {
    //     ui->pdRuntask->setEnabled(true);
    //     QMessageBox::warning(this, "错误", "操作面板开关打开失败！");
    // }
    infraredcamera.setonlyemitdata(true);
    infraredcamera.showWindow();
    taskisexecuting = true;
    taskConnectWire->setDevice(device_id);
    QString TableName = device_id + "$$" + currentTaskId + "$$" + QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss");
    current_run_task_table = TableName;

    isTaskRunning = true;
    taskThread = std::make_shared<QThread>();
    localExecuteTask = std::make_shared<ExecuteTask>(currentTaskId, TableName, false);
    localExecuteTask->moveToThread(taskThread.get());

    connect(localExecuteTask.get(), &ExecuteTask::StateChanged, this, &ManageTask::StateChanged);
    connect(localExecuteTask.get(), &ExecuteTask::GetInfraredImage, this, &ManageTask::GetInfraredImage);
    connect(localExecuteTask.get(), &ExecuteTask::TaskFinished, this, [this]() {
        taskThread->quit();
        taskThread->wait();
        isTaskRunning = false;
    });
    connect(taskThread.get(), &QThread::finished, this, [this]() {
        auto localTask = std::move(localExecuteTask);
        if(localTask) localTask.reset();
        is_infraredcamera_save = false;
        infraredcamera.closeWindow();
        current_run_task_table.clear();
        taskisexecuting = false;
        // g_ch340->writeString("FF00FF");
        emit UpdateMysqlDataSignal();
    });
    taskThread->start();
    localExecuteTask->StartTask();
    ui->pdRuntask->setEnabled(true);
}

void ManageTask::deleteImage(const QString &imageId)
{
    QString table_name_image = device_id + "$$image";
    database.delete_image(table_name_image, QString("id = '%1'").arg(imageId));
}


void ManageTask::on_pbInterrupt_clicked()
{
    if(!isTaskRunning)
    {
        return;
    }
    isTaskRunning = false;
    taskisexecuting = false;
    localExecuteTask->InterruptTask();
}


void ManageTask::on_pbnextstep_clicked()
{
    if(!isTaskRunning)
    {
        return;
    }
    localExecuteTask->ContinueTask();
}

void ManageTask::on_showOrHideBtn_clicked()
{
    if (unfold) {
        ui->ptzWidget->hide();
        ui->showOrHideBtn->setText(">>");
        ui->containerWdget->setFixedWidth(ui->btnWidget->width() + 20);
        unfold = false;
    } else {
        ui->ptzWidget->show();
        ui->showOrHideBtn->setText("<<");
        ui->containerWdget->setFixedWidth(ui->btnWidget->width() + 750);
        unfold = true;
    }
}

void ManageTask::on_showOrHideBtnimage_clicked()
{
    if (unfold_image) {
        ui->groupBox_imageshow->hide();
        ui->showOrHideBtnimage->setText("<<");
        ui->widget_imageshow->setFixedWidth(ui->widget_btnimage->width() + 20);
        if(unfold) ui->containerWdget->setFixedWidth(ui->btnWidget->width() + 750);
        unfold_image = false;
    } else {
        ui->groupBox_imageshow->show();
        ui->showOrHideBtnimage->setText(">>");
        ui->widget_imageshow->setFixedWidth(ui->widget_btnimage->width() + 800);
        unfold_image = true;
    }
}


void ManageTask::on_cbdeviceid_activated(int index)
{
    element_id = ui->cbdeviceid->itemText(index).toInt();
    updateTaskComboBox();
}


void ManageTask::on_pbrefresh_clicked()
{
    ui->cbdeviceid->clear();
    if(device_id.isEmpty())
    {
        return;
    }
    std::vector<Label> label_info;
    QString ErrorMessage;
    if(!database.get_deviceelement(device_id, "", label_info, ErrorMessage))
    {
        QMessageBox::warning(this, "刷新失败", ErrorMessage);
        return;
    }
    if(label_info.empty())
    {
        return;
    }
    for(auto &item : label_info)
    {
        ui->cbdeviceid->addItem(QString::number(item.id));
    }
    element_id = label_info[0].id;
    updateTaskComboBox();
}

void ManageTask::GetInfraredImage(Step step)
{
    if(is_infraredcamera_save) return;
    is_infraredcamera_save = true;
    current_step = step;
    QThread *thread = QThread::create([this]() {
        if(current_step.isthermometry)
        {
            QThread::msleep(current_step.collecttime * (current_step.thermometry_pause_time / 100));
            infraredcamera.on_pbsave_clicked();
        }
    });
    thread->start();
    connect(thread, &QThread::finished, this, [this, thread]() {
        delete thread;
    });
}

void ManageTask::saveinfraredframe(const QImage& image, uint16_t *tempData, uint32_t tempWidth, uint32_t tempHeight)
{
    std::vector<uint16_t> tempDataVector;
    tempDataVector.assign(tempData, tempData + tempWidth * tempHeight);

    if(is_infraredcamera_save)
    {
        std::vector<uint16_t> tempDataVector;
        tempDataVector.assign(tempData, tempData + tempWidth * tempHeight);

        if(current_run_task_table.isEmpty())
        {
            is_infraredcamera_save = false;
            return;
        }
        IRimage.id = current_run_task_table.split("$$")[1] + "_" + current_run_task_table.split("$$")[2] + "_" + QString::number(current_step.step_number);
        IRimage.step_id = current_step.id;
        IRimage.task_table_name = current_run_task_table.split("$$")[1] + "_" + current_run_task_table.split("$$")[2];
        IRimage.device = "infraredcamera";
        IRimage.temp_width = tempWidth;
        IRimage.temp_height = tempHeight;
        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        buffer.open(QIODevice::WriteOnly);
        image.save(&buffer, "JPG");
        buffer.close();
        IRimage.image_data = byteArray;

        QByteArray tempByteArray;
        QBuffer tempBuffer(&tempByteArray);
        tempBuffer.open(QIODevice::WriteOnly);
        tempBuffer.write(reinterpret_cast<const char*>(tempDataVector.data()), tempWidth * tempHeight * sizeof(uint16_t));
        tempBuffer.close();
        IRimage.temp_data = tempByteArray;  
        QString device = current_run_task_table.split("$$")[0];
        QString table_name_image = device + "$$image";
        QString ErrorMessage;
        if(!database.insert_image(table_name_image, IRimage, ErrorMessage))
        {
            is_infraredcamera_save = false;
            QMessageBox::warning(this, "图像保存失败", ErrorMessage);
            return;
        }
        is_infraredcamera_save = false;
    }
}

void ManageTask::on_pblabelinfoshow_clicked()
{
    QString ErrorMessage;
    std::vector<Device> devices;
    label_info.clear();
    if(!database.get_device("id = '" + device_id + "'", devices, true))
    {
        return;
    }
    if(!database.get_deviceelement(device_id, "", label_info, ErrorMessage))
    {
        QMessageBox::warning(this, "设备元素查询失败", ErrorMessage);
        return;
    }
    LabeledingImage = QImage::fromData(devices[0].image);
    labelediting = std::make_shared<LabelEditingWindow>(nullptr, LabeledingImage, label_info, label_info_add, delete_id);
    labelediting->setSelectModel();
    connect(labelediting.get(), &LabelEditingWindow::window_close, this, [this]() {
        int select_id = labelediting->getSelectedLabelId();
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
    labelediting->show();
}

void ManageTask::ConnectWire(QString step_id)
{
    stackedWidget_connectwire->setCurrentIndex(0);
    taskConnectWire->setDevice(device_id);
}

void ManageTask::on_pb8902expand_clicked()
{
    handlePlotExpand(plot8902Window, customPlot8902, placeholderLabel8902, ui->horizontalScrollBar_8902, ui->gridLayout_8902, "万用表");
}

void ManageTask::on_pb5322expand_clicked()
{
    handlePlotExpand(plot5322Window, customPlot5322, placeholderLabel5322, ui->horizontalScrollBar_5322, ui->gridLayout_5322, "数字量输入");
}

void ManageTask::on_pb5323expand_clicked()
{
    handlePlotExpand(plot5323Window, customPlot5323, placeholderLabel5323, ui->horizontalScrollBar_5323, ui->gridLayout_5323, "数字量输入");
}

void ManageTask::handlePlotExpand(QWidget*& plotWindow, UESTCQCustomPlot* customPlot, QLabel*& placeholderLabel, 
                                 QScrollBar* scrollBar, QGridLayout* gridLayout, const QString& title)
{
    if (!plotWindow) {
        // 创建占位Label
        placeholderLabel = new QLabel(this);
        placeholderLabel->setSizePolicy(customPlot->sizePolicy());
        placeholderLabel->setMinimumSize(customPlot->minimumSize());
        placeholderLabel->setText("窗口已弹出");
        placeholderLabel->setAlignment(Qt::AlignCenter);
        QFont font = placeholderLabel->font();
        font.setPointSize(placeholderLabel->height() / 10); // 根据label高度调整字体大小
        placeholderLabel->setFont(font);
        

        // 创建新窗口
        plotWindow = new QWidget(nullptr);
        plotWindow->setAttribute(Qt::WA_DeleteOnClose, false);
        plotWindow->setWindowTitle(title);
        plotWindow->resize(800, 600);
        
        // 创建布局
        auto* mainLayout = new QVBoxLayout(plotWindow);
        auto* plotWidget = new QWidget;
        auto* plotLayout = new QVBoxLayout(plotWidget);
        plotLayout->setContentsMargins(0, 0, 0, 0);
        
        // 移动图表和滚动条到新窗口
        customPlot->setParent(plotWidget);
        plotLayout->addWidget(customPlot);
        scrollBar->setParent(plotWidget);
        plotLayout->addWidget(scrollBar);
        
        mainLayout->addWidget(plotWidget);
        gridLayout->addWidget(placeholderLabel, 0, 0);
        
        plotWindow->installEventFilter(this);
        plotWindow->show();
    } else {
        plotWindow->raise();
        plotWindow->activateWindow();
    }
}

bool ManageTask::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::Close) {
        if (watched == plot8902Window) {
            reparentPlot8902();
            handlePlotClose(plot8902Window);
            return true;
        } else if (watched == plot5322Window) {
            reparentPlot5322();
            handlePlotClose(plot5322Window);
            return true;
        } else if (watched == plot5323Window) {
            reparentPlot5323();
            handlePlotClose(plot5323Window);
            return true;
        }
    }
    return QWidget::eventFilter(watched, event);
}

void ManageTask::handlePlotClose(QWidget*& plotWindow)
{
    plotWindow->hide();
    delete plotWindow;
    plotWindow = nullptr;
}

void ManageTask::reparentPlot8902()
{
    handlePlotReparent(customPlot8902, placeholderLabel8902, ui->horizontalScrollBar_8902,
                      ui->gridLayout_8902, ui->verticalLayout_8902scroll);
}

void ManageTask::reparentPlot5322()
{
    handlePlotReparent(customPlot5322, placeholderLabel5322, ui->horizontalScrollBar_5322,
                      ui->gridLayout_5322, ui->verticalLayout_5322scroll);
}

void ManageTask::reparentPlot5323()
{
    handlePlotReparent(customPlot5323, placeholderLabel5323, ui->horizontalScrollBar_5323,
                      ui->gridLayout_5323, ui->verticalLayout_5323scroll);
}

void ManageTask::handlePlotReparent(UESTCQCustomPlot* customPlot, QLabel*& placeholderLabel,
                                   QScrollBar* scrollBar, QGridLayout* gridLayout,
                                   QVBoxLayout* verticalLayout)
{
    if (customPlot) {
        // 从当前父窗口的布局中移除
        if (QLayout* oldLayout = customPlot->parentWidget()->layout()) {
            oldLayout->removeWidget(customPlot);
            oldLayout->removeWidget(scrollBar);
        }
        
        // 删除占位Label
        if (placeholderLabel) {
            gridLayout->removeWidget(placeholderLabel);
            delete placeholderLabel;
            placeholderLabel = nullptr;
        }
        
        // 重新添加到主窗口的布局中
        customPlot->setParent(this);
        scrollBar->setParent(this);
        gridLayout->addWidget(customPlot);
        verticalLayout->addWidget(scrollBar);
        
        // 更新布局
        customPlot->show();
        scrollBar->show();
    }
}


void ManageTask::saveinfraredframe_hypertherm(QImage image, std::vector<uint16_t> tempData, uint32_t tempWidth, uint32_t tempHeight)
{
    if(irimagedisplay_hypertherm)
    {
        irimagedisplay_hypertherm->setImage(image, tempData, tempWidth, tempHeight);
        irimagedisplay_hypertherm->show();
    }else
    {
        irimagedisplay_hypertherm = new IRImageDisplay(nullptr);
        irimagedisplay_hypertherm->setImage(image, tempData, tempWidth, tempHeight);
        irimagedisplay_hypertherm->MarkMaxTemp(true);
        irimagedisplay_hypertherm->show();
    }
    on_pbInterrupt_clicked();
}


