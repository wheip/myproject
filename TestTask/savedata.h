#ifndef SAVEDATA_H
#define SAVEDATA_H

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

enum class SaveDataStatus
{
    Running,
    Finished
};

class SaveData
{
public:
    SaveData(int threadnums, const QString& TableName);
    ~SaveData();

    void addTask(const std::vector<PXIe5320Waveform>& data, int serial_number);

    bool getTask(PXIe5320Waveform& data);

    void waitForTask();

    bool isFinished(bool taskCompleted);

    bool isStop();

    SaveDataStatus getStatus(){return status;};
private:
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
    SaveDataStatus status = SaveDataStatus::Finished;


    void worker();

    void processTask(const PXIe5320Waveform& data);
    
    void stopWorkers();
};

#endif // SAVEDATA_H
