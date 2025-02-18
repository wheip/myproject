#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "TaskManager/FlowTaskManager.h"
#include <QDebug>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QThread>
#include <fstream>
#include <sstream>
#include <QInputDialog>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QWidget>

bool taskisexecuting = false;
QString current_device_id = "";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , database(Database("fault_detect", this))
    , yolomodel(YOLOModel::getInstance())
{
    manageTask = std::make_shared<ManageTask>(this);
    // addtask = std::make_shared<AddTask>(this);
    FlowTaskManagerptr = std::make_shared<FlowTaskManager>(this);
    manageDevice = std::make_shared<SearchDevice>(this);
    ui->setupUi(this);
    ui->statusbar->showMessage("Ready");
    manageTask->close();
    yolomodel_init();
    pcbcomponentsdetect = std::make_shared<PCBComponentsDetect>(nullptr);
    pxi5711 = std::make_shared<PXIe5711>();
    
    g_FolderCheck.Check_Folder(); // 检查文件夹完整性

    InitListSelectDevice();
    currentWidget = FlowTaskManagerptr;

    // 创建 FlowTaskManager 实例
    
}

MainWindow::~MainWindow()

{
    qDebug() << "MainWindow::~MainWindow()";
    delete ui;
    qDebug() << "MainWindow::~MainWindow() end";
}

void MainWindow::switchWidget(std::shared_ptr<QWidget> newWidget)
{
    qDebug() << "switchWidget: newWidget->show()";
    if (ui->display->count() > 0) {
        QLayoutItem *item = ui->display->itemAt(0);
        if (item && item->widget()) {
            ui->display->removeWidget(item->widget());
        }
    }
    if(currentWidget){
        currentWidget = newWidget;
        ui->display->addWidget(currentWidget.get());
        qDebug() << "switchWidget: currentWidget->show()";
        currentWidget->show();
    }
}

void MainWindow::on_actiontesttaskmanage_triggered()
{
    if(currentWidget){
        currentWidget->hide();
    }
    switchWidget(FlowTaskManagerptr);
}

void MainWindow::on_actionPCB_triggered()
{
    connect(pcbcomponentsdetect.get(), &PCBComponentsDetect::signal_save_image_label, [this](const cv::Mat &image, const std::vector<Label>& labels){
        label_info = labels;
        image_from_camera = QImage(image.data, image.cols, image.rows, image.step, QImage::Format_RGB888);
        pcbcomponentsdetect->close();
    });
    pcbcomponentsdetect->startrecog();
}

void MainWindow::on_actiontaskmanage_triggered()
{
    if(currentWidget){
        currentWidget->hide();
    }
    switchWidget(manageTask);
}

void MainWindow::updateMysqlData()
{
    // updatamysqldata->notyfy();
}

void MainWindow::on_actiondevicemanage_triggered()
{
    if(currentWidget){
        currentWidget->hide();
    }
    switchWidget(manageDevice);
}

void MainWindow::InitListSelectDevice()
{
    std::vector<Device> devices;
    database.get_device("", devices, false);
    ui->menulistselect->clear();
    QAction* action = new QAction("刷新列表", ui->menulistselect);
    connect(action, &QAction::triggered, [this](){
        InitListSelectDevice();
    });
    
    ui->menulistselect->addAction(action);
    for(const auto& device : devices){
        QAction* action = new QAction(device.id, ui->menulistselect);
        ui->menulistselect->addAction(action);
        connect(action, &QAction::triggered, [this, device](){
            current_device_id = device.id;
            gDeviceId.setDeviceId(current_device_id);
        });
    }
    if(devices.size() > 0){
        current_device_id = devices[0].id;
        gDeviceId.setDeviceId(current_device_id);
    }
}

void MainWindow::on_actionautodetect_triggered()
{
    if(current_device_id.isEmpty()){
        QMessageBox::information(this, "提示", "请先选择设备");
        return;
    }
    qDebug() << "current_device_id: " << current_device_id;
}

void MainWindow::yolomodel_init(){
    QThread* yolomodelthread;
    yolomodelthread = QThread::create([this](){
        yolomodel->recognize(cv::Mat(cv::Size(640, 640), CV_8UC3, cv::Scalar(0, 0, 0)));
    });
    yolomodelthread->start();
}

void MainWindow::on_actiontest_triggered()
{
    if(currentWidget){
        currentWidget->hide();
    }
    switchWidget(FlowTaskManagerptr);
    // pxie8902 = std::make_shared<PXIe8902>();
    // std::vector<Data8902> collectdata;
    // double collecttime = 0.1;
    // QString condition = "id = 'ceshi1_1_8902_voltage'";
    // database.get_8902data("rt_iohsdi$$pxie8902", condition, collectdata);
    // pxie8902->StartAcquisition(collectdata, collecttime);
    // pxie8902->SendSoftTrigger();
    // connect(pxie8902.get(), &PXIe8902::signalAcquisitionData, [this](const std::vector<PXIe5320Waveform>& data, int serial_number){
    //     for(const auto& waveform : data){
    //         qDebug() << "id: " << waveform.id << "device: " << waveform.device << "datasize: " << waveform.data.size();
    //     }
    // });
    // connect(pxie8902.get(), &PXIe8902::CompleteAcquisition, [this](){
    //     pxie8902.reset();
    // });

    // waveforms.push_back(waveform);
    // pxi5711->receivewaveform(waveforms);
    // pxi5711->SendSoftTrigger();
    // system("start C:/Users/Administrator/Pictures/A41E6B690A36CE9E0BE10B70A30DA5E3.jpg");
    // pcbexteaction = std::make_shared<PCBExteaction>(QString("C:/Users/Administrator/Pictures/A41E6B690A36CE9E0BE10B70A30DA5E3.jpg"));
    // std::vector<Data8902> datas;
    // QString id = QInputDialog::getText(this, "提示", "请输入id");
    // QString condition = "id = '" + id + "'";
    // database.get_8902data("frss$$pxie8902", condition, datas);
    // for(const auto& data : datas){
    //     QString notes = QString::fromUtf8(data.notes);
    //     QMessageBox::information(this, "提示", notes);
    // }
    // pxie8902->show();
    // detectcamera->start();
    // detectcamera->Setmode(false);
    // detectcamera->show();
    // cv::VideoCapture cap;
    // cap.open(0);
    // cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    // cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
    // cap.set(CV_CAP_PROP_FPS, 30);
    // cv::Mat frame;
    // int i = 0;
    // while(true){
    //     cap >> frame;
    //     cv::imshow("test", frame);
    //     cv::waitKey(1);
    //     i++;
    //     if(i > 100)
    //         break;
    // }
}

