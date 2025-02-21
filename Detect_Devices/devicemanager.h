#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <QWidget>

namespace Ui {
class DeviceManager;
}

class DeviceManager : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceManager(QWidget *parent = nullptr);
    ~DeviceManager();

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    // 新增槽函数，用于处理 pdsearch 按钮的点击事件
    void onPdSearchClicked();
    // 新增槽函数，用于处理表格单元格双击事件
    void on_tableWidget_devices_cellDoubleClicked(int row, int column);
    // 新增槽函数，用于添加设备
    void onPbAddDeviceClicked();

private:
    Ui::DeviceManager *ui;
    void setupTableWidget();
};

#endif // DEVICEMANAGER_H
