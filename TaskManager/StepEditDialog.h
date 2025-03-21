#ifndef STEPEDITDIALOG_H
#define STEPEDITDIALOG_H

#include <QDialog>
#include "ClassList.h"
#include <QTableWidget>
#include <QComboBox>
#include <QPushButton>
#include "WaveformManageDialog.h"
#include "AcquisitionManageDialog.h"
#include "database.h"
#include <QMap>
#include "deviceid.h"

class StepEditDialog : public QDialog
{
    Q_OBJECT

signals:
    void stepDataChanged(const Step& step);  // 添加信号声明

public:
    explicit StepEditDialog(const Step &step, const int& deviceId, QWidget *parent = nullptr);
    // 返回编辑后的步骤数据
    Step step() const;

private slots:
    void onItemChanged(QTableWidgetItem *item);
    void onComboBoxChanged(int index);
    void manageWaveforms();
    void accept() override;

private:
    int m_deviceId;  // 存储设备ID
    void setupUI();
    void setupTable();

    Step m_step;
    QTableWidget *tableWidget;
    QComboBox *continueStepCombo;
    QComboBox *isThermometryCombo;
    QPushButton *acquisitionButton;
    QPushButton *waveformButton;

    std::shared_ptr<Database> db;  // 数据库连接
    QComboBox* createBooleanComboBox(bool value);
};

#endif // STEPEDITDIALOG_H 
