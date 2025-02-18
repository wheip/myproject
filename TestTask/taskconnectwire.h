#ifndef TASKCONNECTWIRE_H
#define TASKCONNECTWIRE_H

#include <QWidget>
#include "ClassList.h"
#include "database.h"
#include "labelediting.h"
#include "deviceid.h"
#include <condition_variable>
#include <QFuture>
#include <QFutureWatcher>
#include <QStackedWidget>
#include <QMutex>

namespace Ui {
class TaskConnectWire;
}

class TaskConnectWire : public QWidget
{
    Q_OBJECT

public:
    explicit TaskConnectWire(QWidget *parent = nullptr, QStackedWidget *stackedWidget = nullptr);
    ~TaskConnectWire();

    void setStep(const QString step_id);
    void setDevice(const QString device_id);

public slots:

    void showTip(QString text, int location);

private slots:
    void on_pbcomplete_clicked();

    void on_pbbegin_clicked();

private:
    Ui::TaskConnectWire *ui;
    std::mutex mtx;
    std::mutex uiMutex;
    bool is_connect_wire = false;
    std::condition_variable cv;
    bool is_init_ui = false;
    std::condition_variable init_ui_cv;
    QString step_id;
    QString device_id;
    QImage LabeledingImage;
    std::vector<Label> label_info;
    std::vector<Label> label_info_add;
    std::vector<int> delete_id;
    std::unique_ptr<LabelEditingWindow> labelEditingWindow;
    std::vector<PXIe5320Waveform> pxie5320waveforms;
    std::vector<PXIe5711Waveform> pxie5711waveforms;
    std::vector<Data8902> data8902s;
    QStackedWidget *stackedWidget;

    int lastMessageStart = 0;
    int lastMessageEnd = 0;

    void initimage();
    void initWaveform();
    void initUI();
    void beginwireconnect();
};

#endif // TASKCONNECTWIRE_H
