#ifndef RUNTASK_H
#define RUNTASK_H
#include "database.h"
#include "executeTask.h"
#include <condition_variable>
#include <QTimer>
#include <mutex>
#include "camera.h"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "taskconnectwire.h"
#include "jydevicemanager.h"

enum class RunTaskStatus {
    Stop,
    Running,
    Paused
};
class RunTask : public QObject
{
    Q_OBJECT
public:
    RunTask();
    ~RunTask();
    void RunTestTask(int& device_id, QString& taskId);
    void InterruptTask();
    void RunStep(Step& step);
    void SetSaveDataPath(QString& path);
    RunTaskStatus GetTaskStatus(){return m_taskStatus;};

signals:
    void taskCompleted();

    void StateChanged(int state, const QString& message);
public slots:
    void begin_collect_image(double time);

    void TemptureMonitor(const IRImageData& imageData);

    void ExecuteTaskStateChanged(TaskStatus state, const QString& message);

    void ConnectWire(Step step);

    void HandleError(const QString& errorMessage);
private:
    Database* m_database;
    ExecuteTask* m_executeTask;
    JYDeviceManager* m_deviceManager;
    std::vector<Step> m_steps;
    QTimer* m_timer;
    std::condition_variable m_cv;
    std::mutex m_mutex;

    std::condition_variable m_connectwire_cv;
    std::mutex m_connectwire_mutex;

    std::mutex m_status_mutex;

    bool m_interrupt = false;
    int m_device_id;
    int m_current_run_step_id;
    QString m_taskId;
    QString m_imagetask_table_name;
    RunTaskStatus m_taskStatus = RunTaskStatus::Stop;
    QVector<TaskConfig> GengerateTaskConfig(Step step);
    void disconnect();
    void Signalconnect();
};

#endif // RUNTASK_H
