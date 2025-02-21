#include "FlowTaskManager.h"
#include <QDateTime>
#include <QRandomGenerator>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QGraphicsView>
#include "VisioView.h"
#include <QHBoxLayout>
#include "StepEditDialog.h"
#include "deviceid.h"
#include <QRegExp>

FlowTaskManager::FlowTaskManager(QWidget *parent)
    : QWidget(parent)
    , stepCount(0)
    , deviceId(gDeviceId.getDeviceId())
    , rootTaskId("")
    , currentElementId(-1)
    , currentElementLabel(nullptr)
{
    setupUI();
    connectDatabase();
    
    // 连接设备 ID 变更信号
    connect(&gDeviceId, &DeviceId::deviceChanged,
            this, &FlowTaskManager::onDeviceIdChanged);
            
    updateTaskList();
    loadTaskFromDatabase();
    this->close();
}

void FlowTaskManager::setupUI()
{
    // 创建布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // 创建顶部工具栏
    QHBoxLayout *toolbarLayout = new QHBoxLayout;
    toolbarLayout->setSpacing(15);  // 设置布局中控件之间的间距
    
    // 创建一个水平分组用于被测元件相关控件
    QHBoxLayout *elementGroup = new QHBoxLayout;
    elementGroup->setSpacing(8);  // 组内控件间距
    
    // 添加当前被测元件显示标签
    currentElementLabel = new QLabel("当前被测元件: 未选择", this);
    currentElementLabel->setStyleSheet("QLabel { color: #2c3e50; font-weight: bold; }");
    elementGroup->addWidget(currentElementLabel);
    
    // 添加选择被测元件按钮
    QPushButton *selectComponentBtn = new QPushButton("选择被测元件", this);
    selectComponentBtn->setStyleSheet(
        "QPushButton {"
        "    background-color: #3498db;"
        "    color: white;"
        "    border: none;"
        "    padding: 5px 15px;"
        "    border-radius: 4px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #2980b9;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #2472a4;"
        "}"
    );
    elementGroup->addWidget(selectComponentBtn);
    
    // 添加元件组到工具栏
    toolbarLayout->addLayout(elementGroup);
    
    // 添加垂直分隔线
    QFrame* vLine1 = new QFrame(this);
    vLine1->setFrameShape(QFrame::VLine);
    vLine1->setFrameShadow(QFrame::Sunken);
    vLine1->setStyleSheet("QFrame { color: #bdc3c7; }");
    toolbarLayout->addWidget(vLine1);
    
    // 创建一个水平分组用于任务相关控件
    QHBoxLayout *taskGroup = new QHBoxLayout;
    taskGroup->setSpacing(8);  // 组内控件间距
    
    // 任务列表下拉框
    taskListCombo = new QComboBox(this);
    QLabel* taskListLabel = new QLabel("任务列表:", this);
    taskListLabel->setStyleSheet("QLabel { color: #2c3e50; }");
    taskListCombo->setStyleSheet(
        "QComboBox {"
        "    border: 1px solid #bdc3c7;"
        "    border-radius: 4px;"
        "    padding: 4px;"
        "    min-width: 150px;"
        "}"
    );
    taskGroup->addWidget(taskListLabel);
    taskGroup->addWidget(taskListCombo);
    
    // 任务ID输入框
    taskIdEdit = new QLineEdit(this);
    taskIdEdit->setPlaceholderText("输入任务ID");
    QLabel* taskIdLabel = new QLabel("任务ID:", this);
    taskIdLabel->setStyleSheet("QLabel { color: #2c3e50; }");
    taskIdEdit->setStyleSheet(
        "QLineEdit {"
        "    border: 1px solid #bdc3c7;"
        "    border-radius: 4px;"
        "    padding: 4px;"
        "    min-width: 150px;"
        "}"
    );
    taskGroup->addWidget(taskIdLabel);
    taskGroup->addWidget(taskIdEdit);
    
    // 创建按钮
    createTaskBtn = new QPushButton("创建任务", this);
    createTaskBtn->setStyleSheet(
        "QPushButton {"
        "    background-color: #2ecc71;"
        "    color: white;"
        "    border: none;"
        "    padding: 5px 15px;"
        "    border-radius: 4px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #27ae60;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #219a52;"
        "}"
    );
    taskGroup->addWidget(createTaskBtn);
    
    // 添加任务组到工具栏
    toolbarLayout->addLayout(taskGroup);
    
    // 添加垂直分隔线
    QFrame* vLine2 = new QFrame(this);
    vLine2->setFrameShape(QFrame::VLine);
    vLine2->setFrameShadow(QFrame::Sunken);
    vLine2->setStyleSheet("QFrame { color: #bdc3c7; }");
    toolbarLayout->addWidget(vLine2);
    
    addStepBtn = new QPushButton("添加步骤", this);
    deleteTaskBtn = new QPushButton("删除任务", this);
    
    // 设置其他按钮的样式
    QString otherBtnStyle =
        "QPushButton {"
        "    background-color: #95a5a6;"
        "    color: white;"
        "    border: none;"
        "    padding: 5px 15px;"
        "    border-radius: 4px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #7f8c8d;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #6c7778;"
        "}";
    addStepBtn->setStyleSheet(otherBtnStyle);
    deleteTaskBtn->setStyleSheet(otherBtnStyle);
    
    toolbarLayout->addWidget(addStepBtn);
    toolbarLayout->addWidget(deleteTaskBtn);
    toolbarLayout->addStretch();
    
    // 创建图形场景和自定义视图（使用 VisioView 实现平移缩放）
    scene = new QGraphicsScene(this);
    view = new VisioView(this);
    view->setScene(scene);
    
    // 添加部件到布局
    mainLayout->addLayout(toolbarLayout);
    mainLayout->addWidget(view);
    
    // 连接信号和槽
    connect(selectComponentBtn, &QPushButton::clicked, this, &FlowTaskManager::showSelectComponentDialog);
    connect(createTaskBtn, SIGNAL(clicked()), this, SLOT(createNewTask()));
    connect(addStepBtn, SIGNAL(clicked()), this, SLOT(addNewStep()));
    connect(deleteTaskBtn, &QPushButton::clicked, this, &FlowTaskManager::deleteCurrentTask);
    connect(taskListCombo, &QComboBox::currentTextChanged, this, &FlowTaskManager::loadTaskFromDatabase);
    
    addStepBtn->setEnabled(false);  // 初始禁用添加步骤按钮
    deleteTaskBtn->setEnabled(false);
}

FlowTaskManager::~FlowTaskManager()
{
    clearFlowChart();
}

QString FlowTaskManager::generateTaskId()
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    int random = QRandomGenerator::global()->bounded(1000, 9999);
    return QString("%1-%2").arg(timestamp).arg(random);
}

void FlowTaskManager::createNewTask()
{
    // 检查是否已选择被测元件
    if (currentElementId <= 0) {
        QMessageBox::warning(this, "警告", "请先选择被测元件");
        return;
    }

    QString taskId = taskIdEdit->text();

    if (taskId.isEmpty()) {
        taskId = generateTaskId();
        taskIdEdit->setText(taskId);
    }
    
    if(taskId.length() > 20)
    {
        QMessageBox::warning(this, "错误", "任务ID长度不能超过20个字符");
        return;
    }

    QRegExp rx("[a-zA-Z0-9_-]+");
    if(!rx.exactMatch(taskId) || taskId.startsWith("test"))
    {
        QMessageBox::warning(this, "错误", "任务ID只能由数字、字母和下划线组成,且不能以test开头");
        return;
    }

    // 检查任务ID是否已存在
    std::vector<TestTask> existingTasks;
    QString errorMsg;
    QString tableName = deviceId + "$$TestTask";
    if (db->get_testtask(tableName, "id = '" + taskId + "'", existingTasks, errorMsg) && 
        !existingTasks.empty()) {
        QMessageBox::warning(this, "错误", "任务ID已存在，请使用其他ID");
        return;
    }
    
    // 设置任务ID
    currentTaskId = taskId;
    rootTaskId = taskId;
    
    // 清除旧的步骤节点和场景
    clearFlowChart();
    stepCount = 0;  // 重置步骤计数
    
    drawFlowChart();
    addStepBtn->setEnabled(true);
    deleteTaskBtn->setEnabled(true);
    
    // 保存新任务到数据库
    saveCurrentTask();
    
    // 更新任务列表并选中新建的任务
    int newIndex = taskListCombo->findText(taskId);
    if (newIndex == -1) {  // 如果找不到（不应该发生），就添加它
        QMessageBox::warning(this, "错误", "任务列表中找不到新建的任务");
        return;
    }
    taskListCombo->setCurrentIndex(newIndex);  // 选中新建的任务
}

void FlowTaskManager::addNewStep()
{
    if (currentTaskId.isEmpty()) {
        return;
    }
    
    // 增加步骤计数
    stepCount++;
    
    QString stepText = QString("步骤 %1").arg(stepCount);
    FlowNode *newStep = new FlowNode(stepText, stepNodes.isEmpty(), currentTaskId);
    
    // 设置步骤节点的 Step 数据
    newStep->stepData.test_task_id = currentTaskId;
    newStep->stepData.id = QString("%1_%2").arg(currentTaskId).arg(stepCount);
    newStep->stepData.step_number = stepCount;
    newStep->stepData.collecttime = 10;
    newStep->stepData.continue_step = false;
    newStep->stepData.isthermometry = true;
    newStep->stepData.thermometry_pause_time = 50;
    
    connect(newStep, &FlowNode::nodeDeleted, this, &FlowTaskManager::handleNodeDeleted);
    connect(newStep, &FlowNode::nodeRunRequested, this, &FlowTaskManager::handleNodeRun);
    
    // 将新节点直接追加到容器中（保持顺序为先创建的在前）
    stepNodes.append(newStep);
    
    // 保存新步骤到数据库
    QString stepTableName = deviceId + "$$Step";
    if (!db->insert_step(stepTableName, newStep->stepData)) {
        QMessageBox::warning(this, "错误", "保存步骤失败");
    }
    
    drawFlowChart();
}

void FlowTaskManager::drawFlowChart()
{
    if (stepNodes.isEmpty()) return;

    // 设置初始位置
    qreal startX = 100;  // 左侧留出空间
    qreal startY = scene->height() / 2 - FlowNode::NODE_HEIGHT / 2;  // 垂直居中
    qreal horizontalSpacing = FlowNode::NODE_WIDTH + 100;  // 节点之间的水平间距

    FlowNode* lastNode = nullptr;

    // 添加根节点
    if (!stepNodes.isEmpty()) {
        FlowNode* rootNode = stepNodes.first();
        scene->addItem(rootNode);
        rootNode->setPos(startX, startY);
        lastNode = rootNode;
        startX += horizontalSpacing;
    }

    // 布局其他节点
    for (int i = 1; i < stepNodes.size(); ++i) {
        FlowNode* node = stepNodes[i];
        scene->addItem(node);
        node->setPos(startX, startY);
        node->setPrevNode(lastNode);  // 设置前一个节点

        lastNode = node;
        startX += horizontalSpacing;
    }

    // 调整场景大小以适应所有节点
    scene->setSceneRect(scene->itemsBoundingRect());
}

void FlowTaskManager::clearFlowChart()
{
    for (FlowNode* node : stepNodes) {
        delete node;
    }
    stepNodes.clear();
    stepCount = 0;
    // 清除场景中的所有项目
    scene->clear();
    // 重置场景大小
    scene->setSceneRect(0, 0, view->width(), view->height());
}

void FlowTaskManager::handleNodeDeleted(FlowNode* node)
{
    if (!node || !stepNodes.contains(node)) return;
    
    // 从场景中移除节点
    if (node->scene()) {
        scene->removeItem(node);
    }
    // 删除数据库中对应的步骤记录
    QString stepTableName = deviceId + "$$Step";
    QString errorMsg;
    if (!db->delete_step(stepTableName, node->stepData.id, errorMsg)) {
        QMessageBox::warning(this, "错误", "删除步骤失败: " + errorMsg);
    }
    
    // 从容器中移除该节点
    stepNodes.removeAll(node);
    
    // 更新步骤计数
    if (stepNodes.isEmpty()) {
        // 如果删除后没有节点了，重置步骤计数
        stepCount = 0;
    } else {
        // 更新步骤计数为当前最大步骤号
        stepCount = 0;
        for (const FlowNode* remainingNode : stepNodes) {
            stepCount = std::max(stepCount, remainingNode->stepData.step_number);
        }
    }
    
    drawFlowChart();
}

void FlowTaskManager::handleNodeRun(FlowNode* node)
{
    if (!node) return;
    
    // 打开步骤编辑对话框
    StepEditDialog dialog(node->stepData, deviceId, this);
    if (dialog.exec() == QDialog::Accepted) {
        // 直接更新节点数据
        Step newStep = dialog.step();
        node->stepData = newStep; // 直接用新步骤数据更新 FlowNode
        node->setContinueStep(newStep.continue_step);  // 更新 continue_step，内部会调用 updateConnectorStyle()
        node->updateConnectorStyle(); // 直接调用更新函数，立即刷新连接线样式

        // 保存到数据库
        QString stepTableName = deviceId + "$$Step";
        if (!db->update_step(stepTableName, node->stepData)) {
            QMessageBox::warning(this, "错误", "保存步骤失败");
        }
    }
}

void FlowTaskManager::connectDatabase()
{
    db = std::make_shared<Database>("TaskManager");
}

void FlowTaskManager::updateTaskList()
{
    taskListCombo->clear();
    std::vector<TestTask> tasks;
    QString errorMsg;
    QString tableName = deviceId + "$$TestTask";
    
    QString condition = currentElementId > 0 ? 
        QString("element_id = %1").arg(currentElementId) : "";
    
    if (db->get_testtask(tableName, condition, tasks, errorMsg)) {
        for (const auto& task : tasks) {
            taskListCombo->addItem(task.id);
        }
    }
}

void FlowTaskManager::loadTaskFromDatabase()
{
    if (taskListCombo->currentText().isEmpty()) return;
    
    clearFlowChart();
    currentTaskId = taskListCombo->currentText();
    taskIdEdit->setText(currentTaskId);
    
    // 加载步骤
    std::vector<Step> steps;
    QString stepTableName = deviceId + "$$Step";
    if (db->get_step(stepTableName, "test_task_id = '" + currentTaskId + "'", steps)) {
        // 按步骤号排序
        std::sort(steps.begin(), steps.end(), 
            [](const Step& a, const Step& b) { return a.step_number < b.step_number; });
        
        // 对于每一步，创建节点后追加到 stepNodes 列表中
        for (const auto& step : steps) {
            QString nodeText = QString("步骤 %1").arg(step.step_number);
            FlowNode* node = new FlowNode(nodeText, stepNodes.isEmpty(), currentTaskId);
            node->stepData = step;
            node->updateConnectorStyle();  // 初始化连接线样式
            connect(node, &FlowNode::nodeDeleted, this, &FlowTaskManager::handleNodeDeleted);
            connect(node, &FlowNode::nodeRunRequested, this, &FlowTaskManager::handleNodeRun);
            stepNodes.append(node);
            stepCount = std::max(stepCount, step.step_number);
        }
    }
    
    drawFlowChart();
    addStepBtn->setEnabled(true);
    deleteTaskBtn->setEnabled(true);
}

void FlowTaskManager::saveCurrentTask()
{
    if (currentTaskId.isEmpty()) return;
    
    QString errorMsg;
    QString testTaskTableName = deviceId + "$$TestTask";
    
    // 保存任务
    TestTask task;
    task.id = currentTaskId;
    task.element_id = currentElementId;  // 使用当前选中的元件ID
    
    if (!db->insert_testtask(testTaskTableName, task, errorMsg)) {
        QMessageBox::warning(this, "错误", "保存任务失败: " + errorMsg);
        return;
    }
    
    QMessageBox::information(this, "成功", "任务保存成功");
    updateTaskList();
}

void FlowTaskManager::deleteCurrentTask()
{
    if (currentTaskId.isEmpty()) return;
    
    QString errorMsg;
    QString testTaskTableName = deviceId + "$$TestTask";
    
    if (db->delete_testtask(testTaskTableName, currentTaskId, errorMsg)) {
        clearFlowChart();
        updateTaskList();
        QMessageBox::information(this, "成功", "任务删除成功");
    } else {
        QMessageBox::warning(this, "错误", "删除任务失败: " + errorMsg);
    }
}

void FlowTaskManager::onDeviceIdChanged(const QString& newDeviceId)
{
    deviceId = newDeviceId;
    updateTaskList();
    loadTaskFromDatabase();
}

void FlowTaskManager::showSelectComponentDialog()
{
    // 从数据库加载设备图像和标签
    std::vector<Device> devices;
    if (db->get_device("id = '" + deviceId + "'", devices, true) && !devices.empty()) {
        QImage deviceImage = QImage::fromData(devices[0].image);
        
        QString errorMsg;
        std::vector<Label> deviceLabels;
        if (db->get_deviceelement(deviceId, "", deviceLabels, errorMsg)) {
            // 创建并显示选择对话框
            ConnectionLocationDialog dialog(deviceImage, deviceLabels, this);
            dialog.setWindowTitle("选择被测元件");
            if (dialog.exec() == QDialog::Accepted) {
                int selectedId = dialog.getSelectedLabelId();
                if (selectedId != -1) {
                    // 找到对应的标签信息
                    auto it = std::find_if(deviceLabels.begin(), deviceLabels.end(),
                        [selectedId](const Label& label) { return label.id == selectedId; });
                    if (it != deviceLabels.end()) {
                        // 更新当前选中的元件ID和显示
                        currentElementId = selectedId;
                        currentElementLabel->setText(QString("当前被测元件: %1 (ID: %2)")
                            .arg(it->label).arg(selectedId));
                        
                        // 清理画布
                        clearFlowChart();
                        currentTaskId.clear();
                        taskIdEdit->clear();
                        stepCount = 0;
                        
                        // 更新任务列表
                        updateTaskList();
                        
                        // 如果有任务，选中第一个
                        if (taskListCombo->count() > 0) {
                            taskListCombo->setCurrentIndex(0);
                            loadTaskFromDatabase();
                        }
                        else {
                            // 如果没有任务，禁用相关按钮
                            addStepBtn->setEnabled(false);
                            deleteTaskBtn->setEnabled(false);
                        }
                    }
                }
            }
        } else {
            QMessageBox::warning(this, "错误", "加载设备标签失败: " + errorMsg);
        }
    } else {
        QMessageBox::warning(this, "错误", "加载设备图像失败");
    }
} 
