#ifndef PXIE5320_H
#define PXIE5320_H
#include <QtCore/QTimer>
#include <QMessageBox>
#include <QObject>
#include <QtCharts/QChartView>
#include <QtCore/QTimer>
#include <QLineSeries>
#include <QLabel>
#include "JY5320Core.h"
#include "ClassList.h"
#include <iostream>
#include <mutex>

using namespace std;

class PXIe5320 : public QObject 
{
    Q_OBJECT
public:
    PXIe5320(QObject *parent = nullptr, int cardID = 5323);
    ~PXIe5320();

    int cardID;
    void DeviceClose();

signals:
    void signalAcquisitionData(const std::vector<PXIe5320Waveform> collectdata, int serial_number);

    void CompleteAcquisition();

    void DeviceReady();

    void StateChanged(const QString& state, int numb);
public slots:
    bool StartAcquisition(std::vector<PXIe5320Waveform> collectdata, double collecttime);

    void InterruptAcquisition();

    void SendSoftTrigger();

private:
    std::mutex mtx;
    bool isStarted = false;          ///< flag of start status
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
};

#endif // PXIE5320_H
