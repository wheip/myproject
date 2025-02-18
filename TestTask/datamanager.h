#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QVector>
#include <QFile>
#include <QDataStream>
#include <memory>
#include <QDebug>
#include <cmath>
#include <algorithm>
#include <limits>
#ifdef Q_OS_WIN
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

class DataManager : public QObject
{
    Q_OBJECT
public:

    struct DataBlock {
        QVector<double> x;
        QVector<double> y;
        double minX, maxX;
        double minY, maxY;
    };

    explicit DataManager(QObject *parent = nullptr);

    ~DataManager();

    void setDataPath(const QString& path);

    bool loadFromBinaryFile(const QString& filePath);

    DataBlock loadDataInRange(double minX, double maxX);

    int getTotalSize();

private:
    QString dataPath;
#ifdef Q_OS_WIN
    HANDLE fileHandle = INVALID_HANDLE_VALUE;
    HANDLE mappingHandle = NULL;
    void* mappedData = nullptr;
#else
    QFile mappedFile;
    uchar* mappedData = nullptr;
#endif
    qint64 mappedSize = 0;

    void unmapFile() {
#ifdef Q_OS_WIN
        if (mappedData) {
            UnmapViewOfFile(mappedData);
            mappedData = nullptr;
        }
        if (mappingHandle) {
            CloseHandle(mappingHandle);
            mappingHandle = NULL;
        }
        if (fileHandle != INVALID_HANDLE_VALUE) {
            CloseHandle(fileHandle);
            fileHandle = INVALID_HANDLE_VALUE;
        }
#else
        if (mappedData) {
            mappedFile.unmap(mappedData);
            mappedData = nullptr;
        }
        if (mappedFile.isOpen()) {
            mappedFile.close();
        }
#endif
        mappedSize = 0;
    }
};

#endif // DATAMANAGER_H
