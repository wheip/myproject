#ifndef PXIE8902_H
#define PXIE8902_H

#include <QtCore/QTimer>
#include <QObject>
#include <QtCharts/QChartView>
#include <QtCore/QTimer>
#include <QLineSeries>
#include <QLabel>
#include "JY8902.h"
#include "ClassList.h"
#include <iostream>
#include <mutex>
#include <condition_variable>

using namespace std;

// 设备状态枚举
enum class PXIe8902Status {
    Closed,     // 关闭
    Ready,      // 准备
    Running     // 运行
};

class PXIe8902 : public QObject
{
    Q_OBJECT
public:
    PXIe8902(QObject *parent = nullptr);
    ~PXIe8902();

    bool continueAcquisition = false;
    
    // 获取当前设备状态
    PXIe8902Status getStatus() const { std::lock_guard<std::mutex> lock(mtx); return m_status; }

    QString getErrorMessage() const { std::lock_guard<std::mutex> lock(mtx); return errorMsg; }

    bool SelfTest();

signals:
    void signalAcquisitionData(const std::vector<PXIe5320Waveform>& data, int serial_number);

    void CompleteAcquisition();

    void DeviceReady();

    void StateChanged(const QString& state, int numb);
public slots:
    bool StartAcquisition(std::vector<Data8902> collectdata, double collecttime);

    void InterruptAcquisition();

    bool SendSoftTrigger();

    void DeviceClose();

private:
    double* pDataBuf = nullptr;   ///< buffer pointer for read data

    JY8902_DeviceHandle hDevice = nullptr;  ///< device handle

    double sampleInterval;

    vector<vector<float>> AcqData;

    int samplesToAcq;

    int SampleRate;

    int TotalSamplesToAcq;

    int SampleRemain;

    int serial_number;

    QThread* fetchThread = nullptr;

    PXIe8902_testtype testtype;

    bool signaltype = true; //被测信号类型，true表示交流，false表示直流

    QString errorMsg;

    bool isStarted = false;
    bool shouldExit = false;         ///< flag for thread exit
    mutable std::mutex mtx;
    std::condition_variable cv;
    bool paused = false;
    PXIe8902Status m_status = PXIe8902Status::Closed; ///< 设备状态

    std::vector<PXIe5320Waveform> collectdata;

    bool InitDevice(QString& errorMsg);

    void FetchData();

    void handleError();
    
    // 更新设备状态并发送信号
    void updateStatus(PXIe8902Status status);
};

#endif // PXIE8902_H
