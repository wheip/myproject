#include "AcquisitionManageDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QUuid>
#include <QMessageBox>
#include <QLabel>
#include <QSizePolicy>
#include <QPushButton>
#include <QDebug>
#include <QComboBox>
#include "deviceid.h"
#include "PCB_Components_Detect/labelediting.h"

// 定义静态常量
const QString AcquisitionManageDialog::DIGITAL_INPUT = "数字量输入";
const QString AcquisitionManageDialog::ANALOG_INPUT = "模拟量输入";
const QString AcquisitionManageDialog::MULTIMETER_INPUT = "万用表";
const QStringList AcquisitionManageDialog::TEST_TYPES = {"DC voltage", "DC current", "DC resistance", "AC voltage", "AC current", "AC resistance"};

AcquisitionManageDialog::AcquisitionManageDialog(const int& stepId, const int& deviceId, QWidget *parent)
    : QDialog(parent)
    , m_stepId(stepId)
    , m_deviceId(deviceId)
{
    connectDatabase();
    
    // 加载设备图像和标签
    std::vector<Device> devices;
    if (db->get_device("id = '" + QString::number(deviceId) + "'", devices, true) && !devices.empty()) {
        deviceImage = QImage::fromData(devices[0].image);
    }
    
    QString errorMsg;
    db->get_deviceelement(deviceId, "", deviceLabels, errorMsg);
    
    setupUI();
    loadAcquisitions();
    load8902Data();
}

void AcquisitionManageDialog::connectDatabase()
{
    db = std::make_shared<Database>("AcquisitionManageDialog");
}

void AcquisitionManageDialog::setupUI()
{
    setWindowTitle("管理采集端口");
    setMinimumSize(800, 600);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 创建顶部工具栏布局（包含下拉框和操作按钮）
    QHBoxLayout *selectionLayout = new QHBoxLayout;
    
    // 输入类型选择
    inputTypeComboBox = new QComboBox(this);
    inputTypeComboBox->addItem(DIGITAL_INPUT);
    inputTypeComboBox->addItem(ANALOG_INPUT);
    inputTypeComboBox->addItem(MULTIMETER_INPUT);
    selectionLayout->addWidget(new QLabel("输入类型:", this));
    selectionLayout->addWidget(inputTypeComboBox);
    
    // 端口选择
    portComboBox = new QComboBox(this);
    selectionLayout->addWidget(new QLabel("端口:", this));
    selectionLayout->addWidget(portComboBox);
    
    // 测试类型选择（用于万用表）
    testTypeComboBox = new QComboBox(this);
    testTypeComboBox->addItems(TEST_TYPES);
    testTypeComboBox->hide();  // 默认隐藏
    selectionLayout->addWidget(new QLabel("测试类型:", this));
    selectionLayout->addWidget(testTypeComboBox);
    
    // 添加一些间距
    selectionLayout->addSpacing(20);
    
    // 添加和删除按钮
    addButton = new QPushButton("添加", this);
    deleteButton = new QPushButton("删除", this);
    selectionLayout->addWidget(addButton);
    selectionLayout->addWidget(deleteButton);
    selectionLayout->addStretch();
    
    mainLayout->addLayout(selectionLayout);

    // 创建表格
    tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(5);
    tableWidget->setHorizontalHeaderLabels({"ID", "设备", "端口/类型", "正极连接位置", "负极连接位置"});
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    // 修改表格双击事件处理
    connect(tableWidget, &QTableWidget::itemDoubleClicked, this, [this](QTableWidgetItem *item) {
        // 只有正负极连接位置列（第3和第4列）在双击时打开选择对话框
        if (item->column() == 3 || item->column() == 4) {
            showConnectionLocationDialog(item->row(), item->column());
        }
    });
    
    mainLayout->addWidget(tableWidget);

    // 创建底部按钮布局（确定和取消按钮）
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    QPushButton *okButton = new QPushButton("确定", this);
    QPushButton *cancelButton = new QPushButton("取消", this);

    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);

    // 连接信号和槽
    connect(inputTypeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AcquisitionManageDialog::updatePortComboBox);
    connect(addButton, &QPushButton::clicked, this, &AcquisitionManageDialog::addAcquisition);
    connect(deleteButton, &QPushButton::clicked, this, &AcquisitionManageDialog::deleteAcquisition);
    connect(okButton, &QPushButton::clicked, this, &AcquisitionManageDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &AcquisitionManageDialog::reject);

    // 初始化端口下拉框
    updatePortComboBox();
}

void AcquisitionManageDialog::loadAcquisitions()
{
    QString tableName = QString::number(m_deviceId) + "$$PXIe5320";
    std::vector<PXIe5320Waveform> acquisitions;
    
    if (db->get_pxie5320waveform(tableName, "step_id = '" + QString::number(m_stepId) + "'", acquisitions)) {
        m_acquisitions = QList<PXIe5320Waveform>(acquisitions.begin(), acquisitions.end());
        updateTable();
    }
}

void AcquisitionManageDialog::saveAcquisitions()
{
    QString tableName = QString::number(m_deviceId) + "$$PXIe5320";
    
    // 保存当前的采集端口记录
    for (const auto& acquisition : m_acquisitions) {
        if(!db->update_pxie5320waveform(tableName, acquisition))
        {
            if (!db->insert_pxie5320waveform(tableName, acquisition)) {
                QMessageBox::warning(this, "错误", "保存采集端口失败");
                return;
            }
        }
    }
}

void AcquisitionManageDialog::updatePortComboBox()
{
    portComboBox->clear();
    
    if (inputTypeComboBox->currentText() == MULTIMETER_INPUT) {
        portComboBox->hide();
        testTypeComboBox->show();
        return;
    }
    
    portComboBox->show();
    testTypeComboBox->hide();
    
    int currentDevice = getDeviceFromInputType(inputTypeComboBox->currentText());
    int startPort = 0;
    int endPort = (currentDevice == DEVICE_5322) ? 15 : 31;
    
    for (int port = startPort; port <= endPort; ++port) {
        if (!isPortUsed(port) && isValidPortForDevice(port, currentDevice)) {
            QString portName = QString("%1 %2").arg(inputTypeComboBox->currentText()).arg(port);
            portComboBox->addItem(portName, port);
        }
    }
    
    addButton->setEnabled(portComboBox->count() > 0);
}

bool AcquisitionManageDialog::isPortUsed(int port) const
{
    int currentDevice = getDeviceFromInputType(inputTypeComboBox->currentText());
    for (const auto& acquisition : m_acquisitions) {
        if (acquisition.device == currentDevice && acquisition.port == port) {
            return true;
        }
    }
    return false;
}

int AcquisitionManageDialog::getDeviceFromInputType(const QString& inputType) const
{
    if (inputType == DIGITAL_INPUT) return DEVICE_5322;
    if (inputType == ANALOG_INPUT) return DEVICE_5323;
    if (inputType == MULTIMETER_INPUT) return DEVICE_8902;
    return -1;
}

bool AcquisitionManageDialog::isValidPortForDevice(int port, int device) const
{
    if (device == DEVICE_5322) {
        // 数字量输入设备 (5322) 的有效端口范围是 0-15
        return port >= 0 && port <= 15;
    } else if (device == DEVICE_5323) {
        // 模拟量输入设备 (5323) 的有效端口范围是 0-31
        return port >= 0 && port <= 31;
    }
    return false;
}

void AcquisitionManageDialog::addAcquisition()
{
    if (inputTypeComboBox->currentText() == MULTIMETER_INPUT) {
        // 检查是否已经存在万用表数据
        if (!m_data8902List.isEmpty()) {
            QMessageBox::warning(this, "警告", "每个步骤只能使用一种万用表测量类型");
            return;
        }

        PXIe8902_testtype testType = static_cast<PXIe8902_testtype>((testTypeComboBox->currentIndex()) % 3);

        Data8902 newData;
        newData.step_id = m_stepId;
        newData.test_type = testType;
        newData.model = testTypeComboBox->currentText().contains("DC");
        newData.positive_connect_location = 0;
        newData.negative_connect_location = 0;
        
        m_data8902List.append(newData);
        updateTable();
        return;
    }

    if (portComboBox->count() == 0) {
        QMessageBox::warning(this, "警告", "没有可用的端口");
        return;
    }

    PXIe5320Waveform newAcquisition;
    newAcquisition.step_id = m_stepId;
    newAcquisition.device = getDeviceFromInputType(inputTypeComboBox->currentText());
    newAcquisition.port = portComboBox->currentData().toInt();
    newAcquisition.positive_connect_location = 0;
    newAcquisition.negative_connect_location = 0;
    
    m_acquisitions.append(newAcquisition);
    updateTable();
    updatePortComboBox();  // 更新可用端口列表
}

void AcquisitionManageDialog::deleteAcquisition()
{
    QList<QTableWidgetItem*> selectedItems = tableWidget->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择要删除的项");
        return;
    }

    int row = selectedItems.first()->row();
    
    // 判断是否是万用表数据
    if (row >= m_acquisitions.size()) {
        // 删除万用表数据
        int data8902Index = row - m_acquisitions.size();
        int id = m_data8902List[data8902Index].id;
        QString tableName = QString::number(m_deviceId) + "$$PXIe8902";
        if (db->delete_8902data(tableName, id)) {
            m_data8902List.removeAt(data8902Index);
        } else {
            QMessageBox::warning(this, "错误", "删除万用表数据失败");
            return;
        }
    } else {
        // 删除 PXIe5320 数据
        int id = m_acquisitions[row].id;
        QString tableName = QString::number(m_deviceId) + "$$PXIe5320";
        m_acquisitions.removeAt(row);
        if (!db->delete_pxie5320waveform(tableName, id)) {
            QMessageBox::warning(this, "错误", "删除采集端口失败");
            return;
        }
    }
    
    updateTable();
    updatePortComboBox();  // 更新可用端口列表
}

void AcquisitionManageDialog::updateTable()
{
    int totalRows = m_acquisitions.size() + m_data8902List.size();
    tableWidget->setRowCount(totalRows);
    
    int row = 0;
    
    // 修改添加 PXIe5320 数据的部分
    for (const auto& acquisition : m_acquisitions) {
        QTableWidgetItem *idItem = new QTableWidgetItem(QString::number(acquisition.id));
        QTableWidgetItem *deviceItem = new QTableWidgetItem(QString::number(acquisition.device));
        QTableWidgetItem *portItem = new QTableWidgetItem(QString::number(acquisition.port));
        QTableWidgetItem *positiveItem = new QTableWidgetItem(QString::number(acquisition.positive_connect_location));
        QTableWidgetItem *negativeItem = new QTableWidgetItem(QString::number(acquisition.negative_connect_location));
        
        // 设置 ID、设备和端口为不可编辑
        idItem->setFlags(idItem->flags() & ~Qt::ItemIsEditable);
        deviceItem->setFlags(deviceItem->flags() & ~Qt::ItemIsEditable);
        portItem->setFlags(portItem->flags() & ~Qt::ItemIsEditable);
        
        // 正负极连接位置初始设置为不可编辑
        positiveItem->setFlags(positiveItem->flags() & ~Qt::ItemIsEditable);
        negativeItem->setFlags(negativeItem->flags() & ~Qt::ItemIsEditable);
        
        tableWidget->setItem(row, 0, idItem);
        tableWidget->setItem(row, 1, deviceItem);
        tableWidget->setItem(row, 2, portItem);
        tableWidget->setItem(row, 3, positiveItem);
        tableWidget->setItem(row, 4, negativeItem);
        row++;
    }
    
    // 修改添加 8902 数据的部分
    for (const auto& data : m_data8902List) {
        QTableWidgetItem *idItem = new QTableWidgetItem(QString::number(data.id));
        QTableWidgetItem *deviceItem = new QTableWidgetItem(QString::number(DEVICE_8902));
        QTableWidgetItem *typeItem = new QTableWidgetItem(data.model ? "DC " + PXIe8902_testtype_to_string(data.test_type) : "AC " + PXIe8902_testtype_to_string(data.test_type));
        QTableWidgetItem *positiveItem = new QTableWidgetItem(QString::number(data.positive_connect_location));
        QTableWidgetItem *negativeItem = new QTableWidgetItem(QString::number(data.negative_connect_location));
        
        // 设置 ID、设备和类型为不可编辑
        idItem->setFlags(idItem->flags() & ~Qt::ItemIsEditable);
        deviceItem->setFlags(deviceItem->flags() & ~Qt::ItemIsEditable);
        typeItem->setFlags(typeItem->flags() & ~Qt::ItemIsEditable);
        
        // 正负极连接位置初始设置为不可编辑
        positiveItem->setFlags(positiveItem->flags() & ~Qt::ItemIsEditable);
        negativeItem->setFlags(negativeItem->flags() & ~Qt::ItemIsEditable);
        
        tableWidget->setItem(row, 0, idItem);
        tableWidget->setItem(row, 1, deviceItem);
        tableWidget->setItem(row, 2, typeItem);
        tableWidget->setItem(row, 3, positiveItem);
        tableWidget->setItem(row, 4, negativeItem);
        row++;
    }
}

void AcquisitionManageDialog::load8902Data()
{
    QString tableName = QString::number(m_deviceId) + "$$PXIe8902";
    QString condition = "step_id = '" + QString::number(m_stepId) + "'";
    
    std::vector<Data8902> data8902Vector;
    if (db->get_8902data(tableName, condition, data8902Vector)) {
        m_data8902List = QList<Data8902>(data8902Vector.begin(), data8902Vector.end());
        updateTable();
    }
}

void AcquisitionManageDialog::save8902Data()
{
    QString tableName = QString::number(m_deviceId) + "$$PXIe8902";
    
    for (const auto& data : m_data8902List) {
        if (!db->update_8902data(tableName, data)) {
            if (!db->insert_8902data(tableName, data)) {
                QMessageBox::warning(this, "错误", "保存万用表数据失败");
                return;
            }
        }
    }
}

void AcquisitionManageDialog::accept()
{
    // 保存采集端口信息到数据库
    saveAcquisitions();
    save8902Data();
    QDialog::accept();
}

void AcquisitionManageDialog::showConnectionLocationDialog(int row, int column)
{
    if (deviceImage.isNull()) {
        QMessageBox::warning(this, "警告", "没有可用的设备图像");
        return;
    }

    ConnectionLocationDialog dialog(deviceImage, deviceLabels, this);
    if (dialog.exec() == QDialog::Accepted) {
        int selectedId = dialog.getSelectedLabelId();
        if (selectedId != -1) {
            // 更新表格中的值
            tableWidget->item(row, column)->setText(QString::number(selectedId));
            
            // 更新对应的数据
            QString idStr = tableWidget->item(row, 0)->text();
            
            // 检查是否是万用表数据(万用表总是添加在最后)
            if (row >= m_acquisitions.size()) {
                // 更新万用表数据
                int data8902Index = row - m_acquisitions.size();
                if (data8902Index < m_data8902List.size()) {
                    if (column == 3) {
                        m_data8902List[data8902Index].positive_connect_location = selectedId;
                    } else if (column == 4) {
                        m_data8902List[data8902Index].negative_connect_location = selectedId;
                    }
                }
            } else {
                // 更新采集端口数据
                if (column == 3) {
                    m_acquisitions[row].positive_connect_location = selectedId;
                } else if (column == 4) {
                    m_acquisitions[row].negative_connect_location = selectedId;
                }
            }
        }
    }
} 
