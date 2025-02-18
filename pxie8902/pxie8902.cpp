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

}

PXIe8902::~PXIe8902() {
    std::lock_guard<std::mutex> lock(mtx);
    if(!isStarted) return;
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
            qDebug() << errorMsg;
        }
        if(JY8902_DMM_Stop(hDevice) != 0)
        {
            errorMsg = "JY8902_DMM_Stop: failed!";
            qDebug() << errorMsg;
        }

        if(JY8902_Close(hDevice) != 0)
        {
            errorMsg = "JY8902_Close: failed!";
            qDebug() << errorMsg;
        }
        hDevice = nullptr;
        isStarted = false;
    }
}

bool PXIe8902::InitDevice(QString& errorMsg) {
    JY8902_DMM_AC_CurrentRange currentRange = static_cast<JY8902_DMM_AC_CurrentRange>(0);
    JY8902_DMM_AC_VoltRange voltRange = static_cast<JY8902_DMM_AC_VoltRange>(0);
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

    if (testtype == "current")
    {
        if (JY8902_DMM_SetMeasurementFunction(hDevice, JY8902_DMM_MeasurementFunction::JY8902_AC_Current) != 0)
        {
            errorMsg = "JY8902_DMM_SetMeasurementFunction: set failed!";
            return false;
        }

        if (JY8902_DMM_SetACCurrent(hDevice, currentRange) != 0)
        {
            errorMsg = "JY8902_DMM_SetACCurrent: set failed!";
            return false;
        }
    }else if (testtype == "voltage")
    {
        if (JY8902_DMM_SetMeasurementFunction(hDevice, JY8902_DMM_MeasurementFunction::JY8902_AC_Volts) != 0)
        {
            errorMsg = "JY8902_DMM_SetMeasurementFunction: set failed!";
            return false;
        }
        if(JY8902_DMM_SetACVolt(hDevice, voltRange) != 0)
        {
            errorMsg = "JY8902_DMM_SetACVolt: set failed!";
            return false;
        }
    }
    else if (testtype == "resistance")
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
    qDebug() << "PXIe8902::InitDevice: samplesToAcq: " << samplesToAcq;
    return true;
}

bool PXIe8902::StartAcquisition(std::vector<Data8902> collectdata, double collecttime) {
    testtype = collectdata[0].test_type;
    this->TotalSamplesToAcq = static_cast<int>(SampleRate * collecttime);
    this->samplesToAcq = (SampleRate * collecttime) > SampleRate ? SampleRate : (SampleRate * collecttime);
    serial_number = 0;

    if(!InitDevice(errorMsg)){
        emit StateChanged(errorMsg, -1);
        handleError();
        emit CompleteAcquisition();
        return false;
    }

    PXIe5320Waveform waveform;
    waveform.id = collectdata[0].id;
    waveform.step_id = collectdata[0].step_id;
    waveform.device = 8902;
    waveform.port = -1;
    waveform.data = {};
    this->collectdata.clear();
    this->collectdata.push_back(waveform);

    emit DeviceReady();
}

void PXIe8902::SendSoftTrigger() {
    if(isStarted) return;

    QThread* fetchThread = QThread::create([this]() {
        while (isStarted) {
            this->FetchData();
            QThread::msleep(1); // 相当于10ms的定时器间隔
        }
    });
    connect(fetchThread, &QThread::finished, fetchThread, &QObject::deleteLater);
    fetchThread->start();

    this->isStarted = true;

    if(JY8902_DMM_SendSoftTrigger(hDevice)!= 0)
    {
        errorMsg ="JY8902_DMM_SendSoftTrigger:failed!";
        emit StateChanged(errorMsg, -1);
        handleError();
        return;
    }
    qDebug() << "PXIe8902::SendSoftTrigger: send soft trigger";
}


void PXIe8902::DeviceClose() {
    if(!isStarted) {
        return;
    }
    if(hDevice)
    {
        if(JY8902_SetDeviceStatusLed(hDevice, false, 0) != 0)
        {
            errorMsg = "JY8902_SetDeviceStatusLed: set failed!";
            qDebug() << errorMsg;
        }
        if(JY8902_DMM_Stop(hDevice)!= 0)
        {
            errorMsg ="JY8902_DMM_Stop:failed!";
        }
        if(JY8902_Close(hDevice)!= 0)
        {
            errorMsg ="JY8902_Close:failed!";
        }
        hDevice = NULL;
    }
    isStarted = false;
}

void PXIe8902::FetchData() {
    if(!isStarted) return;
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

    DeviceClose();
    emit CompleteAcquisition();

    delete[] pDataBuf;
    pDataBuf = nullptr;
}


void PXIe8902::handleError() {
    if(pDataBuf)
    {
        delete[] pDataBuf;
        pDataBuf = nullptr;
    }
    samplesToAcq = 0;
    SampleRemain = 0;
    serial_number = 0;

    QMetaObject::invokeMethod(this, [this]() {
        DeviceClose();
        emit CompleteAcquisition();
    }, Qt::QueuedConnection);
}

void PXIe8902::InterruptAcquisition() {
    TotalSamplesToAcq = 0;
    samplesToAcq = 0;
}


