#ifndef WAVEFORMMANAGEDIALOG_H
#define WAVEFORMMANAGEDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QPushButton>
#include <QList>
#include <QComboBox>
#include <QDialogButtonBox>
#include "ClassList.h"
#include <memory>
#include "database.h"
#include "ConnectionLocationDialog.h"

// 本对话框专门用于管理输出信号（波形）的增删改查
class WaveformManageDialog : public QDialog
{
    Q_OBJECT
signals:
    void waveformsUpdated(const QList<PXIe5711Waveform>& waveforms);

public:
    explicit WaveformManageDialog(const QString& stepId, const QString& deviceId, QWidget *parent = nullptr);
    QList<PXIe5711Waveform> waveforms() const { return m_waveforms; }
    void setWaveforms(const QList<PXIe5711Waveform>& waveforms) {
        m_waveforms = waveforms;
        setupTable();
        updateChannelComboBox();
    }

private slots:
    void addWaveform();
    void deleteWaveform();
    void onItemChanged(QTableWidgetItem *item);
    void onAccepted();
    void showConnectionLocationDialog(int row, int column);

private:
    void setupUI();
    void setupTable();
    void updateTableRow(int row, const PXIe5711Waveform& waveform);
    void updateChannelComboBox();
    bool isChannelUsed(int channel) const;
    static QString getChannelDisplayName(int channel);
    static QString getChannelTypeName(int channel);
    static QStringList getWaveformTypes(int channel);
    static const QStringList ANALOG_POWER_TYPES;  // 模拟量和电源输出的波形类型
    static const QStringList DIGITAL_TYPES;       // 数字量输出的波形类型

    QTableWidget *tableWidget;
    QPushButton *addButton;
    QPushButton *deleteButton;
    QComboBox *channelComboBox;
    QString m_deviceId;  // 存储设备ID
    QString m_stepId;
    QList<PXIe5711Waveform> m_waveforms;
    static const QList<int> AVAILABLE_CHANNELS;

    // 独立的数据库连接
    std::shared_ptr<Database> db;

    QImage deviceImage;  // 添加设备图像成员
    std::vector<Label> deviceLabels;  // 添加设备标签成员

protected:
    void showEvent(QShowEvent *event) override;
};

#endif // WAVEFORMMANAGEDIALOG_H 
