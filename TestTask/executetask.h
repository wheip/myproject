#ifndef EXECUTETASK_H
#define EXECUTETASK_H
#include "ClassList.h"
#include <vector>
#include "database.h"
#include "pxie5711.h"
#include "pxie5320.h"
#include "pxie8902.h"
#include <iostream>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <thread>
#include <queue>
#include <condition_variable>
#include <QTextCodec>
#include <QStringConverter>

using namespace std;


class TaskManager {
public:
    TaskManager(int threadnums, const QString& TableName) : threadnums(threadnums), TableName(TableName) {
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
    ~TaskManager() {
        qDebug() << "TaskManager Destructor";
        stopWorkers();
        qDebug() << "TaskManager Finished";
    }

    void addTask(const std::vector<PXIe5320Waveform>& data, int serial_number) {
        std::lock_guard<std::mutex> lock(mtx);
        for(const auto& waveform : data) {
            taskQueue.push(waveform);
            Taskcv.notify_one(); // 通知一个等待的线程
        }
    }

    bool getTask(PXIe5320Waveform& data) {
        std::lock_guard<std::mutex> lock(mtx);
        if (taskQueue.empty()) {
            return false;
        }
        data = taskQueue.front();
        taskQueue.pop();
        return true;
    }

    void waitForTask() {
        std::unique_lock<std::mutex> lock(mtx);
        Taskcv.wait(lock, [this] { return !taskQueue.empty() || stop; });
    }

    bool isFinished(bool taskCompleted) {
        std::lock_guard<std::mutex> lock(mtx);
        if(taskCompleted && taskQueue.empty())
        {
            stop = true;
            Taskcv.notify_all();
            return true;
        }
        return false;
    }

    bool isStop() {
        std::lock_guard<std::mutex> lock(mtx);
        return stop && taskQueue.empty();
    }

private:
    void worker() {
        while (true) {
            PXIe5320Waveform task;
            waitForTask(); // 等待任务
            if (isStop()) {
                qDebug() << "线程结束";
                break;
            }
            if (getTask(task)) {
                // 执行任务
                qDebug() << "执行任务";
                processTask(task);
            }
        }
    }

    void processTask(const PXIe5320Waveform& data) {
        // 创建保存路径
        QString FolderPath = "./CollectData/" + device_id + "/" + task_id + "/" + time + "/" + QString::number(data.device);
        if(!QDir(FolderPath).exists()) {
            QDir().mkpath(FolderPath);
        }
        QString FilePath = FolderPath + "/" + data.id + ".mmap";
        
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

    void stopWorkers() {
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

    std::queue<PXIe5320Waveform> taskQueue;
    std::vector<std::thread> workers;
    std::mutex mtx;
    std::condition_variable Taskcv;
    int threadnums;
    QString TableName;
    QString device_id;
    QString task_id;
    QString time;
    bool stop = false;
};

class ExecuteTask : public QObject
{
    Q_OBJECT
public:
    ExecuteTask(const QString taskid = "", const QString TableName = "", bool single_step = false, Step step_signal = Step());
    ~ExecuteTask();

    void StartTask();

    void InterruptTask();

    void ContinueTask();

public slots:
    void SlotAcquisitionData_5322(const std::vector<PXIe5320Waveform> collectdata, int serial_number);

    void SlotAcquisitionData_5323(const std::vector<PXIe5320Waveform> collectdata, int serial_number);

    void SlotAcquisitionData_8902(const std::vector<PXIe5320Waveform> collectdata, int serial_number);

    void device_StateChanged(const QString& state, int numb);

signals:

    void StateChanged(QString state, int stepNumber);

    void GetInfraredImage(Step step);

    void TaskFinished();

private:
    std::condition_variable cv;
    std::condition_variable cv_5711;
    bool paused = false;
    bool single_step = false;
    Step step_signal;
    std::mutex mtx;
    std::mutex mtx_5711;
    QString taskid;
    QString connectionName;
    QString TableName;
    QString device_id;
    int step_5322collected_time;
    int step_5323collected_time;
    QThread *timerthread;
    bool is5711Generated = true, is5322Acquired = true, is5323Acquired = true, is8902Acquired = true;
    bool is5711Ready = false, is5322Ready = false, is5323Ready = false, is8902Ready = false;
    bool taskCompleted = false;
    bool continueExecution = true;
    bool isInterrupted = false;
    QByteArray connection_image_data;

    std::vector<TestTask> testtask;
    std::vector<Step> step;
    std::vector<PXIe5711Waveform> pxi5711waveform = {};
    std::vector<PXIe5320Waveform> pxi5322waveform = {};
    std::vector<PXIe5320Waveform> pxi5323waveform = {};
    std::vector<Data8902> pxi8902waveform = {};


    Database database;
    QThread pxi5711thread;
    QThread pxi5322thread;
    QThread pxi5323thread;
    QThread pxi8902thread;
    QThread* RunTaskThread;

    std::shared_ptr<PXIe5711> pxi5711;
    std::shared_ptr<PXIe5320> pxi5322;
    std::shared_ptr<PXIe5320> pxi5323;
    std::shared_ptr<PXIe8902> pxi8902;
    std::shared_ptr<TaskManager> taskManager;

    void initializeDevices();
    void loadTaskData(const QString& taskid);
    void setupDeviceThreads();
    void connectDeviceSignals();
    void startMonitoringThread();
    void WaveformAcquisition(Step& s);
    void WaveformGeneration(Step& s);
    void isTaskComplete();
    void connectDeviceSignals_5322();
    void connectDeviceSignals_5323();
    void connectDeviceSignals_5711();
    void connectDeviceSignals_8902();
};
#endif // EXECUTETASK_H
