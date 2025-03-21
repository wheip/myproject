#ifndef DEVICEID_H
#define DEVICEID_H

#include <QObject>

extern int current_device_id;
class DeviceId : public QObject
{
    Q_OBJECT
public:
    static DeviceId& instance() {
        static DeviceId instance;
        return instance;
    }

    int getDeviceId() const {
        return current_device_id;
    }

    void setDeviceId(const int& device_id) {
        if (current_device_id != device_id) {
            current_device_id = device_id;
            emit deviceChanged(device_id);
        }
    }

signals:
    void deviceChanged(const int& device_id);

protected:
    DeviceId() {} // 构造函数设为protected
    DeviceId(const DeviceId&) = delete;
    DeviceId& operator=(const DeviceId&) = delete;

private:
    int current_device_id;
};

#define gDeviceId DeviceId::instance()

#endif // DEVICEID_H 