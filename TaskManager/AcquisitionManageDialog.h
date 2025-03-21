#ifndef ACQUISITIONMANAGEDIALOG_H
#define ACQUISITIONMANAGEDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QPushButton>
#include <QComboBox>
#include <QList>
#include "ClassList.h"
#include <memory>
#include "database.h"
#include "ConnectionLocationDialog.h"

class AcquisitionManageDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AcquisitionManageDialog(const int& stepId, const int& deviceId, QWidget *parent = nullptr);

private slots:
    void addAcquisition();
    void deleteAcquisition();
    void saveAcquisitions();
    void loadAcquisitions();
    void accept() override;
    void showConnectionLocationDialog(int row, int column);

private:
    int m_stepId;
    int m_deviceId;  // 存储设备ID
    QTableWidget *tableWidget;
    QComboBox *portComboBox;      // 端口选择下拉框
    QComboBox *inputTypeComboBox; // 输入类型选择下拉框
    QComboBox *testTypeComboBox;  // 测试类型下拉框（用于万用表）
    QPushButton *addButton;
    QPushButton *deleteButton;
    QList<PXIe5320Waveform> m_acquisitions;
    QList<Data8902> m_data8902List;
    std::shared_ptr<Database> db;

    // 新增成员变量
    QImage deviceImage;
    std::vector<Label> deviceLabels;

    // 常量定义
    static const int DEVICE_5322 = 5322;  // 数字量输入设备
    static const int DEVICE_5323 = 5323;  // 模拟量输入设备
    static const int DEVICE_8902 = 8902;  // 万用表设备
    static const QString DIGITAL_INPUT;    // 数字量输入
    static const QString ANALOG_INPUT;     // 模拟量输入
    static const QString MULTIMETER_INPUT; // 万用表输入
    static const QStringList TEST_TYPES;   // 测试类型列表

    void setupUI();
    void updateTable();
    void connectDatabase();
    void updatePortComboBox();    // 更新端口下拉框
    void updateTestTypeComboBox(); // 更新测试类型下拉框
    bool isPortUsed(int port) const;  // 检查端口是否已被使用
    QString getPortTypeName(int port, int device);  // 获取端口类型名称
    bool isValidPortForDevice(int port, int device) const;  // 检查端口是否对设备有效
    int getDeviceFromInputType(const QString& inputType) const;  // 从输入类型获取设备号
    void load8902Data();          // 加载万用表数据
    void save8902Data();          // 保存万用表数据
};

#endif // ACQUISITIONMANAGEDIALOG_H 
