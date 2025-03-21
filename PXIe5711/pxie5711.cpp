#include "pxie5711.h"
#include <QPainter>

PXIe5711::PXIe5711(QObject *parent) 
    : QObject(parent),
      waveforms(32, nullptr),
      SampleRate(1000000) {
    // 初始状态为关闭
    updateStatus(PXIe5711Status::Closed);
}

void PXIe5711::CloseDevice()
{
    if(m_status == PXIe5711Status::Closed) return;
    
    std::lock_guard<std::mutex> lock(mtx);
    
    if(isStarted == true)
    {
        if(hDevice != NULL)
        {
            JY5710_SetDeviceStatusLed(hDevice, false, 0);
            JY5710_AO_Stop(hDevice);
            JY5710_Close(hDevice);
            hDevice = NULL;
            isStarted = false;
        }
    }
    
    // 更新状态为关闭
    updateStatus(PXIe5711Status::Closed);
}

PXIe5711::~PXIe5711() {
    // 确保不重复删除
    if(!isStarted) return;
    if (pDataBuf) {
        delete[] pDataBuf;
        pDataBuf = nullptr;
    }

    // 删除 waveforms 中的每个指针
    for(auto &waveform : waveforms){
        if(waveform){
            delete waveform;
            waveform = nullptr;
        }
    }

    // 关闭设备
    CloseDevice();
}

bool PXIe5711::SelfTest()
{
    {std::lock_guard<std::mutex> lock(mtx);
    if(m_status != PXIe5711Status::Closed) return true;}
    if(JY5710_Open(2, &hDevice) != 0)
    {
        m_errorMessage = "JY5710_Open: 打开设备失败!";
        return false;
    }
    JY5710_AO_Stop(hDevice);
    JY5710_Close(hDevice);
    return true;
}
bool PXIe5711::InitDevice(QString& errorMsg)
{
    double actualUpdateRate = 0;
    double* pRangeLow = new(std::nothrow) double[channelCount];
    double* pRangeHi = new(std::nothrow) double[channelCount];

    pDataBuf = new(std::nothrow) double[channelCount * SampleRate];
    if (!pDataBuf) {
        errorMsg = "数据缓冲区内存分配失败!";
        return false;
    }

    if(!pChannels || !pRangeLow || !pRangeHi){
        errorMsg = "通道、低范围、高范围内存分配失败!";
        delete[] pChannels;
        delete[] pRangeLow;
        delete[] pRangeHi;
        return false;
    }

    for(int i = 0; i < channelCount; i++)
    {
        pRangeHi[i] = this->highRange;
        pRangeLow[i] = this->lowRange;
    }

    if(JY5710_Open(2, &hDevice) != 0)
    {
        errorMsg = "JY5710_Open: 打开设备失败!";
        delete[] pChannels;
        delete[] pRangeLow;
        delete[] pRangeHi;
        return false;
    }
    if(JY5710_AO_EnableChannel(hDevice, channelCount, pChannels, pRangeLow, pRangeHi) != 0)
    {
        errorMsg = "JY5710_AO_EnableChannel: 设置失败!";
        JY5710_AO_Stop(hDevice);
        JY5710_Close(hDevice);
        hDevice = nullptr;
        delete[] pChannels;
        delete[] pRangeLow;
        delete[] pRangeHi;
        return false;
    }
    if(JY5710_AO_SetMode(hDevice, JY5710_AO_UpdateMode::JY5710_AO_ContinuousWrapping) != 0)
    {
        errorMsg = "JY5710_AO_SetMode: 设置失败!";
        JY5710_AO_Stop(hDevice);
        JY5710_Close(hDevice);
        hDevice = nullptr;
        delete[] pChannels;
        delete[] pRangeLow;
        delete[] pRangeHi;
        return false;
    }
    if(JY5710_AO_SetUpdateRate(hDevice, SampleRate, &actualUpdateRate) != 0)
    {
        errorMsg = "JY5710_AO_SetUpdateRate: 设置失败!";
        JY5710_AO_Stop(hDevice);
        JY5710_Close(hDevice);
        hDevice = nullptr;
        delete[] pChannels;
        delete[] pRangeLow;
        delete[] pRangeHi;
        return false;
    }
    if(JY5710_SetDeviceStatusLed(hDevice, true, 10) != 0)
    {
        errorMsg = "JY5710_SetDeviceStatusLed: set failed!";
        JY5710_AO_Stop(hDevice);
        JY5710_Close(hDevice);
        hDevice = nullptr;
        delete[] pChannels;
        delete[] pRangeLow;
        delete[] pRangeHi;
        return false;
    }
    if(JY5710_AO_SetStartTriggerType(hDevice, JY5710_AO_TriggerType::JY5710_AO_Soft) != 0) //software start trigger type
    {
        errorMsg = "JY5710_AO_SetStartTriggerType: set failed!";
        JY5710_AO_Stop(hDevice);
        JY5710_Close(hDevice);
        hDevice = nullptr;
        delete[] pChannels;
        delete[] pRangeLow;
        delete[] pRangeHi;
        return false;
    }

    delete[] pChannels;
    delete[] pRangeLow;
    delete[] pRangeHi;

    isStarted = true;
    return true;
}

bool PXIe5711::SendSoftTrigger()
{
    // 只有在准备状态才能触发
    if (m_status != PXIe5711Status::Ready || !isStarted) {
        QString errorMsg = "设备未准备好，无法触发";
        emit StateChanged(errorMsg, -1);
        return false;
    }
    
    JY5710_AO_SendSoftTrigger(hDevice);
    
    // 更新状态为运行
    updateStatus(PXIe5711Status::Running);
    
    emit WaveformGenerated();
    return true;
}

bool PXIe5711::InitializeDevice()
{
    if(m_status != PXIe5711Status::Closed) CloseDevice();
    std::vector<PXIe5711Waveform> waveforms5711;
    for(int i = 0; i < 32; i++)
    {
        PXIe5711Waveform waveform;
        waveform.channel = i;
        waveform.waveform_type = PXIe5711_testtype::HighLevelWave;
        waveform.amplitude = 0;
        waveform.frequency = 0;
        waveform.dutyCycle = 0;
        waveforms5711.push_back(waveform);
    }
    if(!receivewaveform(waveforms5711)) {
        return false;
    }
    SendSoftTrigger();
    CloseDevice();
}

void PXIe5711::createwaveform(PXIe5711Waveform waveform)
{
    if(!isStarted) return;
    delete waveforms[waveform.channel];
    waveforms[waveform.channel] = nullptr;

    if (waveform.waveform_type == PXIe5711_testtype::SineWave) {
        waveforms[waveform.channel] = new(std::nothrow) SineWave(waveform.amplitude, waveform.frequency);
    } else if (waveform.waveform_type == PXIe5711_testtype::SquareWave) {
        waveforms[waveform.channel] = new(std::nothrow) SquareWave(waveform.amplitude, waveform.frequency, waveform.dutyCycle);
    } else if (waveform.waveform_type == PXIe5711_testtype::TriangleWave) {
        waveforms[waveform.channel] = new(std::nothrow) TriangleWave(waveform.amplitude, waveform.frequency);
    } else if (waveform.waveform_type == PXIe5711_testtype::StepWave) {
        waveforms[waveform.channel] = new(std::nothrow) StepWave(waveform.amplitude);
    } else if (waveform.waveform_type == PXIe5711_testtype::HighLevelWave) {
        waveforms[waveform.channel] = new(std::nothrow) HighLevelWave(waveform.amplitude);
    } else if (waveform.waveform_type == PXIe5711_testtype::LowLevelWave) {
        waveforms[waveform.channel] = new(std::nothrow) LowLevelWave(waveform.amplitude);
    }

    if(!waveforms[waveform.channel]){
        qDebug() << "创建波形失败，通道：" << waveform.channel;
    }
}

void PXIe5711::WaveformGenerate()
{
    if(!isStarted) return;
    for(int j = 0; j < pChannelsIndex.size(); j++)
    {
        if(!waveforms[pChannelsIndex[j]]){
            continue;
        }
        for(int i = 0; i < SampleRate; i++)
        {
            this->pDataBuf[i * channelCount + j] = waveforms[pChannelsIndex[j]]->generate(i, SampleRate);
        }
    }
}

bool PXIe5711::receivewaveform(std::vector<PXIe5711Waveform> waveforms5711)
{
    {std::lock_guard<std::mutex> lock(mtx);
    if (m_status != PXIe5711Status::Closed) {
        CloseDevice();
    }}
    
    channelCount = waveforms5711.size();
    pChannels = new(std::nothrow) unsigned char[channelCount];
    if (!pChannels) {
        return false;
    }
    for(int i = 0; i < channelCount; i++)
    {
        pChannels[i] = waveforms5711[i].channel;
        pChannelsIndex.push_back(waveforms5711[i].channel);
    }
    QString errorMsg;

    if(isStarted)
    {
        if(JY5710_AO_Stop(hDevice) != 0)
        {
            errorMsg = "JY5710_AO_Stop: 停止AO失败!";
            emit StateChanged(errorMsg, -1);
            return false;
        }
        isStarted = false;
    }
    if(!InitDevice(errorMsg)){
        m_errorMessage = errorMsg;
        return false;
    }
    for(auto &waveform : waveforms5711)
    {
        createwaveform(waveform);
    }
    WaveformGenerate();
    
    if(JY5710_AO_WriteData(hDevice, this->pDataBuf, channelCount * SampleRate, -1, &actualWriteSamples) != 0)
    {
        errorMsg = "JY5710_AO_WriteData: 数据写入失败!";
        emit StateChanged(errorMsg, -1);
        return false;
    }
    if(JY5710_AO_Start(hDevice) != 0)
    {
        errorMsg = "JY5710_AO_Start: 启动AO失败!";
        emit StateChanged(errorMsg, -1);
        return false;
    }
    
    // 更新状态为准备
    updateStatus(PXIe5711Status::Ready);
    emit DeviceReady();
    
    return true;
}

void PXIe5711::updateStatus(PXIe5711Status status)
{
    if (m_status != status) {
        m_status = status;
        
        // 根据状态发送不同的信号
        switch (status) {
            case PXIe5711Status::Closed:
                emit StateChanged("设备已关闭", 0);
                break;
            case PXIe5711Status::Ready:
                emit StateChanged("设备已准备", 1);
                break;
            case PXIe5711Status::Running:
                emit StateChanged("设备运行中", 2);
                break;
        }
    }
}
