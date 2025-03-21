#include "pxie8902.h"
#include <QThread>
#include <QEventLoop>
#include <memory>
#include <QDebug>

PXIe8902::PXIe8902(QObject *parent) :
    QObject(parent),
    SampleRate(50),
    AcqData(1)
{
    // 初始状态为关闭
    updateStatus(PXIe8902Status::Closed);
}

PXIe8902::~PXIe8902() {
    // 确保线程安全退出
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
        if(JY8902_SetDeviceStatusLed(hDevice, false, 0) != 0)
        {
            errorMsg = "JY8902_SetDeviceStatusLed: set failed!";
        }
        if(JY8902_DMM_Stop(hDevice) != 0)
        {
            errorMsg = "JY8902_DMM_Stop: failed!";
        }

        if(JY8902_Close(hDevice) != 0)
        {
            errorMsg = "JY8902_Close: failed!";
        }
        hDevice = nullptr;
        updateStatus(PXIe8902Status::Closed);
    }
}

bool PXIe8902::SelfTest()
{
    {std::lock_guard<std::mutex> lock(mtx);
    if(m_status != PXIe8902Status::Closed) return true;}
    if(JY8902_Open(0, &hDevice) != 0)
    {
        errorMsg = "JY8902_Open: 打开设备失败!";
        return false;
    }
    JY8902_DMM_Stop(hDevice);
    JY8902_Close(hDevice);
    return true;
}
bool PXIe8902::InitDevice(QString& errorMsg) {

    JY8902_DMM_2_Wire_ResistanceRange ResistanceRange = static_cast<JY8902_DMM_2_Wire_ResistanceRange>(0);

    double sampleInterval = 0.02;

    if(JY8902_Open(0, &hDevice) != 0)
    {
        errorMsg = "JY8902_Open: Open device failed!";
        return false;
    }
    if(JY8902_DMM_SetSampleMode(hDevice, JY8902_DMM_SampleMode::JY8902_ContinuousMultiPoint) != 0)
    {
        errorMsg = "JY8902_DMM_SetSampleMode: set failed!";
        return false;
    }
    if(JY8902_DMM_PowerLineFrequency(hDevice, JY8902_DMM_PowerFrequency::JY8902_50_Hz) != 0) // trigger type
    {
        errorMsg = "JY8902_DMM_PowerLineFrequency: set failed!";
        return false;
    }

    if (testtype == PXIe8902_testtype::PXIe8902_current)
    {
        JY8902_DMM_MeasurementFunction measurementFunction = signaltype ? JY8902_DMM_MeasurementFunction::JY8902_DC_Current : JY8902_DMM_MeasurementFunction::JY8902_AC_Current;
        if (JY8902_DMM_SetMeasurementFunction(hDevice, measurementFunction) != 0)
        {
            errorMsg = "JY8902_DMM_SetMeasurementFunction: set failed!";
            return false;
        }
        if(signaltype) {
            JY8902_DMM_DC_CurrentRange currentRange = static_cast<JY8902_DMM_DC_CurrentRange>(0);
            if (JY8902_DMM_SetDCCurrent(hDevice, currentRange) != 0)
            {
                errorMsg = "JY8902_DMM_SetDCCurrent: set failed!";
                return false;
            }
        } else {
            JY8902_DMM_AC_CurrentRange currentRange = static_cast<JY8902_DMM_AC_CurrentRange>(0);
            if (JY8902_DMM_SetACCurrent(hDevice, currentRange) != 0)
            {
                errorMsg = "JY8902_DMM_SetACCurrent: set failed!";
                return false;
            }
        }
    } else if (testtype == PXIe8902_testtype::PXIe8902_voltage)
    {
        JY8902_DMM_MeasurementFunction measurementFunction = signaltype ? JY8902_DMM_MeasurementFunction::JY8902_DC_Volts : JY8902_DMM_MeasurementFunction::JY8902_AC_Volts;
        if (JY8902_DMM_SetMeasurementFunction(hDevice, measurementFunction) != 0)
        {
            errorMsg = "JY8902_DMM_SetMeasurementFunction: set failed!";
            return false;
        }
        if(signaltype) {
            JY8902_DMM_DC_VoltRange voltRange = static_cast<JY8902_DMM_DC_VoltRange>(0);
            if (JY8902_DMM_SetDCVolt(hDevice, voltRange) != 0)
            {
                errorMsg = "JY8902_DMM_SetDCVolt: set failed!";
                return false;
            }
        } else {
            JY8902_DMM_AC_VoltRange voltRange = static_cast<JY8902_DMM_AC_VoltRange>(0);
            if (JY8902_DMM_SetACVolt(hDevice, voltRange) != 0)
            {
                errorMsg = "JY8902_DMM_SetACVolt: set failed!";
                return false;
            }
        }
    }
    else if (testtype == PXIe8902_testtype::PXIe8902_resistance)
    {
        if(JY8902_DMM_SetMeasurementFunction(hDevice, JY8902_DMM_MeasurementFunction::JY8902_2_Wire_Resistance) != 0)
        {
            errorMsg = "JY8902_DMM_SetMeasurementFunction: set failed!";
            return false;
        }
        if(JY8902_DMM_Set2WireResistance(hDevice, ResistanceRange) != 0)
        {
            errorMsg = "JY8902_DMM_Set2WireResistance: set failed!";
            return false;
        }
    }

    if (JY8902_DMM_SetApertureUnit(hDevice, JY8902_DMM_ApetureUint::JY8902_Second) != 0)
    {
        errorMsg = "JY8902_DMM_SetApertureUnit: set failed!";
        return false;
    }
    if(JY8902_DMM_SetApertureTime(hDevice, sampleInterval) != 0)
    {
        errorMsg = "JY8902_DMM_SetApertureTime: set failed!";
        return false;
    }

    if (JY8902_DMM_SetMultiSample(hDevice, 20, JY8902_DMM_SampleTrigger::JY8902_Sample_Immediately, sampleInterval) != 0)
    {
        errorMsg = "JY8902_DMM_SetMultiSample: set failed!";
        return false;
    }

    if (JY8902_DMM_DisableCalibration(hDevice, true) != 0)
    {
        errorMsg = "JY8902_DMM_DisableCalibration: start AI failed!";
        return false;
    }

    if (JY8902_DMM_SetTriggerDelay(hDevice, 0.1) != 0)
    {
        errorMsg = "JY8902_DMM_SetTriggerDelay: start AI failed!";
        return false;
    }
    if( JY8902_DMM_SetTriggerType(hDevice, JY8902_DMM_TriggerType::JY8902_Soft) != 0)
    {
        errorMsg ="JY8902_DMM_SetTriggerType: start AI failed!";
        return false;
    }
    if(JY8902_SetDeviceStatusLed(hDevice, true, 10) != 0)
    {
        errorMsg ="JY8902_SetDeviceStatusLed: start AI failed!";
        return false;
    }
    if(JY8902_DMM_Start(hDevice) != 0)
    {
        errorMsg ="JY8902_DMM_Start: start AI failed!";
        return false;
    }

    pDataBuf = new double[samplesToAcq];
    return true;
}

bool PXIe8902::StartAcquisition(std::vector<Data8902> collectdata, double collecttime) {
    // 如果设备正在运行，先关闭
    if (m_status != PXIe8902Status::Closed) {
        DeviceClose();
    }
    
    testtype = collectdata[0].test_type;
    this->TotalSamplesToAcq = static_cast<int>(SampleRate * collecttime);
    this->samplesToAcq = (SampleRate * collecttime) > SampleRate ? SampleRate : (SampleRate * collecttime);
    serial_number = 0;
    signaltype = collectdata[0].model;
    
    if(!InitDevice(errorMsg)){
        handleError();
        return false;
    }
    
    PXIe5320Waveform waveform;
    waveform.id = collectdata[0].id;
    waveform.step_id = collectdata[0].step_id;
    waveform.device = 8902;
    if(testtype == PXIe8902_testtype::PXIe8902_voltage) {
        waveform.port = -1;
    } else if(testtype == PXIe8902_testtype::PXIe8902_current) {
        waveform.port = -2;
    } else if(testtype == PXIe8902_testtype::PXIe8902_resistance) {
        waveform.port = -3;
    }
    waveform.data = {};
    this->collectdata.clear();
    this->collectdata.push_back(waveform);
    
    // 更新状态为准备
    updateStatus(PXIe8902Status::Ready);
    emit DeviceReady();
    
    return true;
}

bool PXIe8902::SendSoftTrigger() {
    // 只有在准备状态才能触发
    if (m_status != PXIe8902Status::Ready || isStarted) {
        errorMsg = "设备未准备好，无法触发";
        handleError();
        return false;
    }
    
    {
        std::lock_guard<std::mutex> lock(mtx);
        paused = false;
        isStarted = true;
    }
    
    if(fetchThread == nullptr) {
        fetchThread = new QThread();
        QObject* worker = new QObject();
        worker->moveToThread(fetchThread);
        connect(fetchThread, &QThread::started, worker, [this, worker]() {
            while (true) {
                std::unique_lock<std::mutex> lock(mtx);
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
        {
            std::lock_guard<std::mutex> lock(mtx);
            paused = false;
        }
        cv.notify_all();
    }
    
    if(JY8902_DMM_SendSoftTrigger(hDevice)!= 0)
    {
        errorMsg ="JY8902_DMM_SendSoftTrigger:failed!";
        handleError();
        return false;
    }
    
    updateStatus(PXIe8902Status::Running);
    return true;
}

void PXIe8902::DeviceClose() {
    std::lock_guard<std::mutex> lock(mtx);
    if (!isStarted && m_status == PXIe8902Status::Closed) return;
    
    isStarted = false;
    paused = true;
    
    if (hDevice) {
        if (JY8902_SetDeviceStatusLed(hDevice, false, 0) != 0) {
            errorMsg = "JY8902_SetDeviceStatusLed: set failed!";
        }
        if (JY8902_DMM_Stop(hDevice) != 0) {
            errorMsg = "JY8902_DMM_Stop: failed!";
        }
        if (JY8902_Close(hDevice) != 0) {
            errorMsg = "JY8902_Close: failed!";
        }
        hDevice = NULL;
    }
    
    // 更新状态为关闭
    updateStatus(PXIe8902Status::Closed);
}

void PXIe8902::FetchData() {
    if (!isStarted) return;
    {
        std::lock_guard<std::mutex> lock(mtx);
        // 检查是否应该退出
        if (shouldExit) return;
    }
    unsigned long long availableSamples = 0;
    int actualSample = 0;
    unsigned long long transferedSamples = 0;
    bool overRun = false;

    if(JY8902_DMM_CheckBufferStatus(hDevice, &availableSamples,&transferedSamples, &overRun)!= 0)
    {
        errorMsg = "JY8902_DMM_CheckBufferStatus:failed!";
        handleError();
        return;
    }

    if(availableSamples < samplesToAcq)
    {
        return;
    }

    if(JY8902_DMM_ReadMultiPoint(hDevice, pDataBuf, samplesToAcq, -1, &actualSample)!= 0)
    {
        errorMsg = "JY8902_DMM_ReadMultiPoint:failed!";
        handleError();
        return;
    }

    for(int i = 0; i < samplesToAcq; i++)
    {
        AcqData[0].push_back(pDataBuf[i]);
    }

    this->SampleRemain = TotalSamplesToAcq - transferedSamples - samplesToAcq;
    samplesToAcq = SampleRemain > SampleRate ? SampleRate : SampleRemain;

    collectdata[0].data.insert(collectdata[0].data.end(), AcqData[0].begin(), AcqData[0].end());
    AcqData[0].clear();

    if(SampleRemain > 0)
    {
        return;
    }

    emit signalAcquisitionData(collectdata, serial_number);

    if(continueAcquisition) {
        TotalSamplesToAcq += SampleRate;
        collectdata[0].data.clear();
        return;
    }

    QMetaObject::invokeMethod(this, "DeviceClose", Qt::QueuedConnection);
    emit CompleteAcquisition();

    delete[] pDataBuf;
    pDataBuf = nullptr;
}

void PXIe8902::handleError() {
    emit StateChanged(errorMsg, -1);
    if(pDataBuf)
    {
        delete[] pDataBuf;
        pDataBuf = nullptr;
    }
    samplesToAcq = 0;
    SampleRemain = 0;
    serial_number = 0;

    DeviceClose();
}

void PXIe8902::InterruptAcquisition() {
    std::lock_guard<std::mutex> lock(mtx);
    shouldExit = true;
}

void PXIe8902::updateStatus(PXIe8902Status status) {
    if (m_status != status) {
        m_status = status;
        
        // 根据状态发送不同的信号
        switch (status) {
            case PXIe8902Status::Closed:
                emit StateChanged("设备已关闭", 0);
                break;
            case PXIe8902Status::Ready:
                emit StateChanged("设备已准备", 1);
                break;
            case PXIe8902Status::Running:
                emit StateChanged("设备运行中", 2);
                break;
        }
    }
}


