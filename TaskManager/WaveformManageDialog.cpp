#include "WaveformManageDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QUuid>
#include <QMessageBox>
#include <QComboBox>
#include <QLabel>
#include <QSizePolicy>
#include <QDialogButtonBox>
#include <QShowEvent>
#include <QImage>
#include <QVector>
#include <algorithm>

// 定义可用通道列表（1-32）
const QList<int> WaveformManageDialog::AVAILABLE_CHANNELS = [](){
    QList<int> channels;
    for(int i = 0; i <= 31; ++i) {
        channels.append(i);
    }
    return channels;
}();

// 定义静态常量
const QStringList WaveformManageDialog::ANALOG_POWER_TYPES = {"HighLevelWave", "LowLevelWave"};
const QStringList WaveformManageDialog::DIGITAL_TYPES = {"SineWave", "SquareWave", "TriangleWave"};

WaveformManageDialog::WaveformManageDialog(const int& stepId, const int& deviceId, QWidget *parent)
    : QDialog(parent)
    , m_stepId(stepId)
    , m_deviceId(deviceId)
{
    setWindowTitle("输出端口管理");
    setMinimumSize(800, 600);
    
    // 初始化独立的数据库连接
    db = std::make_shared<Database>("WaveformManageDialog");
    
    // 加载设备图像和标签
    std::vector<Device> devices;
    if (db->get_device("id = '" + QString::number(deviceId) + "'", devices, true) && !devices.empty()) {
        deviceImage = QImage::fromData(devices[0].image);
    }
    
    QString errorMsg;
    db->get_deviceelement(deviceId, "", deviceLabels, errorMsg);
    
    setupUI();
}

void WaveformManageDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // 创建顶部工具栏布局
    QHBoxLayout *toolbarLayout = new QHBoxLayout;
    
    // 添加通道选择下拉框
    QLabel *channelLabel = new QLabel("选择通道:", this);
    channelComboBox = new QComboBox(this);
    channelComboBox->setMinimumWidth(150);
    channelComboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    
    addButton = new QPushButton("添加", this);
    deleteButton = new QPushButton("删除", this);
    
    toolbarLayout->addWidget(channelLabel);
    toolbarLayout->addWidget(channelComboBox);
    toolbarLayout->addWidget(addButton);
    toolbarLayout->addWidget(deleteButton);
    toolbarLayout->addStretch();
    
    // 创建表格
    tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(9);
    tableWidget->setHorizontalHeaderLabels(QStringList() 
        << "通道号" << "通道类型" << "波形类型" << "幅值" << "频率" << "占空比" 
        << "正极连接位置" << "负极连接位置" << "ID");
    // 设置表头可见和可调整大小
    tableWidget->horizontalHeader()->setVisible(true);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    
    // 设置默认列宽
    tableWidget->setColumnWidth(0, 80);  // 通道号
    tableWidget->setColumnWidth(1, 100); // 通道类型
    tableWidget->setColumnWidth(2, 100); // 波形类型
    tableWidget->setColumnWidth(3, 80);  // 幅值
    tableWidget->setColumnWidth(4, 80);  // 频率
    tableWidget->setColumnWidth(5, 80);  // 占空比
    tableWidget->setColumnWidth(6, 80);  // 正极连接位置
    tableWidget->setColumnWidth(7, 80);  // 负极连接位置
    tableWidget->setColumnWidth(8, 200); // ID
    
    // 允许表格根据内容自动调整大小
    tableWidget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    
    // 设置表格的选择模式
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    
    tableWidget->verticalHeader()->setVisible(false);
    
    // 修改表格双击事件处理
    connect(tableWidget, &QTableWidget::itemDoubleClicked, this, [this](QTableWidgetItem *item) {
        // 只有正负极连接位置列（第6和第7列）在双击时打开选择对话框
        if (item->column() == 6 || item->column() == 7) {
            showConnectionLocationDialog(item->row(), item->column());
        }
    });
    
    // 创建确认和取消按钮的布局
    QHBoxLayout *dialogButtonLayout = new QHBoxLayout;
    QPushButton *okButton = new QPushButton("确认", this);
    QPushButton *cancelButton = new QPushButton("取消", this);
    connect(okButton, &QPushButton::clicked, this, &WaveformManageDialog::onAccepted);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    
    dialogButtonLayout->addStretch();
    dialogButtonLayout->addWidget(okButton);
    dialogButtonLayout->addWidget(cancelButton);
    
    // 添加到主布局
    mainLayout->addLayout(toolbarLayout);  // 添加工具栏到顶部
    mainLayout->addWidget(tableWidget);
    mainLayout->addLayout(dialogButtonLayout);  // 添加确认和取消按钮到布局底部
    
    // 连接信号
    connect(addButton, &QPushButton::clicked, this, &WaveformManageDialog::addWaveform);
    connect(deleteButton, &QPushButton::clicked, this, &WaveformManageDialog::deleteWaveform);
    connect(tableWidget, &QTableWidget::itemChanged, this, &WaveformManageDialog::onItemChanged);
    
    setupTable();
    updateChannelComboBox();
}

void WaveformManageDialog::setupTable()
{
    tableWidget->setRowCount(m_waveforms.size());
    for (int i = 0; i < m_waveforms.size(); ++i) {
        updateTableRow(i, m_waveforms[i]);
    }
}

void WaveformManageDialog::updateTableRow(int row, const PXIe5711Waveform& waveform)
{
    // 通道号列 - 根据不同范围显示不同的编号
    QString channelDisplay;
    if (waveform.channel >= 0 && waveform.channel <= 15) {
        channelDisplay = QString::number(waveform.channel);  // 模拟量输出 0-15
    } else if (waveform.channel >= 16 && waveform.channel <= 27) {
        channelDisplay = QString::number(waveform.channel - 16);  // 数字量输出 0-11
    } else if (waveform.channel >= 28 && waveform.channel <= 31) {
        channelDisplay = QString::number(waveform.channel - 28);  // 电源输出 0-3
    }
    
    QTableWidgetItem *channelNumItem = new QTableWidgetItem(channelDisplay);
    channelNumItem->setFlags(channelNumItem->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(row, 0, channelNumItem);

    // 通道类型列
    QTableWidgetItem *channelTypeItem = new QTableWidgetItem(getChannelTypeName(waveform.channel));
    channelTypeItem->setFlags(channelTypeItem->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(row, 1, channelTypeItem);

    // 波形类型列（使用下拉框）
    QComboBox *waveformTypeCombo = new QComboBox(tableWidget);
    waveformTypeCombo->addItems(getWaveformTypes(waveform.channel));
    waveformTypeCombo->setCurrentText(PXIe5711_testtype_to_string(waveform.waveform_type));
    connect(waveformTypeCombo, &QComboBox::currentIndexChanged,
            [this, row](const int &index) {
                if (row < m_waveforms.size()) {
                    m_waveforms[row].waveform_type = static_cast<PXIe5711_testtype>(index + static_cast<int>(m_waveforms[row].waveform_type));
                }
            });
    tableWidget->setCellWidget(row, 2, waveformTypeCombo);

    // 其他列
    tableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(waveform.amplitude)));
    tableWidget->setItem(row, 4, new QTableWidgetItem(QString::number(waveform.frequency)));
    tableWidget->setItem(row, 5, new QTableWidgetItem(QString::number(waveform.dutyCycle)));
    
    // 修改正负极连接位置的单元格设置
    QTableWidgetItem *positiveItem = new QTableWidgetItem(QString::number(waveform.positive_connect_location));
    QTableWidgetItem *negativeItem = new QTableWidgetItem(QString::number(waveform.negative_connect_location));
    
    // 初始设置为不可编辑
    positiveItem->setFlags(positiveItem->flags() & ~Qt::ItemIsEditable);
    negativeItem->setFlags(negativeItem->flags() & ~Qt::ItemIsEditable);
    
    tableWidget->setItem(row, 6, positiveItem);
    tableWidget->setItem(row, 7, negativeItem);
    
    // ID列设置为只读
    QTableWidgetItem *idItem = new QTableWidgetItem(waveform.id);
    idItem->setFlags(idItem->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(row, 8, idItem);
}

void WaveformManageDialog::updateChannelComboBox()
{
    channelComboBox->clear();
    // 添加所有未使用的通道
    for (int channel : AVAILABLE_CHANNELS) {
        if (!isChannelUsed(channel)) {
            channelComboBox->addItem(getChannelDisplayName(channel), channel);
        }
    }
    // 如果没有可用通道，禁用添加按钮
    addButton->setEnabled(channelComboBox->count() > 0);
}

bool WaveformManageDialog::isChannelUsed(int channel) const
{
    for (const PXIe5711Waveform& waveform : m_waveforms) {
        if (waveform.channel == channel) {
            return true;
        }
    }
    return false;
}

void WaveformManageDialog::addWaveform()
{
    if (channelComboBox->count() == 0) {
        QMessageBox::warning(this, "警告", "没有可用的通道");
        return;
    }

    PXIe5711Waveform newWaveform;
    int channel = channelComboBox->currentData().toInt();
    newWaveform.step_id = m_stepId;
    newWaveform.channel = channel;
    // 设置默认波形类型
    QStringList types = getWaveformTypes(channel);
    if (!types.isEmpty()) {
        newWaveform.waveform_type = static_cast<PXIe5711_testtype>(types == ANALOG_POWER_TYPES ? 0 : 2);
    }
    
    m_waveforms.append(newWaveform);
    
    int row = tableWidget->rowCount();
    tableWidget->setRowCount(row + 1);
    updateTableRow(row, newWaveform);
    
    // 更新可用通道列表
    updateChannelComboBox();
}

void WaveformManageDialog::deleteWaveform()
{
    QList<QTableWidgetItem*> selectedItems = tableWidget->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择要删除的行");
        return;
    }
    
    int row = selectedItems.first()->row();
    if (row >= 0 && row < m_waveforms.size()) {
        QString tableName = QString::number(m_deviceId) + "$$PXIe5711";
        db->delete_pxie5711waveform(tableName, m_waveforms[row].id);
        for (int col = 0; col < tableWidget->columnCount(); ++col) {
            QWidget *widget = tableWidget->cellWidget(row, col);
            if (widget) {
                tableWidget->removeCellWidget(row, col);
                widget->deleteLater();
            }
        }
        m_waveforms.removeAt(row);
        tableWidget->removeRow(row);
        // 更新可用通道列表
        updateChannelComboBox();
    }
}

void WaveformManageDialog::onItemChanged(QTableWidgetItem *item)
{
    int row = item->row();
    if (row >= m_waveforms.size()) return;
    
    bool ok;
    PXIe5711Waveform &waveform = m_waveforms[row];
    
    switch (item->column()) {
        case 3: // amplitude
            waveform.amplitude = item->text().toDouble(&ok);
            if (!ok || waveform.amplitude < 0 || waveform.amplitude > 10) {
                QMessageBox::warning(this, "错误", "幅值必须是大于或等于0小于或等于10的数值");
                item->setText(QString::number(0));
            }
            break;
        case 4: // frequency
            waveform.frequency = item->text().toDouble(&ok);
            if (!ok || waveform.frequency < 0 || waveform.frequency > 1000000) {
                QMessageBox::warning(this, "错误", "频率必须是大于或等于0小于或等于1000000的数值");
                item->setText(QString::number(0));
            }
            break;
        case 5: // dutyCycle
            waveform.dutyCycle = item->text().toDouble(&ok);
            if (!ok || waveform.dutyCycle < 0 || waveform.dutyCycle > 100) {
                QMessageBox::warning(this, "错误", "占空比必须是大于或等于0小于或等于100的数值");
                item->setText(QString::number(0));
            }
            break;
        case 6: // positive_connect_location
            waveform.positive_connect_location = item->text().toInt(&ok);
            if (!ok) {
                QMessageBox::warning(this, "错误", "正极连接位置必须是整数");
                item->setText(QString::number(waveform.positive_connect_location));
            }
            break;
        case 7: // negative_connect_location
            waveform.negative_connect_location = item->text().toInt(&ok);
            if (!ok) {
                QMessageBox::warning(this, "错误", "负极连接位置必须是整数");
                item->setText(QString::number(waveform.negative_connect_location));
            }
            break;
    }
}

QString WaveformManageDialog::getChannelDisplayName(int channel)
{
    if (channel >= 0 && channel <= 15) {
        return QString("模拟量输出 %1").arg(channel);
    } else if (channel >= 16 && channel <= 27) {
        return QString("数字量输出 %1").arg(channel - 16);
    } else if (channel >= 28 && channel <= 31) {
        return QString("电源输出 %1").arg(channel - 28);
    }
    return QString::number(channel);
}

QString WaveformManageDialog::getChannelTypeName(int channel)
{
    if (channel >= 0 && channel <= 15) {
        return "模拟量输出";
    } else if (channel >= 16 && channel <= 27) {
        return "数字量输出";
    } else if (channel >= 28 && channel <= 31) {
        return "电源输出";
    }
    return "未知类型";
}

QStringList WaveformManageDialog::getWaveformTypes(int channel)
{
    if (channel >= 16 && channel <= 27) {  // 数字量输出
        return DIGITAL_TYPES;
    } else {  // 模拟量输出和电源输出
        return ANALOG_POWER_TYPES;
    }
}

void WaveformManageDialog::onAccepted()
{
    QString tableName = QString::number(m_deviceId) + "$$PXIe5711";
    
    // 保存当前的波形列表
    for (const auto& waveform : m_waveforms) {
        if (!db->update_pxie5711waveform(tableName, waveform)) {
            if (!db->insert_pxie5711waveform(tableName, waveform)) {
                QMessageBox::warning(this, "错误", "保存波形失败");
                return;
            }
        }
    }
    accept();
}

void WaveformManageDialog::showEvent(QShowEvent *event)
{
    // 调用父类实现
    QDialog::showEvent(event);

    // 每次对话框打开时，从数据库中加载对应步骤的端口信息
    QString tableName = QString::number(m_deviceId) + "$$PXIe5711";
    std::vector<PXIe5711Waveform> waveformsFromDb;
    QString errorMsg;
    if (db->get_pxie5711waveform(tableName, "step_id = '" + QString::number(m_stepId) + "'", waveformsFromDb)) {
        // 更新 m_waveforms
        m_waveforms = QList<PXIe5711Waveform>(waveformsFromDb.begin(), waveformsFromDb.end());
    } else {
        // 如果加载失败，可以清空数据或提示错误（此处选择清空）
        m_waveforms.clear();
    }
    
    // 重新刷新表格和可用通道下拉框
    setupTable();
    updateChannelComboBox();
}

// 添加新的处理函数
void WaveformManageDialog::showConnectionLocationDialog(int row, int column)
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
            
            // 更新对应的波形数据
            int channel = tableWidget->item(row, 0)->text().toInt();
            if (channel > 0) {
                auto it = std::find_if(m_waveforms.begin(), m_waveforms.end(),
                    [channel](const PXIe5711Waveform& w) { return w.channel == channel; });
                if (it != m_waveforms.end()) {
                    if (column == 6) {
                        it->positive_connect_location = selectedId;
                    } else if (column == 7) {
                        it->negative_connect_location = selectedId;
                    }
                }
            }
        }
    }
} 
