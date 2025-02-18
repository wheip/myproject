#ifndef PXIE8902_H
#define PXIE8902_H

#include <QtCore/QTimer>
#include <QMessageBox>
#include <QObject>
#include <QtCharts/QChartView>
#include <QtCore/QTimer>
#include <QLineSeries>
#include <QLabel>
#include "JY8902.h"
#include "ClassList.h"
#include <iostream>
#include <mutex>

using namespace std;

class PXIe8902 : public QObject
{
    Q_OBJECT
public:
    PXIe8902(QObject *parent = nullptr);
    ~PXIe8902();

    void DeviceClose();

signals:
    void signalAcquisitionData(const std::vector<PXIe5320Waveform>& data, int serial_number);

    void CompleteAcquisition();

    void DeviceReady();

    void StateChanged(const QString& state, int numb);
public slots:
    bool StartAcquisition(std::vector<Data8902> collectdata, double collecttime);

    void InterruptAcquisition();

    void SendSoftTrigger();

private:
    std::mutex mtx;
    double* pDataBuf;   ///< buffer pointer for read data

    JY8902_DeviceHandle hDevice;  ///< device handle

    double sampleInterval;

    vector<vector<float>> AcqData;

    int samplesToAcq;

    int SampleRate;

    int TotalSamplesToAcq;

    int SampleRemain;

    int serial_number;

    QString testtype;

    QString errorMsg;

    bool isStarted = false;

    std::vector<PXIe5320Waveform> collectdata;

    bool InitDevice(QString& errorMsg);

    void FetchData();

    void handleError();
};

#endif // PXIE8902_H
