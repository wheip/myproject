#ifndef EXECUTETASK_H
#define EXECUTETASK_H

#include <QObject>
#include <QTimer>
#include <QVector>
#include <QMap>
#include <functional>
#include "jydevicemanager.h"
#include "ClassList.h"
#include "savedata.h"


// 任务状态枚举
enum class TaskStatus {
    Idle,       // 空闲
    Ready,      // 准备
    Running,    // 运行中
    Completed,  // 已完成
    Error       // 错误
};

// 设备类型枚举
enum class DeviceType {
    PXIe5711,
    PXIe8902,
    PXIe5322,
    PXIe5323,
    None
};

// 任务配置结构体
struct TaskConfig {
    DeviceType deviceType;
    double collectTime;
    bool enabled = false;
    std::variant<std::vector<PXIe5711Waveform>, std::vector<Data8902>, std::vector<PXIe5320Waveform>> parameters;
};

class ExecuteTask : public QObject
{
    Q_OBJECT
public:
    explicit ExecuteTask(QObject *parent = nullptr);
    ~ExecuteTask();

    // 配置任务
    void configureTask(const QVector<TaskConfig>& configs);

    // 获取当前任务状态
    TaskStatus getTaskStatus() const { return m_taskStatus; }

    // 获取错误信息
    QString getErrorMessage() const { return m_errorMessage; }

signals:
    // 任务状态变化信号
    void taskStatusChanged(TaskStatus status, const QString& message);

    // 任务错误信号
    void taskError(const QString& errorMessage);

    // 数据更新信号
    void dataUpdated(DeviceType deviceType, const QVariant& data);

    // 任务完成信号
    void taskCompleted();

public slots:
    // 启动所有任务
    bool startAllTasks();

    // 触发所有任务
    bool triggerAllTasks();

    // 停止所有任务
    void stopAllTasks();

    // 设备数据接收槽
    void onDeviceDataReceived(const std::vector<PXIe5320Waveform>& data, int serialNumber);

    // 设备状态变化槽
    void onDeviceStateChanged(const QString& state, int code);

    // 保存数据槽
    void SetsaveData(int ThreadNum, const QString& TableName, bool saveData);

private:
    // 初始化设备连接
    void initDeviceConnections();

    // 断开设备连接
    void disconnectDevices();

    // 准备设备任务
    bool prepareDeviceTasks();

    // 设备管理器
    JYDeviceManager* m_deviceManager;

    // 设备引用
    PXIe5711* m_device5711;
    PXIe8902* m_device8902;
    PXIe5320* m_device5322;
    PXIe5320* m_device5323;

    SaveData* m_saveData = nullptr;

    // 任务配置
    QVector<TaskConfig> m_taskConfigs;

    // 任务状态
    TaskStatus m_taskStatus = TaskStatus::Idle;

    // 任务计时器
    QTimer* m_taskTimer;

    // 活跃设备计数
    int m_activeDeviceCount = 0;

    // 已完成设备计数
    int m_completedDeviceCount = 0;

    // 是否存在5711设备
    bool m_have5711 = false;

    // 是否保存数据
    bool m_isSaveData = false;

    // 错误消息
    QString m_errorMessage;
};

#endif // EXECUTETASK_H
