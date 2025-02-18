#include "ch340.h"
#include <QSerialPortInfo>

CH340::CH340() : m_serialPort(new QSerialPort())
{
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        if (info.description() == "USB-SERIAL CH340") {
            if(openPort(info.portName(), QSerialPort::Baud9600, QSerialPort::Data8, QSerialPort::NoParity, QSerialPort::OneStop)){
                qDebug() << "CH340设备打开成功";
            }else{
                qDebug() << "CH340设备打开失败：" << getLastError();
            }
            break;
        }
    }
}

CH340::~CH340()
{
    if (m_serialPort->isOpen()) {
        m_serialPort->close();
    }
    delete m_serialPort;
}

bool CH340::openPort(const QString& portName, 
                    int baudRate,
                    QSerialPort::DataBits dataBits,
                    QSerialPort::Parity parity,
                    QSerialPort::StopBits stopBits)
{
    if (m_serialPort->isOpen()) {
        m_serialPort->close();
    }

    m_serialPort->setPortName(portName);
    m_serialPort->setBaudRate(baudRate);
    m_serialPort->setDataBits(dataBits);
    m_serialPort->setParity(parity);
    m_serialPort->setStopBits(stopBits);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);

    if (!m_serialPort->open(QIODevice::ReadWrite)) {
        m_lastError = m_serialPort->errorString();
        return false;
    }
    return true;
}

void CH340::closePort()
{
    if (m_serialPort->isOpen()) {
        m_serialPort->close();
    }
}

bool CH340::writeData(const QByteArray& data)
{
    if (!m_serialPort->isOpen()) {
        m_lastError = "串口未打开";
        return false;
    }

    qint64 bytesWritten = m_serialPort->write(data);
    if (bytesWritten == -1) {
        m_lastError = m_serialPort->errorString();
        return false;
    }
    
    // 等待数据写入完成
    if (!m_serialPort->waitForBytesWritten(1000)) {
        m_lastError = "数据写入超时";
        return false;
    }
    
    return true;
}

QByteArray CH340::readData()
{
    if (!m_serialPort->isOpen()) {
        m_lastError = "串口未打开";
        return QByteArray();
    }

    if (!m_serialPort->waitForReadyRead(100)) {
        return QByteArray();
    }
    
    return m_serialPort->readAll();
}

bool CH340::isOpen() const
{
    return m_serialPort->isOpen();
}

QString CH340::getLastError() const
{
    return m_lastError;
}

bool CH340::writeString(const QString& str)
{
    if (!m_serialPort->isOpen()) {
        m_lastError = "串口未打开";
        return false;
    }

    QByteArray data = str.toUtf8();
    
    qint64 bytesWritten = m_serialPort->write(data);
    if (bytesWritten == -1) {
        m_lastError = m_serialPort->errorString();
        return false;
    }
    
    // 等待数据写入完成
    if (!m_serialPort->waitForBytesWritten(1000)) {
        m_lastError = "数据写入超时";
        return false;
    }
    
    return true;
}
