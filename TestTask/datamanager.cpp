#include "datamanager.h"

DataManager::DataManager(QObject *parent) : QObject(parent) {}

DataManager::~DataManager()
{
    unmapFile();
}

void DataManager::setDataPath(const QString& path) {
    dataPath = path;
}

bool DataManager::loadFromBinaryFile(const QString& filePath)
{
    unmapFile();  // 确保之前的映射被清理
    setDataPath(filePath);

#ifdef Q_OS_WIN
        // 打开文件
        fileHandle = CreateFileW(dataPath.toStdWString().c_str(),
                                 GENERIC_READ,
                                 FILE_SHARE_READ,
                                 NULL,
                                 OPEN_EXISTING,
                                 FILE_ATTRIBUTE_NORMAL,
                                 NULL);

        if (fileHandle == INVALID_HANDLE_VALUE) {
            return false;
        }

        // 创建文件映射
        LARGE_INTEGER fileSize;
        if (!GetFileSizeEx(fileHandle, &fileSize)) {
            CloseHandle(fileHandle);
            return false;
        }
        mappedSize = fileSize.QuadPart;

        mappingHandle = CreateFileMappingW(fileHandle,
                                           NULL,
                                           PAGE_READONLY,
                                           0,
                                           0,
                                           NULL);
        if (!mappingHandle) {
            CloseHandle(fileHandle);
            return false;
        }

        // 映射视图
        mappedData = MapViewOfFile(mappingHandle,
                                   FILE_MAP_READ,
                                   0,
                                   0,
                                   0);
        if (!mappedData) {
            CloseHandle(mappingHandle);
            CloseHandle(fileHandle);
            return false;
        }
#else
        mappedFile.setFileName(dataPath);
        if (!mappedFile.open(QIODevice::ReadOnly)) {
            return false;
        }

        mappedSize = mappedFile.size();
        mappedData = mappedFile.map(0, mappedSize);
        if (!mappedData) {
            mappedFile.close();
            return false;
        }
#endif
        return true;
    }

DataManager::DataBlock DataManager::loadDataInRange(double minX, double maxX)
{
    if (!mappedData) {
    return DataBlock();
    }

    DataBlock result;
    const float* rawData = reinterpret_cast<const float*>(mappedData);
    int numPoints = mappedSize / sizeof(float);
    qDebug() << "numPoints:" << numPoints;

    // 计算数据范围对应的索引
    int startIdx = (std::max)(0, static_cast<int>(minX));
    int endIdx = (std::min)(numPoints, static_cast<int>(maxX) + 1);

    // 优化采样策略
    int pointsInRange = endIdx - startIdx;
    int skipFactor = 1;
    const int TARGET_POINTS = 1000000; // 降低目标点数以提高性能

    // if (pointsInRange > TARGET_POINTS) {
    //     skipFactor = pointsInRange / TARGET_POINTS;
    //     // 确保skipFactor是2的幂，这样可以更快地进行采样
    //     skipFactor = 1 << static_cast<int>(std::log2(skipFactor));
    // }

    // 预分配空间
    int estimatedPoints = (pointsInRange / skipFactor) * 2;  // *2 是为了最坏情况
    result.x.reserve(estimatedPoints);
    result.y.reserve(estimatedPoints);

    result.minY = DBL_MAX;
    result.maxY = -DBL_MAX;

    if (skipFactor > 1) {
        // 优化的最小-最大采样方法
        float blockMin, blockMax;
        for (int i = startIdx; i < endIdx; i += skipFactor) {
            blockMin = blockMax = rawData[i];
            const int blockEnd = (std::min)(i + skipFactor, endIdx);

            // 使用展开循环来提高性能
            for (int j = i + 1; j < blockEnd - 3; j += 4) {
                blockMin = (std::min)(blockMin, (std::min)(
                                                    (std::min)(rawData[j], rawData[j + 1]),
                                                    (std::min)(rawData[j + 2], rawData[j + 3])
                                                    ));
                blockMax = (std::max)(blockMax, (std::max)(
                                                    (std::max)(rawData[j], rawData[j + 1]),
                                                    (std::max)(rawData[j + 2], rawData[j + 3])
                                                    ));
            }

            // 处理剩余的点
            for (int j = blockEnd - (blockEnd - i) % 4; j < blockEnd; ++j) {
                blockMin = (std::min)(blockMin, rawData[j]);
                blockMax = (std::max)(blockMax, rawData[j]);
            }

            // 只在值不同时才添加两个点
            if (blockMin != blockMax) {
                result.x.append(i);
                result.y.append(blockMin);
                result.x.append(i);
                result.y.append(blockMax);
            } else {
                result.x.append(i);
                result.y.append(blockMin);
            }

            result.minY = (std::min)(result.minY, static_cast<double>(blockMin));
            result.maxY = (std::max)(result.maxY, static_cast<double>(blockMax));
        }
    } else {
        // 直接复制数据，使用批量操作
        const int BATCH_SIZE = 1024;
        for (int i = startIdx; i < endIdx; i += BATCH_SIZE) {
            const int batchEnd = (std::min)(i + BATCH_SIZE, endIdx);
            for (int j = i; j < batchEnd; ++j) {
                double y = rawData[j];
                result.x.append(j);
                result.y.append(y);
                result.minY = (std::min)(result.minY, y);
                result.maxY = (std::max)(result.maxY, y);
            }
        }
    }

    result.minX = startIdx;
    result.maxX = endIdx - 1;

    return result;
}

int DataManager::getTotalSize()
{
    if (!mappedData) {
        return 0;
    }
    return mappedSize / sizeof(float);
}
