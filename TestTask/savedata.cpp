#include "savedata.h"

SaveData::SaveData(int threadnums, const QString& TableName) : threadnums(threadnums), TableName(TableName) {
    device_id = TableName.split("$$")[0];
    task_id = TableName.split("$$")[1];
    time = TableName.split("$$")[2];
    for(int i = 0; i < threadnums; i++) {
        workers.emplace_back([this]() { 
            try
            {
                worker();
            }
            catch(const std::exception& e)
            {
                qDebug() << e.what();
            }
        });
    }
};

SaveData::~SaveData() {
    stopWorkers();
}

void SaveData::addTask(const std::vector<PXIe5320Waveform>& data, int serial_number) {
    std::lock_guard<std::mutex> lock(mtx);
    for(const auto& waveform : data) {
        taskQueue.push(waveform);
        Taskcv.notify_one(); // 通知一个等待的线程
    }
}

bool SaveData::getTask(PXIe5320Waveform& data) {
    std::lock_guard<std::mutex> lock(mtx);
    if (taskQueue.empty()) {
        return false;
    }
    data = taskQueue.front();
    taskQueue.pop();
    return true;
}

void SaveData::waitForTask() {
    std::unique_lock<std::mutex> lock(mtx);
    Taskcv.wait(lock, [this] { return !taskQueue.empty() || stop; });
}

bool SaveData::isFinished(bool taskCompleted) {
    std::lock_guard<std::mutex> lock(mtx);
    if(taskCompleted && taskQueue.empty())
    {
        stop = true;
        Taskcv.notify_all();
        return true;
    }
    return false;
}

bool SaveData::isStop() {
    std::lock_guard<std::mutex> lock(mtx);
    return stop && taskQueue.empty();
}

void SaveData::worker() {
    while (true) {
        PXIe5320Waveform task;
        waitForTask(); // 等待任务
        if (isStop()) {
            break;
        }
        if (getTask(task)) {
            // 执行任务
            processTask(task);
        }
    }
}

void SaveData::processTask(const PXIe5320Waveform& data) {
    // 创建保存路径
    QString FolderPath = "./CollectData/" + device_id + "/" + task_id + "/" + time + "/" + QString::number(data.device);
    if(!QDir(FolderPath).exists()) {
        QDir().mkpath(FolderPath);
    }
    QString FilePath = FolderPath + "/" + QString::number(data.id) + ".mmap";
    
    QFile file(FilePath);
    if(!file.open(QIODevice::WriteOnly | QIODevice::ReadWrite)) {  // 需要读写权限
        throw std::runtime_error("Failed to open files for writing.");
    }
    
    // 计算数据大小（字节数）
    qint64 dataSize = data.data.size() * sizeof(float);
    
    // 设置文件大小
    if (!file.resize(dataSize)) {
        file.close();
        throw std::runtime_error("Failed to resize file.");
    }
    
    // 创建内存映射
    uchar* mappedMemory = file.map(0, dataSize);
    if (!mappedMemory) {
        file.close();
        throw std::runtime_error("Failed to create memory mapping.");
    }
    
    try {
        // 复制数据到映射内存
        memcpy(mappedMemory, data.data.data(), dataSize);
        
        // 确保数据写入到磁盘
        if (!file.flush()) {
            throw std::runtime_error("Failed to flush data to disk.");
        }
    }
    catch (const std::exception& e) {
        // 确保清理资源
        file.unmap(mappedMemory);
        file.close();
        throw;
    }
    
    // 解除映射
    if (!file.unmap(mappedMemory)) {
        file.close();
        throw std::runtime_error("Failed to unmap memory.");
    }
    
    file.close();
}

void SaveData::stopWorkers() {
    {std::lock_guard<std::mutex> lock(mtx);
    stop = true;
    Taskcv.notify_all();}
    for (auto &worker : workers) {
        if(worker.joinable())
        {
            worker.join();
        }
    }
}