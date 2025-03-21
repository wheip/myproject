#include "jydevicemanager.h"
#include <QApplication>

JYDeviceManager::JYDeviceManager()
{
    device8902 = new PXIe8902();
    device5711 = new PXIe5711();
    device5322 = new PXIe5320(nullptr, 5322);
    device5323 = new PXIe5320(nullptr, 5323);

    thread8902 = new QThread();
    device8902->moveToThread(thread8902);
    thread8902->start();

    thread5711 = new QThread();
    device5711->moveToThread(thread5711);
    thread5711->start();

    thread5322 = new QThread();
    device5322->moveToThread(thread5322);
    thread5322->start();

    thread5323 = new QThread();
    device5323->moveToThread(thread5323);
    thread5323->start();
}

JYDeviceManager::~JYDeviceManager()
{
    thread8902->quit();
    thread5711->quit();
    thread5322->quit();
    thread5323->quit();
    thread8902->wait();
    thread5711->wait();
    thread5322->wait();

    delete device5711;
    delete device5322;
    delete device5323;
    delete device8902;

    delete thread8902;
    delete thread5711;
    delete thread5322;
    delete thread5323;
}

bool JYDeviceManager::CloseAllDevices()
{
    if(device8902) device8902->DeviceClose();
    if(device5711) { device5711->CloseDevice(); device5711->InitializeDevice(); }
    if(device5322) device5322->DeviceClose();
    if(device5323) device5323->DeviceClose();
    return true;
}

bool JYDeviceManager::SelfTestAllDevices(QString& errorMsg)
{
    bool result = true;
    if(!device8902->SelfTest()) {errorMsg += device8902->getErrorMessage() + "\n"; result = false;}
    if(!device5711->SelfTest()) {errorMsg += device5711->getErrorMessage() + "\n"; result = false;}
    if(!device5322->SelfTest()) {errorMsg += device5322->getErrorMessage() + "\n"; result = false;}
    if(!device5323->SelfTest()) {errorMsg += device5323->getErrorMessage() + "\n"; result = false;}
    return result;
}

JYDeviceManager* JYDeviceManager::getInstance()
{
    // 确保 QApplication 已经存在
    if (!QApplication::instance()) {
        return nullptr;
    }
    
    static JYDeviceManager instance;
    return &instance;
}
