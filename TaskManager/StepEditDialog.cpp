#include "StepEditDialog.h"
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QComboBox>
#include <QPushButton>
#include <QMap>
#include "WaveformManageDialog.h"
#include "AcquisitionManageDialog.h"
#include "database.h"
#include "deviceid.h"

StepEditDialog::StepEditDialog(const Step &step, const QString& deviceId, QWidget *parent)
    : QDialog(parent),
      m_step(step),
      m_deviceId(deviceId)
{
    // 初始化数据库连接
    db = std::make_shared<Database>("StepEditDialog");
    
    setWindowTitle("步骤属性");
    setMinimumSize(800, 400);
    
    QVBoxLayout *layout = new QVBoxLayout(this);

    tableWidget = new QTableWidget(7, 2, this);
    tableWidget->setHorizontalHeaderLabels(QStringList() << "属性" << "值");
    tableWidget->verticalHeader()->setVisible(false);
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    
    setupTable();
    
    layout->addWidget(tableWidget);
    
    // 添加管理输出端口按钮
    waveformButton = new QPushButton("管理输出端口", this);
    layout->addWidget(waveformButton);
    connect(waveformButton, &QPushButton::clicked, this, &StepEditDialog::manageWaveforms);

    // 添加管理采集端口按钮
    acquisitionButton = new QPushButton("管理采集端口", this);
    layout->addWidget(acquisitionButton);
    connect(acquisitionButton, &QPushButton::clicked, [this]() {
        AcquisitionManageDialog dialog(m_step.id, m_deviceId, this);
        dialog.exec();
    });

    QDialogButtonBox *buttonBox = new QDialogButtonBox(
                                    QDialogButtonBox::Ok | QDialogButtonBox::Cancel, 
                                    Qt::Horizontal, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &StepEditDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &StepEditDialog::reject);
    layout->addWidget(buttonBox);
    
    connect(tableWidget, &QTableWidget::itemChanged,
            this, &StepEditDialog::onItemChanged);
}

Step StepEditDialog::step() const
{
    return m_step;
}

void StepEditDialog::setupTable()
{
    // 设置第一列（属性名称列）不可编辑
    tableWidget->setColumnWidth(0, 150);  // 设置属性列宽度
    
    // 行 0: id (不可编辑)
    QTableWidgetItem *propId = new QTableWidgetItem("id");
    propId->setFlags(propId->flags() & ~Qt::ItemIsEditable);  // 设置为不可编辑
    tableWidget->setItem(0, 0, propId);
    QTableWidgetItem *itemId = new QTableWidgetItem(m_step.id);
    itemId->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    tableWidget->setItem(0, 1, itemId);

    // 行 1: test_task_id (不可编辑)
    QTableWidgetItem *propTestTaskId = new QTableWidgetItem("test_task_id");
    propTestTaskId->setFlags(propTestTaskId->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(1, 0, propTestTaskId);
    QTableWidgetItem *itemTestTaskId = new QTableWidgetItem(m_step.test_task_id);
    itemTestTaskId->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    tableWidget->setItem(1, 1, itemTestTaskId);

    // 行 2: collecttime (可编辑)
    QTableWidgetItem *propCollectTime = new QTableWidgetItem("collecttime (s)");
    propCollectTime->setFlags(propCollectTime->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(2, 0, propCollectTime);
    QTableWidgetItem *itemCollectTime = new QTableWidgetItem(QString::number(m_step.collecttime));
    tableWidget->setItem(2, 1, itemCollectTime);

    // 行 3: step_number (可编辑)
    QTableWidgetItem *propStepNumber = new QTableWidgetItem("step_number");
    propStepNumber->setFlags(propStepNumber->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(3, 0, propStepNumber);
    QTableWidgetItem *itemStepNumber = new QTableWidgetItem(QString::number(m_step.step_number));
    itemStepNumber->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);  // 设置为不可编辑
    tableWidget->setItem(3, 1, itemStepNumber);

    // 行 4: continue_step (使用下拉框)
    QTableWidgetItem *propContinue = new QTableWidgetItem("continue_step");
    propContinue->setFlags(propContinue->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(4, 0, propContinue);
    continueStepCombo = createBooleanComboBox(m_step.continue_step);
    tableWidget->setCellWidget(4, 1, continueStepCombo);

    // 行 5: isthermometry (使用下拉框)
    QTableWidgetItem *propIsThermometry = new QTableWidgetItem("isthermometry");
    propIsThermometry->setFlags(propIsThermometry->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(5, 0, propIsThermometry);
    isThermometryCombo = createBooleanComboBox(m_step.isthermometry);
    tableWidget->setCellWidget(5, 1, isThermometryCombo);

    // 行 6: thermometry_pause_time (可编辑)
    QTableWidgetItem *propThermometryPause = new QTableWidgetItem("thermometry_pause_time (%)");
    propThermometryPause->setFlags(propThermometryPause->flags() & ~Qt::ItemIsEditable);
    tableWidget->setItem(6, 0, propThermometryPause);
    // 添加百分号到显示值
    QTableWidgetItem *itemThermometryPause = new QTableWidgetItem(QString::number(m_step.thermometry_pause_time) + "%");
    tableWidget->setItem(6, 1, itemThermometryPause);

    // 设置属性列的背景色，使其更容易区分
    for(int row = 0; row < tableWidget->rowCount(); ++row) {
        if(QTableWidgetItem *item = tableWidget->item(row, 0)) {
            item->setBackground(QColor(240, 240, 240));
        }
    }

    // 连接下拉框的信号
    connect(continueStepCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &StepEditDialog::onComboBoxChanged);
    connect(isThermometryCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &StepEditDialog::onComboBoxChanged);
}

QComboBox* StepEditDialog::createBooleanComboBox(bool value)
{
    QComboBox* combo = new QComboBox;
    combo->addItem("false", false);
    combo->addItem("true", true);
    combo->setCurrentIndex(value ? 1 : 0);
    return combo;
}

void StepEditDialog::onComboBoxChanged(int)
{
    // 更新 Step 数据
    if (sender() == continueStepCombo) {
        m_step.continue_step = continueStepCombo->currentData().toBool();
        // 通知父窗口更新节点样式
        emit stepDataChanged(m_step);
    } else if (sender() == isThermometryCombo) {
        m_step.isthermometry = isThermometryCombo->currentData().toBool();
    }
}

void StepEditDialog::onItemChanged(QTableWidgetItem *item)
{
    // 仅处理第2列的修改
    if (item->column() != 1)
        return;
    
    int row = item->row();
    QString text = item->text();
    bool ok;
    
    switch (row) {
    case 2: // collecttime
        m_step.collecttime = text.toDouble(&ok);
        if (!ok) {
            QMessageBox::warning(this, "输入错误", "collecttime 必须是数值");
        }
        break;
    case 6: // thermometry_pause_time
        {
            // 移除可能存在的百分号后再转换
            QString cleanText = text;
            cleanText.remove('%');
            cleanText = cleanText.trimmed();
            double value = cleanText.toDouble(&ok);
            if (!ok) {
                QMessageBox::warning(this, "输入错误", "thermometry_pause_time 必须是有效的百分比值");
                // 恢复原值
                item->setText(QString::number(m_step.thermometry_pause_time) + "%");
            } else if (value < 0 || value > 100) {
                QMessageBox::warning(this, "输入错误", "thermometry_pause_time 必须在 0% 到 100% 之间");
                // 恢复原值
                item->setText(QString::number(m_step.thermometry_pause_time) + "%");
            } else {
                // 值有效，更新数据
                m_step.thermometry_pause_time = value;
                // 确保显示带有百分号
                item->setText(QString::number(m_step.thermometry_pause_time) + "%");
            }
        }
        break;
    default:
        break;
    }
}

void StepEditDialog::manageWaveforms()
{
    // 创建并显示输出端口管理对话框
    WaveformManageDialog dialog(m_step.id, m_deviceId, this);
    dialog.setWindowModality(Qt::ApplicationModal);
    dialog.exec();
}

void StepEditDialog::accept()
{
    QDialog::accept();
} 