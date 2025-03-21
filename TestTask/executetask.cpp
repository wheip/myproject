#include "executetask.h"
#include <QDebug>

ExecuteTask::ExecuteTask(QObject *parent) : QObject(parent),
    m_device5711(nullptr),
    m_device8902(nullptr),
    m_device5322(nullptr),
    m_device5323(nullptr),
    m_taskTimer(nullptr),
    m_activeDeviceCount(0),
    m_completedDeviceCount(0),
    m_have5711(false)
{
    // 获取设备管理器实例
    m_deviceManager = JYDeviceManager::getInstance();

    if (m_deviceManager) {
        // 获取各设备引用
        m_device5711 = m_deviceManager->getDevice5711();
        m_device8902 = m_deviceManager->getDevice8902();
        m_device5322 = m_deviceManager->getDevice5322();
        m_device5323 = m_deviceManager->getDevice5323();

        // 初始化设备连接
        initDeviceConnections();
    } else {
        qWarning() << "DeviceManager 实例获取失败";
    }

    // 初始化任务计时器
    m_taskTimer = new QTimer(this);
    connect(m_taskTimer, &QTimer::timeout, this, [this]() {
        if(m_taskStatus != TaskStatus::Idle) stopAllTasks();
    });
}

ExecuteTask::~ExecuteTask()
{
    // 确保停止所有任务
    stopAllTasks();

    // 断开设备连接
    disconnectDevices();
}

void ExecuteTask::configureTask(const QVector<TaskConfig>& configs)
{
    // 保存任务配置
    m_taskConfigs = configs;

    // 重置计数器
    m_activeDeviceCount = 0;
    m_completedDeviceCount = 0;
    m_have5711 = false;

    // 设置为非连续采集模式 - 添加空指针检查
    if (m_device8902) m_device8902->continueAcquisition = false;
    if (m_device5322) m_device5322->continueAcquisition = false;
    if (m_device5323) m_device5323->continueAcquisition = false;

    // 计算活跃设备数量
    for (const auto& config : m_taskConfigs) {
        if (config.enabled) {
            m_activeDeviceCount++;
            if(config.deviceType == DeviceType::PXIe5711)
            {
                m_have5711 = true;
            }
        }
    }

    m_taskStatus = TaskStatus::Idle;
    // 更新任务状态
    emit taskStatusChanged(m_taskStatus, "任务已配置");
}

bool ExecuteTask::startAllTasks()
{
    // 检查设备管理器是否可用
    if (!m_deviceManager) {
        m_errorMessage = "设备管理器不可用";
        emit taskError(m_errorMessage);
        return false;
    }

    // 检查是否有任务配置
    if (m_taskConfigs.isEmpty()) {
        m_errorMessage = "没有配置任务";
        emit taskError(m_errorMessage);
        return false;
    }

    // 准备设备任务
    if (!prepareDeviceTasks()) {
        return false;
    }

    // 更新任务状态
    m_taskStatus = TaskStatus::Ready;
    emit taskStatusChanged(m_taskStatus, "任务已启动");

    return true;
}

bool ExecuteTask::triggerAllTasks()
{
    // 检查设备管理器是否可用
    if (!m_deviceManager) {
        m_errorMessage = "设备管理器不可用";
        emit taskError(m_errorMessage);
        return false;
    }

    // 检查任务状态
    if (m_taskStatus != TaskStatus::Ready) {
        m_errorMessage = "任务未处于准备状态，无法触发";
        emit taskError(m_errorMessage);
        return false;
    }

    QString errorMsg;
    // 触发各设备
    bool success = true;

    for (const auto& config : m_taskConfigs) {
        if (!config.enabled) continue;

        switch (config.deviceType) {
        case DeviceType::PXIe5711:
            if (m_device5711 && m_device5711->getStatus() == PXIe5711Status::Ready) {
                // 使用QMetaObject::invokeMethod确保线程安全
                QMetaObject::invokeMethod(m_device5711, "SendSoftTrigger", Qt::QueuedConnection);
                if(m_activeDeviceCount == 1)
                {
                    m_taskTimer->start(config.collectTime * 1000);
                }
            } else {
                errorMsg += "输出 设备未准备好；";
                success = false;
            }
            break;

        case DeviceType::PXIe8902:
            if (m_device8902 && m_device8902->getStatus() == PXIe8902Status::Ready) {
                // 使用QMetaObject::invokeMethod确保线程安全
                QMetaObject::invokeMethod(m_device8902, "SendSoftTrigger", Qt::QueuedConnection);
            } else {
                errorMsg += "万用表 设备未准备好；";
                success = false;
            }
            break;

        case DeviceType::PXIe5322:
            if (m_device5322 && m_device5322->getStatus() == PXIe5320Status::Ready) {
                // 使用QMetaObject::invokeMethod确保线程安全
                QMetaObject::invokeMethod(m_device5322, "SendSoftTrigger", Qt::QueuedConnection);
            } else {
                errorMsg += "数字量输入 设备未准备好；";
                success = false;
            }
            break;

        case DeviceType::PXIe5323:
            if (m_device5323 && m_device5323->getStatus() == PXIe5320Status::Ready) {
                // 使用QMetaObject::invokeMethod确保线程安全
                QMetaObject::invokeMethod(m_device5323, "SendSoftTrigger", Qt::QueuedConnection);
            } else {
                errorMsg += "模拟量输入 设备未准备好；";
                success = false;
            }
            break;

        default:
            success = false;
            break;
        }
    }

    if (!success) {
        m_errorMessage = errorMsg;
        m_deviceManager->CloseAllDevices();
        emit taskError(m_errorMessage);
    }
    else
    {
        m_taskStatus = TaskStatus::Running;
        emit taskStatusChanged(m_taskStatus, "任务已触发");
    }

    return success;
}

void ExecuteTask::stopAllTasks()
{
    if(m_taskStatus == TaskStatus::Idle) return;

    // 停止各设备
    for (const auto& config : m_taskConfigs) {
        if (!config.enabled) continue;

        switch (config.deviceType) {
        case DeviceType::PXIe5711:
            if (m_device5711) {
                m_device5711->CloseDevice();
            }
            break;

        case DeviceType::PXIe8902:
            if (m_device8902) {
                // 使用直接调用而不是QMetaObject::invokeMethod
                m_device8902->DeviceClose();
            }
            break;

        case DeviceType::PXIe5322:
            if (m_device5322) {
                // 使用直接调用而不是QMetaObject::invokeMethod
                m_device5322->DeviceClose();
            }
            break;

        case DeviceType::PXIe5323:
            if (m_device5323) {
                // 使用直接调用而不是QMetaObject::invokeMethod
                m_device5323->DeviceClose();
            }
            break;

        default:
            break;
        }
    }

    // 更新任务状态
    if (m_taskStatus == TaskStatus::Running) {
        m_taskStatus = TaskStatus::Completed;
        emit taskStatusChanged(m_taskStatus, "任务已停止");
    }

    // 停止任务计时器
    if (m_taskTimer && m_taskTimer->isActive()) {
        m_taskTimer->stop();
    }
}

void ExecuteTask::onDeviceStateChanged(const QString& state, int code)
{
    // 处理设备状态变化
    if (code < 0) {
        // 设备错误
        m_errorMessage = state;
        m_taskStatus = TaskStatus::Error;
        emit taskError(m_errorMessage);
        emit taskStatusChanged(m_taskStatus, "任务出错: " + state);
    }
}

void ExecuteTask::onDeviceDataReceived(const std::vector<PXIe5320Waveform>& data, int serialNumber)
{
    if(!data.empty())
        qDebug() << "采集设备" << data[0].device << "数据大小" << data[0].data.size();
    if(m_isSaveData)
    {
        m_saveData->addTask(data, serialNumber);
    }
    m_completedDeviceCount++;
    if(m_completedDeviceCount == (m_activeDeviceCount - static_cast<int>(m_have5711)))
    {
        m_taskStatus = TaskStatus::Completed;
        stopAllTasks();
        emit taskStatusChanged(m_taskStatus, "任务已完成");
        emit taskCompleted();
    }
}

void ExecuteTask::initDeviceConnections()
{
    // 连接设备信号
    if (m_device5711) {
        connect(m_device5711, &PXIe5711::StateChanged, this, &ExecuteTask::onDeviceStateChanged);
        connect(m_device5711, &PXIe5711::WaveformGenerated, this, [this]() {
            // 处理波形生成完成
        });
    }

    if (m_device8902) {
        connect(m_device8902, &PXIe8902::StateChanged, this, &ExecuteTask::onDeviceStateChanged);
        connect(m_device8902, &PXIe8902::signalAcquisitionData, this, &ExecuteTask::onDeviceDataReceived);
    }

    if (m_device5322) {
        connect(m_device5322, &PXIe5320::StateChanged, this, &ExecuteTask::onDeviceStateChanged);
        connect(m_device5322, &PXIe5320::signalAcquisitionData, this, &ExecuteTask::onDeviceDataReceived);
    }

    if (m_device5323) {
        connect(m_device5323, &PXIe5320::StateChanged, this, &ExecuteTask::onDeviceStateChanged);
        connect(m_device5323, &PXIe5320::signalAcquisitionData, this, &ExecuteTask::onDeviceDataReceived);
    }
}

void ExecuteTask::disconnectDevices()
{
    // 断开设备连接
    if (m_device5711) {
        disconnect(m_device5711, nullptr, this, nullptr);
    }

    if (m_device8902) {
        disconnect(m_device8902, nullptr, this, nullptr);
    }

    if (m_device5322) {
        disconnect(m_device5322, nullptr, this, nullptr);
    }

    if (m_device5323) {
        disconnect(m_device5323, nullptr, this, nullptr);
    }
}

bool ExecuteTask::prepareDeviceTasks()
{
    if(m_taskStatus != TaskStatus::Idle) return false;
    bool success = true;
    QString errorMsg;
    // 准备各设备任务
    for (const auto& config : m_taskConfigs) {
        if (!config.enabled) continue;

        switch (config.deviceType) {
        case DeviceType::PXIe5711:
            if (m_device5711 && m_device5711->getStatus() == PXIe5711Status::Closed) {
                // 使用 std::visit 从 variant 中提取 PXIe5711Waveform 参数
                std::visit([&](const auto& params) {
                    using T = std::decay_t<decltype(params)>;
                    if constexpr (std::is_same_v<T, std::vector<PXIe5711Waveform>>) {
                        // 直接调用而不是使用QMetaObject::invokeMethod
                        bool result = m_device5711->receivewaveform(params);
                        if(!result) errorMsg += m_device5711->getErrorMessage();
                        success &= result;
                    } else {
                        errorMsg += "输出 参数类型不匹配；";
                        success = false;
                    }
                }, config.parameters);
            } else {
                if(m_device5711->getStatus() != PXIe5711Status::Closed) errorMsg += "输出 设备未关闭；";
                success = false;
            }
            break;

        case DeviceType::PXIe8902:
            if (m_device8902 && m_device8902->getStatus() == PXIe8902Status::Closed) {
                // 使用 std::visit 从 variant 中提取 Data8902 参数
                std::visit([&](const auto& params) {
                    using T = std::decay_t<decltype(params)>;
                    if constexpr (std::is_same_v<T, std::vector<Data8902>>) {
                        // 直接调用而不是使用QMetaObject::invokeMethod
                        bool result = m_device8902->StartAcquisition(params, config.collectTime);
                        if(!result) errorMsg += m_device8902->getErrorMessage();
                        success &= result;
                    } else {
                        errorMsg = "万用表 参数类型不匹配；";
                        success = false;
                    }
                }, config.parameters);
            } else {
                if(m_device8902->getStatus() != PXIe8902Status::Closed) errorMsg += "万用表 设备未关闭；";
                success = false;
            }
            break;

        case DeviceType::PXIe5322:
            if (m_device5322 && m_device5322->getStatus() == PXIe5320Status::Closed) {
                // 使用 std::visit 从 variant 中提取 PXIe5320Waveform 参数
                std::visit([&](const auto& params) {
                    using T = std::decay_t<decltype(params)>;
                    if constexpr (std::is_same_v<T, std::vector<PXIe5320Waveform>>) {
                        // 直接调用
                        bool result = m_device5322->StartAcquisition(params, config.collectTime);
                        if(!result) errorMsg += m_device5322->getErrorMessage();
                        success &= result;
                    } else {
                        errorMsg += "数字量输入 参数类型不匹配；";
                        success = false;
                    }
                }, config.parameters);
            } else {
                if(m_device5322->getStatus() != PXIe5320Status::Closed) errorMsg += "数字量输入 设备未关闭；";
                success = false;
            }
            break;

        case DeviceType::PXIe5323:
            if (m_device5323 && m_device5323->getStatus() == PXIe5320Status::Closed) {
                // 使用 std::visit 从 variant 中提取 PXIe5320Waveform 参数
                std::visit([&](const auto& params) {
                    using T = std::decay_t<decltype(params)>;
                    if constexpr (std::is_same_v<T, std::vector<PXIe5320Waveform>>) {
                        // 直接调用而不是使用QMetaObject::invokeMethod
                        bool result = m_device5323->StartAcquisition(params, config.collectTime);
                        if(!result) errorMsg += m_device5323->getErrorMessage();
                        success &= result;
                    } else {
                        errorMsg += "模拟量输入 参数类型不匹配；";
                        success = false;
                    }
                }, config.parameters);
            } else {
                if(m_device5323->getStatus() != PXIe5320Status::Closed) errorMsg += "模拟量输入 设备未关闭；";
                success = false;
            }
            break;

        default:
            success = false;
            break;
        }
    }

    if (!success) {
        m_errorMessage = errorMsg;
        m_deviceManager->CloseAllDevices();
        emit taskError(m_errorMessage);
    }
    else
    {
        m_taskStatus = TaskStatus::Ready;
        emit taskStatusChanged(m_taskStatus, "任务已准备");
    }

    return success;
}

void ExecuteTask::SetsaveData(int ThreadNum, const QString& TableName, bool saveData)
{
    if(m_saveData != nullptr) delete m_saveData;
    m_saveData = new SaveData(ThreadNum, TableName);
    m_isSaveData = saveData;
}

