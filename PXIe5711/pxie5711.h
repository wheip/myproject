#ifndef PXIE5711_H
#define PXIE5711_H
#include <QObject>
#include <ClassList.h>
#include "JY5710.h"
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <iostream>
using namespace std;
class PXIe5711 : public QObject
{
    Q_OBJECT
public:
    explicit PXIe5711(QObject *parent = nullptr);
    ~PXIe5711();
    void CloseDevice();
    void StopDevice();
public slots:
    bool receivewaveform(std::vector<PXIe5711Waveform> waveforms5711);

    void InterruptAcquisition();

    void SendSoftTrigger();

signals:
    void WaveformGenerated();

    void DeviceReady();

    void StateChanged(const QString& state, int numb);
private:
    bool InitDevice(QString& errorMsg);
    void createwaveform(PXIe5711Waveform waveform);
    void WaveformGenerate();
    void updatadisplay(std::vector<PXIe5711Waveform> waveforms5711);

    // Ui::PXIe5711 *ui;
    bool isStarted = false;          ///< flag of start status
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
};

#endif // PXIE5711_H
