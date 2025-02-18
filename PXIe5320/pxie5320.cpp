#include "pxie5320.h"
#include <QThread>
#include <QEventLoop>
#include <memory>
#include <QDebug>

PXIe5320::PXIe5320(QObject *parent, int cardID)
    : QObject(parent),
      cardID(cardID),
      slotNumber((cardID == 5323) ? 3 : 5),
      channelCount((cardID == 5323) ? 32 : 16),
      SampleRate((cardID == 5323) ? 200000 : 1000000),
      SamplesToAcq(SampleRate),
      pChannels(std::make_unique<unsigned char[]>(channelCount)),
      pRangeLow(std::make_unique<double[]>(channelCount)),
      pRangeHi(std::make_unique<double[]>(channelCount)),
      pBandWidth(std::make_unique<JY5320_AI_BandWidth[]>(channelCount)),
      AcqData(channelCount)
{
    for(int i = 0; i < channelCount; i++)
    {
        pChannels[i] = static_cast<unsigned char>(i);
        pRangeHi[i] = highRange;
        pRangeLow[i] = lowRange;
        pBandWidth[i] = static_cast<JY5320_AI_BandWidth>(BandWidth);
    }
}

PXIe5320::~PXIe5320() {
    std::lock_guard<std::mutex> lock(mtx);
    if(pDataBuf)
    {
        delete[] pDataBuf;
        pDataBuf = nullptr;
    }
    
    if(hDevice)
    {
        if(JY5320_SetDeviceStatusLed(hDevice, false, 0) != 0)
        {
            errorMsg = "cardID:" + QString::number(cardID) + "_SetDeviceStatusLed: set failed!";
            qDebug() << errorMsg;
        }
        if(JY5320_AI_Stop(hDevice) != 0)
        {
            errorMsg = "JY5320_AI_Stop: failed!";
            qDebug() << errorMsg;
        }
        
        if(JY5320_Close(hDevice) != 0)
        {
            errorMsg = "JY5320_Close: failed!";
            qDebug() << errorMsg;
        }
        hDevice = nullptr;
        isStarted = false;
        qDebug() << "PXIe5320::~PXIe5320()";
    }
}

bool PXIe5320::InitDevice(QString& errorMsg) {
    if(JY5320_Open(slotNumber, &hDevice) != 0)
    {
        errorMsg = "cardID:" + QString::number(cardID) + "_JY5320_Open: Open device failed!";
        qDebug() << errorMsg;
        return false;
    }
    if(JY5320_AI_EnableChannel(hDevice, channelCount, pChannels.get(), pRangeLow.get(), pRangeHi.get(), pBandWidth.get()) != 0)
    {
        errorMsg = "cardID:" + QString::number(cardID) + "_JY5320_AI_EnableChannel: set failed!";
        qDebug() << errorMsg;
        return false;
    }
    if(JY5320_AI_SetMode(hDevice, JY5320_AI_SampleMode::JY5320_AI_Continuous) != 0)
    {
        errorMsg = "cardID:" + QString::number(cardID) + "_JY5320_AI_SetMode: set failed!";
        qDebug() << errorMsg;
        return false;
    }
    if(JY5320_AI_SetStartTriggerType(hDevice, JY5320_AI_TriggerType::JY5320_AI_Soft) != 0) //software start trigger type
    {
        errorMsg = "cardID:" + QString::number(cardID) + "_JY5320_AI_SetStartTriggerType: set failed!";
        qDebug() << errorMsg;
        return false;
    }
    if(JY5320_AI_SetSampleRate(hDevice, this->SampleRate, &this->actualSampleRate) != 0)
    {
        errorMsg = "cardID:" + QString::number(cardID) + "_JY5320_AI_SetSampleRate: set failed!";
        qDebug() << errorMsg;
        return false;
    }
    if(JY5320_SetDeviceStatusLed(hDevice, true, 10) != 0)
    {
        errorMsg = "cardID:" + QString::number(cardID) + "_JY5320_SetDeviceStatusLed: set failed!";
        qDebug() << errorMsg;
        return false;
    }
    if(JY5320_AI_Start(hDevice) != 0)
    {
        errorMsg = "cardID:" + QString::number(cardID) + "_JY5320_AI_Start: start AI failed!";
        return false;
    }
    
    pDataBuf = new double[channelCount * SamplesToAcq];
    return true;
}

bool PXIe5320::StartAcquisition(std::vector<PXIe5320Waveform> collectdata, double collecttime) {
    this->TotalSamplesToAcq = static_cast<int>(SampleRate * collecttime);
    qDebug() << "采集点数" << TotalSamplesToAcq;
    this->SamplesToAcq = (SampleRate * collecttime) > SampleRate ? SampleRate : (SampleRate * collecttime);

    for(int c = 0; c < channelCount; c++)
    {
        AcqData[c].clear();
    }
    serial_number = 0;
    if(!InitDevice(errorMsg)){
        qDebug() << errorMsg;
        emit StateChanged(errorMsg, -1);
        handleError();
        emit CompleteAcquisition();
        return false;
    }

    this->collectdata = std::move(collectdata);

    emit DeviceReady();
}

void PXIe5320::SendSoftTrigger()
{
    if(isStarted) return;
    QThread* fetchThread = QThread::create([this]() {
        while (isStarted) {
            this->FetchData();
            QThread::msleep(10); // 相当于10ms的定时器间隔
        }
    });
    
    connect(fetchThread, &QThread::finished, fetchThread, &QObject::deleteLater);
    fetchThread->start();

    this->isStarted = true;
    if(JY5320_AI_SendSoftTrigger(hDevice, JY5320_TriggerMode::JY5320_StartTrigger)!= 0)
    {
        errorMsg = "cardID:" + QString::number(cardID) + "_JY5320_AI_SendSoftTrigger: failed!";
        qDebug() << errorMsg;
        emit StateChanged(errorMsg, -1);
        handleError();
        return;
    }
    return;
}

void PXIe5320::DeviceClose()
{
    if(!isStarted) return;
    if(hDevice)
    {
        if(JY5320_SetDeviceStatusLed(hDevice, false, 0) != 0)
        {
            errorMsg = "cardID:" + QString::number(cardID) + "_SetDeviceStatusLed: set failed!";
            qDebug() << errorMsg;
        }
        if(JY5320_AI_Stop(hDevice) != 0)
        {
            errorMsg = "cardID:" + QString::number(cardID) + "_JY5320_AI_Stop: failed!";
            qDebug() << errorMsg;
        }
        if(JY5320_Close(hDevice) != 0)
        {
            errorMsg = "cardID:" + QString::number(cardID) + "_JY5320_Close: failed!";
            qDebug() << errorMsg;
        }
        hDevice = nullptr;
        qDebug() << "cardID:" << cardID << "Close";
    }
    this->isStarted = false;
}

void PXIe5320::FetchData()
{
    if(!isStarted) return;

    unsigned long long availableSamples = 0;
    unsigned int actualSample = 0;
    unsigned long long transferedSamples = 0;
    bool overRun = false;

    if(JY5320_AI_CheckBufferStatus(hDevice, &availableSamples, &transferedSamples, &overRun) != 0)
    {
        errorMsg = "cardID:" + QString::number(cardID) + "_JY5320_AI_CheckBufferStatusEx: failed!";
        handleError();
        return;
    }

    if(availableSamples < SamplesToAcq)
    {
        return;
    }

    if(JY5320_AI_ReadData(hDevice, pDataBuf, SamplesToAcq, -1, &actualSample) != 0)
    {
        errorMsg = "cardID:" + QString::number(cardID) + "_JY5320_AI_ReadData: failed!";
        handleError();
        return;
    }

    for(int c = 0; c < channelCount; c++)
    {
        for(int i = 0; i < SamplesToAcq; i++)
        {
            AcqData[c].push_back(pDataBuf[i * channelCount + c]);
        }
    }

    this->SampleRemain = TotalSamplesToAcq - transferedSamples - SamplesToAcq;
    SamplesToAcq = SampleRemain > SampleRate ? SampleRate : SampleRemain;

    for(auto &collec : collectdata)
    {
        collec.data.insert(collec.data.end(), AcqData[collec.port].begin(), AcqData[collec.port].end());
    }

    for(int c = 0; c < channelCount; c++)
    {
        AcqData[c].clear();
    }

    if(SampleRemain > 0)
    {
        qDebug() << "总采集点" << TotalSamplesToAcq << "已采集" << transferedSamples;
        return;
    }

    emit signalAcquisitionData(collectdata, serial_number);

    DeviceClose();
    emit CompleteAcquisition();
    qDebug() << "cardID:" << cardID << "CompleteAcquisition";

    delete[] pDataBuf;
    pDataBuf = nullptr;
}

void PXIe5320::handleError()
{
    if(pDataBuf)
    {
        delete[] pDataBuf;
        pDataBuf = nullptr;
    }
    SamplesToAcq = 0;
    availableSamples = 0;
    transferedSamples = 0;

    QMetaObject::invokeMethod(this, [this]() {
        qDebug() << errorMsg;
    }, Qt::QueuedConnection);
}

void PXIe5320::InterruptAcquisition()
{
    TotalSamplesToAcq = 0;
}
