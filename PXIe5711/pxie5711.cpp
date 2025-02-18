#include "pxie5711.h"
#include <QPainter>

PXIe5711::PXIe5711(QObject *parent) 
    : QObject(parent),
      waveforms(32, nullptr),
      SampleRate(1000000) {       
    // this->setWindowTitle("PXIe5711");
    // this->setWindowIcon(QIcon(":/images/icon.png"));
}

void PXIe5711::CloseDevice()
{
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
}

PXIe5711::~PXIe5711() {
    // 确保不重复删除
    if(!isStarted) return;
    if (pDataBuf) {
        delete[] pDataBuf;
        pDataBuf = nullptr;
        cout << "pDataBuf deleted" << endl;
    }

    // 删除 waveforms 中的每个指针
    for(auto &waveform : waveforms){
        if(waveform){
            delete waveform;
            waveform = nullptr;
        }
    }
    cout << "waveforms deleted" << endl;

    // 关闭设备
    CloseDevice();
    cout << "PXIe5711 closed" << endl;
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

void PXIe5711::SendSoftTrigger()
{
    if(!isStarted) return;
    JY5710_AO_SendSoftTrigger(hDevice);
    emit WaveformGenerated();
}

void PXIe5711::createwaveform(PXIe5711Waveform waveform)
{
    if(!isStarted) return;
    delete waveforms[waveform.channel];
    waveforms[waveform.channel] = nullptr;

    if (waveform.waveform_type == "SineWave") {
        waveforms[waveform.channel] = new(std::nothrow) SineWave(waveform.amplitude, waveform.frequency);
    } else if (waveform.waveform_type == "SquareWave") {
        waveforms[waveform.channel] = new(std::nothrow) SquareWave(waveform.amplitude, waveform.frequency, waveform.dutyCycle);
    } else if (waveform.waveform_type == "TriangleWave") {
        waveforms[waveform.channel] = new(std::nothrow) TriangleWave(waveform.amplitude, waveform.frequency);
    } else if (waveform.waveform_type == "StepWave") {
        waveforms[waveform.channel] = new(std::nothrow) StepWave(waveform.amplitude);
    } else if (waveform.waveform_type == "HighLevelWave") {
        waveforms[waveform.channel] = new(std::nothrow) HighLevelWave(waveform.amplitude);
    } else if (waveform.waveform_type == "LowLevelWave") {
        waveforms[waveform.channel] = new(std::nothrow) LowLevelWave(waveform.amplitude);
    }

    if(!waveforms[waveform.channel]){
        qDebug() << "创建波形失败，通道：" << waveform.channel << "类型：" << waveform.waveform_type;
    }
}

void PXIe5711::WaveformGenerate()
{
    if(!isStarted) return;
    for(int j = 0; j < pChannelsIndex.size(); j++)
    {
        if(!waveforms[pChannelsIndex[j]]){
            qDebug() << "波形未初始化，通道：" << pChannelsIndex[j];
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
    qDebug() << "生成波形：" << waveforms5711[0].channel << waveforms5711[0].waveform_type << waveforms5711[0].amplitude << waveforms5711[0].frequency << waveforms5711[0].dutyCycle;
    channelCount = waveforms5711.size();
    pChannels = new(std::nothrow) unsigned char[channelCount];
    if (!pChannels) {
        qDebug() << "通道内存分配失败!";
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
            qDebug() << errorMsg;
            emit StateChanged(errorMsg, -1);
            return false;
        }
        isStarted = false;
    }
    qDebug() << "停止AO成功!";
    if(!InitDevice(errorMsg)){
        qDebug() << errorMsg;
        emit StateChanged(errorMsg, -1);
        InterruptAcquisition();
        return false;
    }
    qDebug() << "初始化AO成功!";
    for(auto &waveform : waveforms5711)
    {
        createwaveform(waveform);
    }
    qDebug() << "生成波形成功!";
    WaveformGenerate();
    
    if(JY5710_AO_WriteData(hDevice, this->pDataBuf, channelCount * SampleRate, -1, &actualWriteSamples) != 0)
    {
        errorMsg = "JY5710_AO_WriteData: 数据写入失败!";
        qDebug() << errorMsg;
        emit StateChanged(errorMsg, -1);
        return false;
    }
    qDebug() << "数据写入成功!";
    if(JY5710_AO_Start(hDevice) != 0)
    {
        errorMsg = "JY5710_AO_Start: 启动AO失败!";
        qDebug() << errorMsg;
        emit StateChanged(errorMsg, -1);
        return false;
    }
    // SendSoftTrigger();
    qDebug() << "启动AO成功!";
    emit DeviceReady();
}

void PXIe5711::InterruptAcquisition()
{
    emit WaveformGenerated();
}
