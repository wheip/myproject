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
    
    // 初始状态为关闭
    updateStatus(PXIe5320Status::Closed);
}

PXIe5320::~PXIe5320() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        shouldExit = true;
        cv.notify_all();
    }
    
    if(fetchThread) {
        if(QThread::currentThread() != fetchThread) {
            fetchThread->quit();
            fetchThread->wait();
            delete fetchThread;
        } else {
            fetchThread->disconnect();
        }
        fetchThread = nullptr;
    }
    
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
        updateStatus(PXIe5320Status::Closed);
        qDebug() << "PXIe5320::~PXIe5320()";
    }
}

bool PXIe5320::SelfTest() {
    {std::lock_guard<std::mutex> lock(mtx);
    if(m_status != PXIe5320Status::Closed) return true;}
    if(JY5320_Open(slotNumber, &hDevice) != 0)
    {
        errorMsg = "cardID:" + QString::number(cardID) + "_JY5320_Open: Open device failed!";
        return false;
    }
    JY5320_AI_Stop(hDevice);
    JY5320_Close(hDevice);
    return true;
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
    // 如果设备正在运行，先关闭
    if (m_status != PXIe5320Status::Closed) {
        DeviceClose();
    }
    
    this->TotalSamplesToAcq = static_cast<int>(SampleRate * collecttime);
    this->SamplesToAcq = (SampleRate * collecttime) > SampleRate ? SampleRate : (SampleRate * collecttime);
    
    for(int c = 0; c < channelCount; c++)
    {
        AcqData[c].clear();
    }
    serial_number = 0;
    
    if(!InitDevice(errorMsg)){
        handleError();
        return false;
    }
    
    this->collectdata = std::move(collectdata);
    
    // 更新状态为准备
    updateStatus(PXIe5320Status::Ready);
    emit StateChanged("设备已准备好", 0);
    emit DeviceReady();
    
    return true;
}

bool PXIe5320::SendSoftTrigger()
{
    // 只有在准备状态才能触发
    if (m_status != PXIe5320Status::Ready) {
        errorMsg = "cardID:" + QString::number(cardID) + " 设备未准备好，无法触发";
        // emit StateChanged(errorMsg, -1);
        return false;
    }
    
    {
        std::lock_guard<std::mutex> lock(mtx);
        isStarted = true;
        paused = false;  // 恢复运行
    }
    
    // 如果线程已存在，则直接唤醒；否则创建新线程
    if (fetchThread == nullptr) {
        fetchThread = new QThread();
        QObject* worker = new QObject();
        worker->moveToThread(fetchThread);
        connect(fetchThread, &QThread::started, worker, [this, worker]() {
            while (true) {
                std::unique_lock<std::mutex> lock(mtx);
                // 等待直到不处于暂停状态或需要退出
                cv.wait(lock, [this](){ return !paused || shouldExit; });
                if (shouldExit)
                    break;
                lock.unlock();
                this->FetchData();
                QThread::msleep(1);
            }
            QMetaObject::invokeMethod(worker, "deleteLater", Qt::QueuedConnection);
        });
        fetchThread->start();
    } else {
        // 线程已经存在，则清除暂停标志并通知继续执行
        {
            std::lock_guard<std::mutex> lock(mtx);
            paused = false;
        }
        cv.notify_all();
    }
    
    // 发送软件触发
    if(JY5320_AI_SendSoftTrigger(hDevice, JY5320_TriggerMode::JY5320_StartTrigger)!= 0)
    {
        errorMsg = "cardID:" + QString::number(cardID) + "_JY5320_AI_SendSoftTrigger: failed!";
        qDebug() << errorMsg;
        handleError();
        return false;
    }
    
    updateStatus(PXIe5320Status::Running);
    emit StateChanged("设备正在运行", 1);
    return true;
}

void PXIe5320::DeviceClose()
{
    std::lock_guard<std::mutex> lock(mtx);
    if(!isStarted && m_status == PXIe5320Status::Closed) return;
    paused = true;
    isStarted = false;
    
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
    
    // 更新状态为关闭
    updateStatus(PXIe5320Status::Closed);
    emit StateChanged("设备已关闭", 0);
}

void PXIe5320::FetchData()
{
    if(!isStarted) return;
    
    {
        std::lock_guard<std::mutex> lock(mtx);
        if(shouldExit) return;
    }

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
        return;
    }

    emit signalAcquisitionData(collectdata, serial_number);

    if(continueAcquisition) {
        TotalSamplesToAcq += SampleRate;
        for(auto &collec : collectdata)
        {
            collec.data.clear();
        }
        return;
    }
    
    QMetaObject::invokeMethod(this, "DeviceClose", Qt::QueuedConnection);
    emit CompleteAcquisition();

    delete[] pDataBuf;
    pDataBuf = nullptr;
}

void PXIe5320::handleError()
{
    emit StateChanged(errorMsg, -1);
    if(pDataBuf)
    {
        delete[] pDataBuf;
        pDataBuf = nullptr;
    }
    SamplesToAcq = 0;
    availableSamples = 0;
    transferedSamples = 0;
    
    // 发生错误时更新状态为关闭
    updateStatus(PXIe5320Status::Closed);
}

void PXIe5320::InterruptAcquisition()
{
    // 线程安全地设置退出标志
    std::lock_guard<std::mutex> lock(mtx);
    shouldExit = true;
}

void PXIe5320::updateStatus(PXIe5320Status status)
{
    if (m_status != status) {
        m_status = status;
        
        // 根据状态发送不同的信号
        switch (status) {
            case PXIe5320Status::Closed:
                emit StateChanged("设备已关闭", 0);
                break;
            case PXIe5320Status::Ready:
                emit StateChanged("设备已准备", 1);
                break;
            case PXIe5320Status::Running:
                emit StateChanged("设备运行中", 2);
                break;
        }
    }
}
