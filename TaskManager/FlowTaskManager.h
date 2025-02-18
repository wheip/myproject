#ifndef FLOWTASKMANAGER_H
#define FLOWTASKMANAGER_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include "FlowNode.h"
#include "database.h"
#include "deviceid.h"
#include "ConnectionLocationDialog.h"
#include <memory>
#include <QTableWidget>
#include <QLabel>

class FlowTaskManager : public QWidget
{
    Q_OBJECT
public:
    explicit FlowTaskManager(QWidget *parent = nullptr);
    ~FlowTaskManager();

    void drawFlowChart();
    void clearFlowChart();
    QString generateTaskId();
    void loadTaskFromDatabase();
    void saveTaskToDatabase();
    
    // Getters
    QString getCurrentTaskId() const { return currentTaskId; }
    QString getRootTaskId() const { return rootTaskId; }
    int getStepCount() const { return stepCount; }

public slots:
    void createNewTask();
    void addNewStep();
    void handleNodeDeleted(FlowNode* node);
    void handleNodeRun(FlowNode* node);
    void saveCurrentTask();
    void deleteCurrentTask();
    void onDeviceIdChanged(const QString& newDeviceId);
    void showSelectComponentDialog();

signals:
    void flowChartChanged();  // 当流程图发生变化时发出信号

private:
    void setupUI();
    void updateTaskList();
    void connectDatabase();
    void updateTaskInfo();  // 添加更新任务信息的函数
    
    QString rootTaskId;     // 使用字符串保存任务ID
    // 使用容器保存所有步骤节点，顺序为任务步骤顺序
    QList<FlowNode*> stepNodes;
    QString currentTaskId;
    int stepCount;
    QGraphicsScene *scene;
    QGraphicsView *view;
    QLineEdit *taskIdEdit;
    QPushButton *createTaskBtn;
    QPushButton *addStepBtn;
    QPushButton *saveTaskBtn;
    QPushButton *deleteTaskBtn;
    QComboBox *taskListCombo;
    std::shared_ptr<Database> db;
    QString deviceId;
    int currentElementId = -1;  // 当前选中的被测元件ID
    QLabel *currentElementLabel;  // 显示当前被测元件的标签
};

#endif // FLOWTASKMANAGER_H
