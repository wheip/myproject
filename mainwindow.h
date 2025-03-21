#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <memory>
#include "FlowTaskManager.h"
// #include "addtask.h"
#include "executetask.h"
#include "ClassList.h"
#include "managetask.h"
#include "detectcamera.h"
#include "pxie5320.h"
#include "pxie5711.h"
#include "database.h"
#include "irimagedisplay.h"
#include "pcbcomponentsdetect.h"
#include "yolomodel.h"
#include "FolderCheck.h"
#include "pxie8902.h"
#include "devicemanager.h"
#include "labelediting.h"
#include "deviceid.h"
#include <iostream>
#include "siftmatcher.h"
#include "previewdialog.h"
#include "imageflowdialog.h"
#include "jydevice.h"

using namespace std;
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
private slots:

    void on_actionPCB_triggered();

    void on_actiondevicemanage_triggered();

    void on_actiontaskmanage_triggered();

    void on_actiontesttaskmanage_triggered();

    void on_actiontest_triggered();

    void on_actionautodetect_triggered();

    void on_actiontdevice_triggered();

private:
    Ui::MainWindow *ui;
    std::shared_ptr<QWidget> currentWidget;
    std::shared_ptr<ManageTask> manageTask;
    std::shared_ptr<FlowTaskManager> FlowTaskManagerptr;
    std::shared_ptr<DeviceManager> deviceManager;
    std::shared_ptr<JYDevice> jydevice;

    std::shared_ptr<PXIe5320> pxi5322;
    std::shared_ptr<PXIe5320> pxi5323;
    std::shared_ptr<PXIe5711> pxi5711;
    std::shared_ptr<PXIe8902> pxie8902;
    Database database;
    QThread pxi5711thread;
    QThread pxi5322thread;
    QThread pxi5323thread;
    std::shared_ptr<ExecuteTask> localExecuteTask;
    std::vector<Label> label_info;
    QImage image_from_camera;
    std::vector<Label> label_info_add;

    std::shared_ptr<YOLOModel>& yolomodel;
    std::shared_ptr<PCBComponentsDetect> pcbcomponentsdetect;
    std::unique_ptr<LabelEditing> labelediting;
    std::shared_ptr<DetectCamera> detectcamera;
    std::shared_ptr<IRImageDisplay> irimagedisplay;
    void switchWidget(std::shared_ptr<QWidget> newWidget);

    void yolomodel_init();
    void InitListSelectDevice();
    void onImageCaptured(const cv::Mat& image);
};
#endif // MAINWINDOW_H
