#ifndef CH340_H
#define CH340_H

#include <QString>
#include <QSerialPort>
#include <QByteArray>
#include <QDebug>


#define g_ch340 CH340::getInstance()

class CH340
{
public:
    static CH340* getInstance(){
        static CH340 instance;
        return &instance;
    }
    CH340(const CH340&) = delete;
    CH340& operator=(const CH340&) = delete;
    ~CH340();

    // 打开串口
    bool openPort(const QString& portName,
                  int baudRate = QSerialPort::Baud9600,
                  QSerialPort::DataBits dataBits = QSerialPort::Data8,
                  QSerialPort::Parity parity = QSerialPort::NoParity,
                  QSerialPort::StopBits stopBits = QSerialPort::OneStop);

    // 关闭串口
    void closePort();

    // 发送数据
    bool writeData(const QByteArray& data);

    // 读取数据
    QByteArray readData();

    // 检查串口是否打开
    bool isOpen() const;

    // 获取错误信息
    QString getLastError() const;

    // 发送字符串数据
    bool writeString(const QString& str);

private:
    CH340();
    QSerialPort* m_serialPort;
    QString m_lastError;
};

#endif // CH340_H
