#include "adddevice.h"
#include "ui_adddevice.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QPixmap>
#include <QImage>
#include <QBuffer>
#include "imageviewer.h"
#include <QRegExp>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <QProcess>

Adddevice::Adddevice(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Adddevice)
    , db("adddevice", this)
    , yolomodel(YOLOModel::getInstance())
    , pcbcomponentsdetect(PCBComponentsDetect::getInstance())
{
    ui->setupUi(this);
    this->setWindowTitle("添加设备");
    qDebug() << "adddevice";
    // this->close();

    lastPath = QDir::homePath(); // 默认路径为用户主目录

    connect(pcbcomponentsdetect.get(), &PCBComponentsDetect::signal_save_image_label, this, &Adddevice::save_image_label);
}

Adddevice::~Adddevice()
{
    delete ui;
}

void Adddevice::on_pbadddevice_clicked()
{
    ui->pbadddevice->setEnabled(false);
    Device device;
    if(ui->lineEdit_id->text().isEmpty())
    {
        QMessageBox::warning(this, "错误", "设备ID不能为空");
        ui->pbadddevice->setEnabled(true);
        return;
    }
    QRegExp rx("[a-zA-Z0-9_]+");
    if(!rx.exactMatch(ui->lineEdit_id->text()))
    {
        QMessageBox::warning(this, "错误", "设备ID只能由数字、字母和下划线组成");
        ui->pbadddevice->setEnabled(true);
        return;
    }
    device.id = ui->lineEdit_id->text();
    device.device_name = ui->lineEdit_name->text().toUtf8();
    device.device_driver_Voltage = 0;
    device.device_driver_Current = 0;
    device.device_driver_Power = 0;
    device.image = byteArray;
    QFuture<QString> insertFuture = QtConcurrent::run([this, device]() -> QString {
        QString ErrorMessage;
        std::vector<Label> labels(label_info_add);
        labels.insert(labels.end(), label_info.begin(), label_info.end());
        if (db.insert_device(device, ErrorMessage) && db.insert_deviceelement(device.id, labels, ErrorMessage)) {
            byteArray.clear();
            ui->lineEdit_image->setText("");
            return "";
        }
        else
        {
            return ErrorMessage;
        }
    });
    QFutureWatcher<QString> *watcher = new QFutureWatcher<QString>();
    connect(watcher, &QFutureWatcher<QString>::finished, [this, watcher]() {
        QString ErrorMessage = watcher->future().result();
        if (ErrorMessage.isEmpty()) {
            QMessageBox::information(0, "添加成功", "设备添加成功");
        } else {
            QMessageBox::warning(this, "错误", ErrorMessage);
        }
        ui->pbadddevice->setEnabled(true);
        watcher->deleteLater();
    });
    watcher->setFuture(insertFuture);
}

void Adddevice::on_pdcancel_clicked()
{
    this->close();
    pcbcomponentsdetect->close();
    byteArray.clear();
    ui->lineEdit_image->setText("");
}

void Adddevice::on_pushButton_selectFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "选择图片", lastPath, "Images (*.png *.xpm *.jpg)");
    if (!fileName.isEmpty()) {
        lastPath = QFileInfo(fileName).absolutePath(); // 记录上次选择的路径
        ui->lineEdit_image->setText(fileName);

        QImage image(fileName);
        if (image.isNull()) {
            QMessageBox::warning(this, "错误", "无法加载图片，请选择有效的图片文件。");
            return;
        }

        cv::Mat cvImage = cv::imread(fileName.toStdString());
        cv::Mat recog_image = yolomodel->recognize(cvImage);
        label_info.clear();
        label_info_add.clear();
        label_info_add =  yolomodel->get_labels();
        delete_id.clear();
        labelediting = std::make_unique<LabelEditingWindow>(nullptr, image, label_info, label_info_add, delete_id);
        labelediting->show();

        connect(labelediting.get(), &LabelEditingWindow::window_close, [this, image]() {
            label_info_add.clear();
            labelediting->labelEditing->getAllLabelItemInfo(label_info_add);
            QBuffer buffer(&byteArray);
            buffer.open(QIODevice::WriteOnly);
            if (!image.save(&buffer, "PNG")) {
                QMessageBox::critical(this, "错误", "保存图片失败。");
            }
        });
    }
}

void Adddevice::GetImage_from_camera()
{
    if(image_from_camera.isNull())
    {
        QMessageBox::warning(this, "错误", "图片为空");
        return;
    }
    QProcess process;
    process.start("nvidia-smi --gpu-reset");
    process.waitForFinished();
    if (process.exitCode() == 0) {
        qDebug() << "GPU内存已成功释放";
    } else {
        QMessageBox::warning(this, "警告", "GPU内存释放失败，请检查GPU状态");
    }

    label_info.clear();
    delete_id.clear();
    labelediting = std::make_unique<LabelEditingWindow>(nullptr, image_from_camera, label_info, label_info_add, delete_id);
    labelediting->show();

    connect(labelediting.get(), &LabelEditingWindow::window_close, [this]() {
        label_info_add.clear();
        labelediting->labelEditing->getAllLabelItemInfo(label_info_add);
        SaveImage_from_camera();
    });
    
}

void Adddevice::SaveImage_from_camera()
{
    QFuture<bool> future = QtConcurrent::run([this]() -> bool {
        QBuffer buffer(&byteArray);
        if (!buffer.open(QIODevice::WriteOnly)) {
            qDebug() << "无法打开缓冲区。";
            return false;
        }
        if (!this->image_from_camera.save(&buffer, "PNG")) {
            qDebug() << "保存图片失败。";
            return false;
        }
        return true;
    });
    if(future.result())
    {
        ui->lineEdit_image->setText("照片已保存！");
    }
    else
    {
        QMessageBox::warning(this, "错误", "保存图片失败。");
    }

    camera_flag = false;
}

void Adddevice::save_image_label(const cv::Mat &image, const std::vector<Label>& labels) {
    if(!camera_flag || image.empty())
    {
        return;
    }
    pcbcomponentsdetect->close();
    QImage temp_image(image.data, image.cols, image.rows, image.step, QImage::Format_RGB888);
    image_from_camera = temp_image.copy();
    label_info_add.clear();
    label_info_add.insert(label_info_add.end(), labels.begin(), labels.end());
    GetImage_from_camera();
}

void Adddevice::on_pbcamera_clicked()
{
    pcbcomponentsdetect->startrecog();
    camera_flag = true;
}

void Adddevice::closeEvent(QCloseEvent *event)
{
    this->close();
    pcbcomponentsdetect->close();
    byteArray.clear();
    ui->lineEdit_image->setText("");
}

