#ifndef ADDTASK_H
#define ADDTASK_H
#include <QTreeWidgetItem>
#include <QWidget>
#include "imageviewer.h"
#include "irimagedisplay.h"
#include "database.h"
#include <QSystemTrayIcon>
#include <QTableWidget>
#include "executetask.h"
#include "deviceid.h"
#include "labelediting.h"
#include <QFuture>
#include <condition_variable>
#include <QMutex>
#include "FolderCheck.h"

namespace Ui {
class AddTask;
}

class AddTask : public QWidget
{
    Q_OBJECT

public:
    explicit AddTask(QWidget *parent = nullptr);
    ~AddTask();
private slots:
    void UpdateOutputType(int index);
    
    void UpdateCollectPort(int index);

    void on_pbcreattask_clicked();

    void on_pdaddwave_clicked();

    void on_pbaddcollectport_clicked();

    void on_pbaddstep_clicked();

    void on_pbaddagain_clicked();

    void on_pbsave_clicked();

    void on_pbrefreshdeviceid_clicked();

    void on_cbdeviceid_activated(int index);

    void on_cbtesttaskid_activated(int index);

    void on_pbdelete_clicked();

    void on_pbvieworedit_clicked();

    void on_pbsaveandrun_clicked();

    void on_pbcancel_singlestep_clicked();

    void on_pbshowpcbimage_clicked();

    void on_pbselectlocation_clicked();

    void showLocationSelectionTip(const QString& message);

signals:
    void UpdateMysqlDataSignal();

private:
    Ui::AddTask *ui;
    std::vector<QString> Waveform;
    std::mutex setlocationMutex;
    std::condition_variable setlocationCondition;
    QString taskid;
    QString step_id;
    QString &device_id;
    int element_id;
    QString viewtask_id;
    QString RunStep_id;
    int step_num;
    int step_collectedtime;
    std::vector<PXIe5711Waveform> pxi5711waveform;
    std::vector<PXIe5320Waveform> pxi5322waveform;
    std::vector<PXIe5320Waveform> pxi5323waveform;
    std::vector<Data8902> data8902;
    std::shared_ptr<ExecuteTask> localExecuteTask;
    std::shared_ptr<QThread> taskThread;

    std::shared_ptr<LabelEditingWindow> labelediting;
    QImage LabeledingImage;
    std::vector<Label> label_info;
    std::vector<Label> label_info_add;
    std::vector<int> delete_id;
    int select_location_id = 0;

    std::shared_ptr<LabelEditingWindow> labelediting_selectid;
    QImage LabeledingImage_selectid;
    std::vector<Label> label_info_selectid;
    std::vector<Label> label_info_add_selectid;
    std::vector<int> delete_id_selectid;

    Database database;

    QMutex waveformMutex;

    void InitComboBox();

    void initimage();

    void updateimage();

    void display_port_info(std::vector<PXIe5711Waveform> &pxi5711waveform_display, std::vector<PXIe5320Waveform> &pxi5322waveform_display, std::vector<PXIe5320Waveform> &pxi5323waveform_display);

    void update_treeview();

    void setupStepContextMenu(QTreeWidgetItem *item, const Step &step, const QString &tableName, const QString taskid_tree);

    void populateWaveformItems(QTreeWidgetItem *parentItem, const Step &step, const QString &tableName, bool is5711);

    void edit_step(const Step &step, const QString &tableName);

    void clearTableWidgets(QTableWidget *table);

    std::pair<std::vector<PXIe5711Waveform>, std::vector<PXIe5320Waveform>> loadWaveforms(const Step &step, const QString &tableName);

    void setupTableHeaders();

    void populateTableOutputPort(const std::vector<PXIe5711Waveform>& waveforms, const QString &tableName);

    void populateTableCollectPort(const std::vector<PXIe5320Waveform>& waveforms, const QString &tableName);

    void populateOutputRow(int row, const PXIe5711Waveform &waveform, const QString &tableName);

    void populateCollectRow(int row, const PXIe5320Waveform &waveform, const QString &tableName);

    void setupOutputPortConnections(int row, const PXIe5711Waveform &waveform, const QString &tableName);

    void setupCollectPortConnections(int row, const PXIe5320Waveform &waveform, const QString &tableName);

    void updateWaveform(const PXIe5711Waveform &waveform, int row, const QString &tableName);

    template<typename T>
    void deleteWaveform(const T &waveform, const QString &tableName);

    void refresh_testtask();

    void SingleStepRun(const Step &step, const QString &tableName);

    QLabel* currentTip = nullptr;
};

#endif // ADDTASK_H
