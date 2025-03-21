#ifndef JYDEVICEMANAGER_H
#define JYDEVICEMANAGER_H
#include "pxie5711.h"
#include "pxie5320.h"
#include "pxie8902.h"
#include <QThread>

class JYDeviceManager
{
public:
    static JYDeviceManager* getInstance();

    PXIe8902* getDevice8902() { return device8902; }
    PXIe5711* getDevice5711() { return device5711; }
    PXIe5320* getDevice5322() { return device5322; }
    PXIe5320* getDevice5323() { return device5323; }

    bool CloseAllDevices();

    bool SelfTestAllDevices(QString& errorMsg);

private:
    explicit JYDeviceManager();
    ~JYDeviceManager();

    JYDeviceManager(const JYDeviceManager&) = delete;
    JYDeviceManager& operator=(const JYDeviceManager&) = delete;

    PXIe8902* device8902;
    PXIe5711* device5711;
    PXIe5320* device5322;
    PXIe5320* device5323;

    QThread* thread8902;
    QThread* thread5711;
    QThread* thread5322;
    QThread* thread5323;
};

#endif // JYDEVICEMANAGER_H
