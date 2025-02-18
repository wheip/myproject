#ifndef DATABASE_H
#define DATABASE_H
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <vector>
#include <QJsonDocument>
#include <QJsonValue>
#include <QVariant>
#include <ClassList.h>
#include <string>
#include <QObject>
#include <QMutex>

class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(const QString& connectionName, QWidget *parent = nullptr);
    ~Database();


    bool get_table_name(const QString& condition, std::vector<QString>& TableNames);
    bool delete_table(const QString& tableName);

    bool insert_device(const Device& device, QString& ErrorMessage);
    bool get_device(const QString& condition, std::vector<Device>& devices, bool includeImage = false);
    bool update_device(const Device& device, QString& ErrorMessage, bool includeImage = false);
    bool delete_device(const QString& id, QString& ErrorMessage);

    bool insert_deviceelement(const QString& device_id, const std::vector<Label>& labels, QString& ErrorMessage);
    bool get_deviceelement(const QString& device_id, const QString& condition, std::vector<Label>& labels, QString& ErrorMessage);
    bool update_deviceelement(const QString& device_id, const std::vector<Label>& labels, QString& ErrorMessage);
    bool delete_deviceelement(const QString& device_id, const std::vector<int>& ids, QString& ErrorMessage);
    bool delete_all_deviceelement(const QString& device_id, QString& ErrorMessage);
    
    bool insert_testtask(const QString& tableName, const TestTask& testtask, QString& ErrorMessage);
    bool update_testtask(const QString& tableName, const TestTask& testtask);
    bool get_testtask(const QString& tableName, const QString& condition, std::vector<TestTask>& testtasks, QString& ErrorMessage);
    bool delete_testtask(const QString& tableName, const QString& id, QString& ErrorMessage);
    bool delete_TestDate(const QString& tableName, QString& ErrorMessage);

    bool insert_step(const QString& tableName, const Step& step);
    bool get_step(const QString& tableName, const QString& condition, std::vector<Step>& steps);
    bool update_step(const QString& tableName, const Step& step);
    bool delete_step(const QString& tableName, const QString& id, QString& ErrorMessage);

    bool insert_pxie5711waveform(const QString& tableName, const PXIe5711Waveform& waveform);
    bool get_pxie5711waveform(const QString& tableName, const QString& condition, std::vector<PXIe5711Waveform>& waveforms);
    bool update_pxie5711waveform(const QString& tableName, const PXIe5711Waveform& waveform);
    bool delete_pxie5711waveform(const QString& tableName, const QString& id);

    bool insert_pxie5320waveform(const QString& tableName, const PXIe5320Waveform& waveform);
    bool get_pxie5320waveform(const QString& tableName, const QString& condition, std::vector<PXIe5320Waveform>& waveforms);
    bool update_pxie5320waveform(const QString& tableName, const PXIe5320Waveform& waveform);
    bool concat_pxie5320waveform(const QString& tableName, const PXIe5320Waveform& waveform);
    bool delete_pxie5320waveform(const QString& tableName, const QString& id);

    bool insert_8902data(const QString& tableName, const Data8902& data);
    bool get_8902data(const QString& tableName, const QString& condition, std::vector<Data8902>& datas);
    bool delete_8902data(const QString& tableName, const QString& id);
    bool update_8902data(const QString& tableName, const Data8902& data);

    bool insert_image(const QString& tableName, const Image& image, QString& ErrorMessage);
    bool get_image(const QString& tableName, const QString& condition, std::vector<Image>& images, QString& ErrorMessage);
    bool update_image(const QString& tableName, const Image& image);
    bool delete_image(const QString& tableName, const QString& id);

    bool Build_5320data_table(const QString& TableName);
    bool insert_5320data(const QString& sqlFilePath);
    bool get_5320data(const QString& tableName, const QString& condition, std::vector<Data5320>& datas, QString& ErrorMessage);
    bool update_5320data(const Data5320& data);
    bool loadCSVFile(const QString& filePath, const QString& tableName);

    bool disconnect();
    bool connect();
    bool DISABLE_KEYS(const QString& tableName);
    bool ENABLE_KEYS(const QString& tableName);
    bool insert_5320data_batch(const std::vector<Data5320>& datas);
    QSqlError lastError;
    QString connectionName;

private:

    QSqlDatabase db;
    QWidget *parent;
    QSqlQuery query;
};
#endif // DATABASE_H
