#ifndef PXIE5711_H
#define PXIE5711_H
#include <QObject>
#include <ClassList.h>
#include "JY5710.h"
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <iostream>
#include <mutex>

using namespace std;

// 设备状态枚举
enum class PXIe5711Status {
    Closed,     // 关闭
    Ready,      // 准备
    Running     // 运行
};

class PXIe5711 : public QObject
{
    Q_OBJECT
public:
    explicit PXIe5711(QObject *parent = nullptr);
    ~PXIe5711();
    
    // 获取当前设备状态
    PXIe5711Status getStatus() const { std::lock_guard<std::mutex> lock(mtx); return m_status; }

    QString getErrorMessage() const { std::lock_guard<std::mutex> lock(mtx); return m_errorMessage; }

    bool SelfTest();

public slots:
    bool receivewaveform(std::vector<PXIe5711Waveform> waveforms5711);

    void CloseDevice();

    bool SendSoftTrigger();

    void InitializeDevice();

signals:
    void WaveformGenerated();

    void DeviceReady();

    void StateChanged(const QString& state, int numb);
    
private:
    mutable std::mutex mtx;
    bool InitDevice(QString& errorMsg);
    void createwaveform(PXIe5711Waveform waveform);
    void WaveformGenerate();
    void updatadisplay(std::vector<PXIe5711Waveform> waveforms5711);
    
    // 更新设备状态并发送信号
    void updateStatus(PXIe5711Status status);

    bool isStarted = false;          ///< flag of start status
    PXIe5711Status m_status = PXIe5711Status::Closed; ///< 设备状态
    QString m_errorMessage;
    
    QTimer timerWriteData;  ///< timer for writing data
    double* pDataBuf;   ///< buffer pointer for write data
    double lowRange = -10;    ///< low range
    double highRange = 10;   ///< high range
    JY5710_DeviceHandle hDevice;  ///< device handle
    std::vector<Waveform*> waveforms;
    int channelCount;
    int SampleRate;
    unsigned int actualWriteSamples = 0;
    std::vector<int> enableChannel = {1};
    unsigned char* pChannels;
    std::vector<int> pChannelsIndex;

    void StopDevice();
};

#endif // PXIE5711_H
