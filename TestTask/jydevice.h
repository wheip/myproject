#ifndef JYDEVICE_H
#define JYDEVICE_H

#include <QWidget>
#include <QThread>
#include "ClassList.h"
#include <unordered_map>
#include "pxie5711.h"
#include "pxie8902.h"
#include "pxie5320.h"
#include "uestcqcustomplot.h"
#include <QTimer>
#include <queue>
#include <QMutex>
#include "jydevicemanager.h"

namespace Ui {
class JYDevice;
}

// 新增：用于传递每个通道数据的结构体
struct ChannelData {
    int channel;
    QVector<double> data;
};

class JYDevice : public QWidget
{
    Q_OBJECT

public:
    explicit JYDevice(QWidget *parent = nullptr);
    ~JYDevice();

private slots:
    void on_start_5711_clicked();

    void on_Trigger_5711_clicked();

    void on_stop_5711_clicked();

    void on_start_8902_clicked();

    void on_Trigger_8902_clicked();

    void on_stop_8902_clicked();

    void on_start_5323_clicked();

    void on_Trigger_5323_clicked();

    void on_stop_5323_clicked();

    void on_start_5322_clicked();

    void on_Trigger_5322_clicked();

    void on_stop_5322_clicked();

    void on_Initwaveform_clicked();

    void on_pbshow5322_clicked();

    void on_pbshow5323_clicked();

    void on_pbshow_8902_clicked();

    void on_StateChanged(const QString& state, int numb);

    void on_DataFrom5322(const std::vector<PXIe5320Waveform> collectdata, int serial_number);

    void on_DataFrom5323(const std::vector<PXIe5320Waveform> collectdata, int serial_number);

    void on_DataFrom8902(const std::vector<PXIe5320Waveform> collectdata, int serial_number);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::JYDevice *ui;
    void initTables();
    void initdevice();

    int PXIe_5711_state = 0;
    int PXIe_8902_state = 0;
    int PXIe_5323_state = 0;
    int PXIe_5322_state = 0;

    PXIe5711 *device5711 = nullptr;

    PXIe8902 *device8902 = nullptr;
    QWidget *widget8902 = nullptr;

    PXIe5320 *device5323 = nullptr;
    QWidget *widget5323 = nullptr;

    PXIe5320 *device5322 = nullptr;
    QWidget *widget5322 = nullptr;

    void init5711();
    void init8902();
    void init5323();
    void init5322();
    
    UESTCQCustomPlot *plot5322 = nullptr;
    UESTCQCustomPlot *plot5323 = nullptr;
    UESTCQCustomPlot *plot8902 = nullptr;
    vector<QCPGraph*> graph5322;
    vector<QCPGraph*> graph5323;
    vector<QCPGraph*> graph8902;
    vector<queue<float>> queue5322;
    vector<queue<float>> queue5323;
    vector<queue<float>> queue8902;
    
    QTimer* updatePlotTimer = nullptr;
    void updatePlot5322();
    void updatePlot5323();
    void updatePlot8902();
};

#endif // JYDEVICE_H
