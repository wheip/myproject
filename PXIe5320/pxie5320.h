#ifndef PXIE5320_H
#define PXIE5320_H
#include <QtCore/QTimer>
#include <QObject>
#include <QtCharts/QChartView>
#include <QtCore/QTimer>
#include <QLineSeries>
#include <QLabel>
#include "JY5320Core.h"
#include "ClassList.h"
#include <iostream>
#include <mutex>
#include <condition_variable>

using namespace std;

// 设备状态枚举
enum class PXIe5320Status {
    Closed,     // 关闭
    Ready,      // 准备
    Running     // 运行
};

class PXIe5320 : public QObject 
{
    Q_OBJECT
public:
    PXIe5320(QObject *parent = nullptr, int cardID = 5323);
    ~PXIe5320();

    int cardID;

    bool continueAcquisition = false;
    
    // 获取当前设备状态
    PXIe5320Status getStatus() const { std::lock_guard<std::mutex> lock(mtx); return m_status; }

    QString getErrorMessage() const { std::lock_guard<std::mutex> lock(mtx); return errorMsg; }

    bool SelfTest();

signals:
    void signalAcquisitionData(const std::vector<PXIe5320Waveform> collectdata, int serial_number);

    void CompleteAcquisition();

    void DeviceReady();

    void StateChanged(const QString& state, int numb);

public slots:
    bool StartAcquisition(std::vector<PXIe5320Waveform> collectdata, double collecttime);

    void InterruptAcquisition();

    bool SendSoftTrigger();

    void DeviceClose();

private:
    mutable std::mutex mtx;
    std::condition_variable cv;  // 用于暂停/恢复线程
    bool isStarted = false;      // 设备是否启动
    bool shouldExit = false;     // 线程是否最终退出（析构时使用）
    bool paused = false;         // 线程是否暂停（设备关闭时置为 true）
    PXIe5320Status m_status = PXIe5320Status::Closed; // 设备状态
    
    unsigned int channelCount;
    std::unique_ptr<unsigned char[]> pChannels;
    std::unique_ptr<double[]> pRangeLow;
    std::unique_ptr<double[]> pRangeHi;
    std::unique_ptr<JY5320_AI_BandWidth[]> pBandWidth;
    double* pDataBuf = nullptr;   ///< buffer pointer for read data
    double lowRange = -10;    ///< low range
    double highRange = 10;   ///< high range
    JY5320_DeviceHandle hDevice = nullptr;  ///< device handle
    int BandWidth = 0;
    QString errorMsg;
    QThread* fetchThread = nullptr;

    vector<vector<float>> AcqData;
    int slotNumber;   ///< slot number
    int SamplesToAcq; // 设备每次采样点数
    int TotalSamplesToAcq; // 总采样点数
    int SampleRemain; // 剩余的点数
    int SampleRate; // 采样率 samples/s 5322最大1M/s 5323最大200k/s
    int serial_number = 0; // 采集序列号
    double actualSampleRate = 0;
    unsigned long long availableSamples = 0;
    unsigned int actualSample = 0;
    unsigned long long transferedSamples=0;
    std::vector<PXIe5320Waveform> collectdata;

    bool InitDevice(QString& errorMsg);

    void updatadisplay(std::vector<PXIe5320Waveform> &collectdata);
    void FetchData();
    void handleError();
    
    // 更新设备状态并发送信号
    void updateStatus(PXIe5320Status status);
};

#endif // PXIE5320_H
