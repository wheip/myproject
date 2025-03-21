#include "managetask.h"
#include "ui_managetask.h"
#include <QStandardItem>
#include <QMessageBox>
#include <QGraphicsLayout>
#include <QDebug>
#include <QStackedWidget>
#include <QtConcurrent>

extern bool taskisexecuting;
extern int current_device_id;

ManageTask::ManageTask(QMainWindow *parent)
    : parent(parent)
    , ui(new Ui::ManageTask)
    , database("managetask", this)
    , device_id(current_device_id)
{
    ui->setupUi(this);

    runTask = new RunTask();
    connect(runTask, &RunTask::StateChanged, this, &ManageTask::StateChanged);
    ComboBoxInit();

    irimagedisplay_temp = new IRImageDisplay(this);
    irimagedisplay = new IRImageDisplay(this);

    ui->gridLayout_image->addWidget(irimagedisplay_temp, 0, 0);
    ui->gridLayout_iconimage->addWidget(irimagedisplay, 0, 0);
    irimagedisplay_temp->show();
    irimagedisplay->show();
    QObject::connect(&gDeviceId, &DeviceId::deviceChanged, this, [this]() {
        device_id = gDeviceId.getDeviceId();
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
    QString table_name = QString::number(device_id) + "$$TestTask";
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

    std::vector<QString> TaskList = g_FolderCheck.Get_Folder_list("./CollectData/" + QString::number(device_id) + "/" + currentTaskId);

    QTreeWidgetItem *rootItem = new QTreeWidgetItem(ui->treeWidget);
    rootItem->setText(0, currentTaskId);
    ui->treeWidget->addTopLevelItem(rootItem);

    for (int i = 0; i < TaskList.size(); ++i)
    {
        QTreeWidgetItem *timeItem = new QTreeWidgetItem(rootItem);

        QString displayText;
        if(i == 0)
        {
            displayText = "参考数据";
            display_time_id_map[TaskList[i]] = displayText;
        }else{
            displayText = "测试数据" + QString::number(i);
            display_time_id_map[TaskList[i]] = displayText;
        }

        displayText = displayText + " " + "(" + TaskList[i] + ")";
        timeItem->setText(1, displayText);
        timeItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
        timeItem->setCheckState(1, Qt::Unchecked);
        setupStepContextMenu(timeItem, QString::number(device_id) + "/" + currentTaskId + "/" + TaskList[i]);

        for (const auto& step : steps)
        {
            QTreeWidgetItem *stepItem = createStepItem(step);
            timeItem->addChild(stepItem);
            populateStepItem(stepItem, step, QString::number(device_id) + "/" + currentTaskId + "/" + TaskList[i]);
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

void ManageTask::setupStepContextMenu(QTreeWidgetItem *item, const QString &path)
{
    item->treeWidget()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(item->treeWidget(), &QTreeWidget::customContextMenuRequested, this, [this, item, path](const QPoint &pos) {
        QTreeWidgetItem* selectedItem = item->treeWidget()->itemAt(pos);
        if (selectedItem && selectedItem == item) {
            QMenu contextMenu(tr("上下文菜单"), this);
            QAction *actionDelete = new QAction(tr("删除"), this);
            contextMenu.addAction(actionDelete);
            connect(actionDelete, &QAction::triggered, this, [this, item, path]() {
                if (g_FolderCheck.Delete_Folder("./CollectData/" + path)) {
                    QString TableName = path.split("/")[0] + "$$image";
                    QString task_table_name = path.split("/").last();
                    database.delete_image(TableName, "task_table_name = '" + task_table_name + "'");
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
    stepItem->setData(0, Qt::UserRole, QVariant::fromValue(step.step_number));
    stepItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
    stepItem->setCheckState(2, Qt::Unchecked);
    return stepItem;
}

void ManageTask::populateStepItem(QTreeWidgetItem* stepItem, const Step& step, const QString& path) //path = CollectData/divice_id/task_id/test_time
{
    std::vector<PXIe5320Waveform> waveforms;
    std::vector<Data8902> waveforms8902;

    QString table_name_pxie5320 = QString::number(device_id) + "$$PXIe5320";
    database.get_pxie5320waveform(table_name_pxie5320, "step_id = '" + QString::number(step.id) + "'", waveforms);
    QString table_name_pxie8902 = QString::number(device_id) + "$$PXIe8902";
    database.get_8902data(table_name_pxie8902, "step_id = '" + QString::number(step.id) + "'", waveforms8902);

    QTreeWidgetItem *pxie8902Item = new QTreeWidgetItem(stepItem, QStringList() << "" << "" << "" << "万用表");
    QTreeWidgetItem *pxie5322Item = new QTreeWidgetItem(stepItem, QStringList() << "" << "" << "" << "数字输入");
    QTreeWidgetItem *pxie5323Item = new QTreeWidgetItem(stepItem, QStringList() << "" << "" << "" << "模拟输入");
    QTreeWidgetItem *irItem = new QTreeWidgetItem(stepItem, QStringList() << "" << "" << "" << "红外图像");

    pxie8902Item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
    pxie5322Item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
    pxie5323Item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
    irItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
    irItem->setData(0, Qt::UserRole, QVariant::fromValue(QPair<int, QString>(step.id, path)));


    pxie8902Item->setCheckState(3, Qt::Unchecked);
    pxie5322Item->setCheckState(3, Qt::Unchecked);
    pxie5323Item->setCheckState(3, Qt::Unchecked);
    irItem->setCheckState(3, Qt::Unchecked);


    addWaveformsToTree(pxie5322Item, waveforms, 5322, path + "/5322");
    addWaveformsToTree(pxie5323Item, waveforms, 5323, path + "/5323");
    addWaveforms8902ToTree(pxie8902Item, waveforms8902, path + "/8902");
    connectIRItem(irItem);
}

void ManageTask::connectIRItem(QTreeWidgetItem* item)
{
    connect(ui->treeWidget, &QTreeWidget::itemChanged, this, [this, item](QTreeWidgetItem* changedItem, int column) {
        if (changedItem == item && column == 3) {
            if(changedItem->checkState(column) == Qt::Checked)
            {
                QString path = item->data(0, Qt::UserRole).value<QPair<int, QString>>().second;
                int stepId = item->data(0, Qt::UserRole).value<QPair<int, QString>>().first;
                handleIRItemChange(path, stepId);
            }
        }
    });
}

void ManageTask::handleIRItemChange(const QString& path, const int& stepId)
{
    QString table_name_irimage = path.split("/")[0] + "$$image";
    QString ErrorMessage;
    std::vector<Image> irimage;
    if(display_time_id_map[path.split("/")[2]].contains("参考数据"))
        irimagedisplay_temp->clear();
    else
        irimagedisplay->clear();
    QString table_name = path.split("/").last();
    if(!database.get_image(table_name_irimage, "task_table_name = '" + table_name + "' AND step_id = '" + QString::number(stepId) + "'", irimage, ErrorMessage))
    {
        QMessageBox::warning(this, "红外图像查询失败", ErrorMessage);
        return;
    }
    if(irimage.empty()) return;
    QImage qimage = QImage::fromData(irimage[0].image_data);
    std::vector<uint16_t> tempVector(reinterpret_cast<uint16_t*>(irimage[0].temp_data.data()),
                                                 reinterpret_cast<uint16_t*>(irimage[0].temp_data.data() + irimage[0].temp_data.size()));
    if(display_time_id_map[path.split("/")[2]].contains("参考数据"))
    {
        irimagedisplay_temp->setImage(qimage, tempVector, irimage[0].temp_width, irimage[0].temp_height);
    }
    else
    {
        irimagedisplay->setImage(qimage, tempVector, irimage[0].temp_width, irimage[0].temp_height);
    }
}

void ManageTask::addWaveformsToTree(QTreeWidgetItem* parentItem, const std::vector<PXIe5320Waveform>& waveforms, int deviceType, const QString& path)
{
    for(const auto& waveform : waveforms)
    {
        if(waveform.device == deviceType)
        {
            QTreeWidgetItem *waveformItem = createWaveformItem(waveform, path + "/" + QString::number(waveform.id) + ".mmap"); //file path
            parentItem->addChild(waveformItem);
            connectWaveformItem(waveformItem);
        }
    }
}

void ManageTask::addWaveforms8902ToTree(QTreeWidgetItem* parentItem, const std::vector<Data8902>& waveforms8902, const QString& path)
{
    for(const auto& waveform : waveforms8902)
    {
        QTreeWidgetItem *waveformItem = createWaveform8902Item(waveform, path + "/" + QString::number(waveform.id) + ".mmap");
        parentItem->addChild(waveformItem);
        connectWaveformItem(waveformItem);
    }
}

QTreeWidgetItem* ManageTask::createWaveformItem(const PXIe5320Waveform& waveform, const QString& path)
{
    QTreeWidgetItem *waveformItem = new QTreeWidgetItem();
    waveformItem->setText(4, tr("通道%1").arg(waveform.port));
    waveformItem->setFlags(waveformItem->flags() | Qt::ItemIsUserCheckable);
    waveformItem->setCheckState(4, Qt::Unchecked);
    waveformItem->setData(0, Qt::UserRole, QVariant::fromValue(QPair<int, QString>(waveform.device, path)));
    return waveformItem;
}

QTreeWidgetItem* ManageTask::createWaveform8902Item(const Data8902& waveform, const QString& path)
{
    QTreeWidgetItem *waveformItem = new QTreeWidgetItem();
    waveformItem->setText(4, (PXIe8902_testtype_to_string(waveform.test_type)));
    waveformItem->setFlags(waveformItem->flags() | Qt::ItemIsUserCheckable);
    waveformItem->setCheckState(4, Qt::Unchecked);
    waveformItem->setData(0, Qt::UserRole, QVariant::fromValue(QPair<int, QString>(8902, path)));
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
    int deviceType = item->data(0, Qt::UserRole).value<QPair<int, QString>>().first;
    QString path = item->data(0, Qt::UserRole).value<QPair<int, QString>>().second;
    QString stepNumber = QString::number(item->parent()->parent()->data(0, Qt::UserRole).value<int>());
    QString port = item->text(4);
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

        displayWaveform(path, deviceType, stepNumber, port);
    } else {
        undisplayWaveform(path, deviceType);
    }
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
}

void ManageTask::displayWaveform(const QString& path, const int& deviceType, const QString& stepNumber, const QString& port)
{
    if(path_graph_map.find(path) != path_graph_map.end()) return;

    if(!QFile::exists("./CollectData/" + path))
    {
        QMessageBox::warning(this, "文件不存在", "文件不存在");
        return;
    }

    if(deviceType == 5322)
    {
        auto* newGraph = customPlot5322->addLine("./CollectData/" + path);
        newGraph->setName(display_time_id_map[path.split("/")[2]] + "  步骤" + stepNumber + ":" + port);
        path_graph_map[path] = newGraph;
        graph5322.push_back(newGraph);
    }
    else if(deviceType == 5323)
    {
        auto* newGraph = customPlot5323->addLine("./CollectData/" + path);
        newGraph->setName(display_time_id_map[path.split("/")[2]] + "  步骤" + stepNumber + ":" + port);
        path_graph_map[path] = newGraph;
        graph5323.push_back(newGraph);
    }else if(deviceType == 8902)
    {
        auto* newGraph = customPlot8902->addLine("./CollectData/" + path);
        newGraph->setName(display_time_id_map[path.split("/")[2]] + "  步骤" + stepNumber + ":" + port);
        path_graph_map[path] = newGraph;
        graph8902.push_back(newGraph);
    }
}

void ManageTask::undisplayWaveform(const QString& path, const int& deviceType)
{
    if(path_graph_map.find(path) == path_graph_map.end()) return;
    if(deviceType == 5322)
    {
        std::vector<QCPGraph*> graphs = {path_graph_map[path]};
        customPlot5322->removeLine(graphs);
        graph5322.erase(std::remove(graph5322.begin(), graph5322.end(), path_graph_map[path]), graph5322.end());
    }
    else if(deviceType == 5323)
    {
        std::vector<QCPGraph*> graphs = {path_graph_map[path]};
        customPlot5323->removeLine(graphs);
        graph5323.erase(std::remove(graph5323.begin(), graph5323.end(), path_graph_map[path]), graph5323.end());
    }else if(deviceType == 8902)
    {
        std::vector<QCPGraph*> graphs = {path_graph_map[path]};
        customPlot8902->removeLine(graphs);
        graph8902.erase(std::remove(graph8902.begin(), graph8902.end(), path_graph_map[path]), graph8902.end());
    }
    path_graph_map.erase(path);
}

void ManageTask::StateChanged(int state, const QString& message)
{
    switch(state)
    {
        case 10:
        {
            int leftBracket = message.indexOf("[");
            int rightBracket = message.indexOf("]");
            int step = message.mid(leftBracket + 1, rightBracket - leftBracket - 1).toInt();
            leds[step_led_map[step]]->set_led_status("running");
            break;
        }
        case 11:
        {
            int leftBracket = message.indexOf("[");
            int rightBracket = message.indexOf("]");
            int step = message.mid(leftBracket + 1, rightBracket - leftBracket - 1).toInt();
            leds[step_led_map[step]]->set_led_status("finished");
            break;
        }
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

    if(currentTaskId.isEmpty() || device_id < 0) return;
    std::vector<Step> steps;
    QString table_name_step = QString::number(device_id) + "$$Step";
    database.get_step(table_name_step, "test_task_id = '" + currentTaskId + "'", steps);
    if(steps.empty()) return;
    setupTreeWidget(steps);
}

void ManageTask::InitalizeLED()
{
    std::vector<Step> steps;
    QString table_name_step = QString::number(device_id) + "$$Step";
    database.get_step(table_name_step, "test_task_id = '" + currentTaskId + "'", steps);
    leds.clear();
    while (QLayoutItem* item = ui->horizontalLayout_TaskProgress->takeAt(0)) {
        delete item->widget(); // Delete widget
        delete item; // Delete layout item
    }
    for(auto &step : steps)
    {
        auto *led = new LED(this, tr("步骤%1").arg(step.step_number));
        led->set_led_status("ready");
        ui->horizontalLayout_TaskProgress->addWidget(led);
        leds.push_back(led);
        step_led_map[step.step_number] = leds.size() - 1;
    }
}
void ManageTask::on_pdRuntask_clicked()
{
    if(runTask->GetTaskStatus() == RunTaskStatus::Running)
    {
        QMessageBox::warning(this, "提示", "任务正在运行", QMessageBox::Ok);
        return;
    }
    else
    {
        InitalizeLED();
        runTask->RunTestTask(device_id, currentTaskId);
    }
}
void ManageTask::on_pbInterrupt_clicked()
{
    if(runTask->GetTaskStatus() == RunTaskStatus::Running)
    {
        runTask->InterruptTask();
    }
    else
    {
        QMessageBox::warning(this, "提示", "任务未运行", QMessageBox::Ok);
    }
}


void ManageTask::on_pbnextstep_clicked()
{
    if(!isTaskRunning)
    {
        return;
    }
    // localExecuteTask->ContinueTask();
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
    if(device_id < 0)
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

void ManageTask::on_pblabelinfoshow_clicked()
{
    QString ErrorMessage;
    std::vector<Device> devices;
    label_info.clear();
    if(!database.get_device("id = '" + QString::number(device_id) + "'", devices, true))
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


