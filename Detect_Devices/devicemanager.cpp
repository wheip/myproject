#include "devicemanager.h"
#include "ui_devicemanager.h"
#include "database.h"
#include "labelediting.h"
#include "labelmanagerdialog.h"
#include "createdevicedialog.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTableWidgetItem>
#include <QDebug>
#include <QMessageBox>
#include <vector>

DeviceManager::DeviceManager(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DeviceManager)
{
    ui->setupUi(this);
    
    this->close();
    // 设置表格属性
    setupTableWidget();
    
    connect(ui->pdsearch, &QPushButton::clicked, this, &DeviceManager::onPdSearchClicked);
    connect(ui->pbadddevice, &QPushButton::clicked, this, &DeviceManager::onPbAddDeviceClicked);
}

DeviceManager::~DeviceManager()
{
    delete ui;
}

void DeviceManager::setupTableWidget()
{
    // 允许用户调整列宽
    ui->tableWidget_devices->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    
    // 设置表格自动拉伸填满可用空间
    ui->tableWidget_devices->horizontalHeader()->setStretchLastSection(true);
    
    // 设置默认列宽比例
    int totalWidth = ui->tableWidget_devices->width();
    ui->tableWidget_devices->setColumnWidth(0, totalWidth * 0.25); // ID列
    ui->tableWidget_devices->setColumnWidth(1, totalWidth * 0.35); // 名称列
    ui->tableWidget_devices->setColumnWidth(2, totalWidth * 0.2);  // 图片列
    ui->tableWidget_devices->setColumnWidth(3, totalWidth * 0.2);  // 操作列
    
    // 设置表格为只读模式
    ui->tableWidget_devices->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    // 设置表格的选择模式
    ui->tableWidget_devices->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_devices->setSelectionMode(QAbstractItemView::SingleSelection);
    
    // 设置表格的其他属性
    ui->tableWidget_devices->setAlternatingRowColors(true);  // 交替行颜色
    ui->tableWidget_devices->setShowGrid(true);              // 显示网格线
    ui->tableWidget_devices->setGridStyle(Qt::SolidLine);   // 实线网格
    
    // 设置表头样式
    QHeaderView* header = ui->tableWidget_devices->horizontalHeader();
    header->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    header->setHighlightSections(false);  // 点击表头时不高亮
    
    // 设置表格的最小高度
    ui->tableWidget_devices->setMinimumHeight(300);
}

// 在窗口大小改变时调整表格大小
void DeviceManager::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    
    // 如果表格还没有数据，保持默认列宽比例
    if (ui->tableWidget_devices->rowCount() == 0) {
        int totalWidth = ui->tableWidget_devices->width();
        ui->tableWidget_devices->setColumnWidth(0, totalWidth * 0.25);
        ui->tableWidget_devices->setColumnWidth(1, totalWidth * 0.35);
        ui->tableWidget_devices->setColumnWidth(2, totalWidth * 0.2);
        // 最后一列会自动拉伸，不需要设置
    }
}

void DeviceManager::onPdSearchClicked()
{
    QString idText = ui->lineEdit->text().trimmed();
    QString nameText = ui->lineEdit_name->text().trimmed();

    QString condition;
    if (!idText.isEmpty()) {
        condition = "id = '" + idText + "'";
    } else if (!nameText.isEmpty()) {
        condition = "device_name = '" + nameText + "'";
    } else {
        condition = "";
    }

    Database db("searchdeviceConnection", this);
    std::vector<Device> devices;
    if (!db.get_device(condition, devices, true)) {
        qDebug() << "查询设备失败：" << condition;
        return;
    }

    ui->tableWidget_devices->clearContents();
    ui->tableWidget_devices->setRowCount(0);
    int row = 0;
    ui->tableWidget_devices->setRowCount(static_cast<int>(devices.size()));
    for (const auto& dev : devices) {
        ui->tableWidget_devices->setItem(row, 0, new QTableWidgetItem(QString::number(dev.id)));
        QString deviceName = QString::fromUtf8(dev.device_name);
        ui->tableWidget_devices->setItem(row, 1, new QTableWidgetItem(deviceName));
        QString imageStatus = dev.image.isEmpty() ? "无图片" : "有图片";
        ui->tableWidget_devices->setItem(row, 2, new QTableWidgetItem(imageStatus));
        
        QPushButton *btnDelete = new QPushButton("删除");
        ui->tableWidget_devices->setCellWidget(row, 3, btnDelete);
        int deviceId = dev.id;
        connect(btnDelete, &QPushButton::clicked, this, [this, deviceId](){
            Database db("deleteConnection", this);
            QString errorMessage;
            if(!db.delete_device(deviceId, errorMessage)) {
                QMessageBox::warning(this, "错误", "删除设备失败：" + errorMessage);
            } else {
                onPdSearchClicked();
            }
        });
        row++;
    }
}

void DeviceManager::on_tableWidget_devices_cellDoubleClicked(int row, int column)
{
    if(column != 2)
        return;
    
    QTableWidgetItem *idItem = ui->tableWidget_devices->item(row, 0);
    if(!idItem)
        return;
    QString deviceId = idItem->text();

    Database db("checkdeviceConnection", this);
    std::vector<Device> devices;
    QString condition = "id = '" + deviceId + "'";
    if(!db.get_device(condition, devices, true)) {
        qDebug() << "查询设备失败: " << condition;
        return;
    }

    if(devices.empty()){
        qDebug() << "未找到设备: " << deviceId;
        return;
    }

    Device device = devices[0];
    if(device.image.isEmpty()){
        qDebug() << "设备没有图片: " << deviceId;
        return;
    }

    QImage image;
    if(!image.loadFromData(device.image)) {
        qDebug() << "载入图片失败: " << deviceId;
        return;
    }

    std::vector<Label> labelInfoAdd;
    LabelManagerDialog dlg(this, image, deviceId.toInt(), labelInfoAdd);
    if(dlg.exec() == QDialog::Accepted){
         LabelChanges changes = dlg.getLabelChanges();
         QString ErrorMessage;
         if(!db.update_deviceelement(deviceId.toInt(), changes.updatedLabels, ErrorMessage)){
            QMessageBox::warning(this, "错误", "更新设备标签失败: " + ErrorMessage);
         }
         if(!db.insert_deviceelement(deviceId.toInt(), changes.insertedLabels, ErrorMessage)){
            QMessageBox::warning(this, "错误", "插入设备标签失败: " + ErrorMessage);
         }
         if(!db.delete_deviceelement(deviceId.toInt(), changes.deletedIds, ErrorMessage)){
            QMessageBox::warning(this, "错误", "删除设备标签失败: " + ErrorMessage);
         }
    }
}

void DeviceManager::onPbAddDeviceClicked()
{
    int deviceId = -1;
    CreateDeviceDialog createDeviceDialog(this);
    if(createDeviceDialog.exec() == QDialog::Accepted) {
        Device device = createDeviceDialog.getDevice();
        std::vector<Label> deviceLabels = createDeviceDialog.getDeviceLabels();
        Database db("adddeviceConnection", this);
        QString errorMessage;
        if(!db.insert_device(device, errorMessage)) {
            QMessageBox::warning(this, "错误", "添加设备失败：" + errorMessage);
        } else {
            QList<QMap<QString, QVariant>> resId = db.selectQuery("SELECT LAST_INSERT_ID() as id");
            QMessageBox::information(this, "成功", "设备添加成功");
            
            // 清空当前表格
            ui->tableWidget_devices->clearContents();
            ui->tableWidget_devices->setRowCount(0);
            
            // 只显示新添加的设备
            ui->tableWidget_devices->setRowCount(1);
            ui->tableWidget_devices->setItem(0, 0, new QTableWidgetItem(QString::number(resId[0]["id"].toInt())));
            ui->tableWidget_devices->setItem(0, 1, new QTableWidgetItem(QString::fromUtf8(device.device_name)));
            ui->tableWidget_devices->setItem(0, 2, new QTableWidgetItem(device.image.isEmpty() ? "无图片" : "有图片"));
            
            // 添加删除按钮
            QPushButton *btnDelete = new QPushButton("删除");
            ui->tableWidget_devices->setCellWidget(0, 3, btnDelete);
            deviceId = resId[0]["id"].toInt();
            connect(btnDelete, &QPushButton::clicked, this, [this, deviceId](){
                Database db("deleteConnection", this);
                QString errorMessage;
                if(!db.delete_device(deviceId, errorMessage)) {
                    QMessageBox::warning(this, "错误", "删除设备失败：" + errorMessage);
                } else {
                    onPdSearchClicked();
                }
            });
        }
        
        if(!db.insert_deviceelement(deviceId, deviceLabels, errorMessage)){
            QMessageBox::warning(this, "错误", "添加设备标签失败：" + errorMessage);
        }
    }
}
