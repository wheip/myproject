#include "createdevicedialog.h"
#include "ui_createdevicedialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QBuffer>

CreateDeviceDialog::CreateDeviceDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CreateDeviceDialog)
{
    ui->setupUi(this);
    
    // 连接按钮信号
    connect(ui->pbSelectImage, &QPushButton::clicked, this, &CreateDeviceDialog::onSelectImage);
    connect(ui->pbCaptureImage, &QPushButton::clicked, this, &CreateDeviceDialog::onCaptureImage);
}

CreateDeviceDialog::~CreateDeviceDialog()
{
    delete ui;
}

void CreateDeviceDialog::onImageCaptured(const cv::Mat& image, const std::vector<Label>& labels)
{
    selectedMat = image.clone();
    deviceLabels = labels;
    
    cv::Mat matRGB;
    cv::cvtColor(selectedMat, matRGB, cv::COLOR_BGR2RGB);
    QImage qimg(matRGB.data, matRGB.cols, matRGB.rows, matRGB.step, QImage::Format_RGB888);
    selectedImage = qimg.copy();
    LabelEditing();
}

void CreateDeviceDialog::onSelectImage()
{
    // 弹出文件选择对话框
    QString fileName = QFileDialog::getOpenFileName(this, "选择设备图片", QString(), "图片文件 (*.png *.jpg *.bmp)");
    if (!fileName.isEmpty()) {
        cv::Mat mat = cv::imread(fileName.toStdString(), cv::IMREAD_COLOR);
        if (!mat.empty()) {
            cv::Mat matRGB;
            cv::cvtColor(mat, matRGB, cv::COLOR_BGR2RGB);
            // 创建 QImage 用于显示预览
            QImage qimg(matRGB.data, matRGB.cols, matRGB.rows, matRGB.step, QImage::Format_RGB888);
            selectedImage = qimg.copy();
            // 保存原始 OpenCV 图片到成员变量供后续处理使用
            selectedMat = mat;
            YOLOModel::getInstance()->recognize(selectedMat);
            deviceLabels.clear();
            deviceLabels = YOLOModel::getInstance()->get_labels();
            LabelEditing();
            // 更新图片路径显示
            ui->lineEdit_3->setText(fileName);
        } else {
            QMessageBox::warning(this, "错误", "无法加载选中的图片");
        }
    }
}

void CreateDeviceDialog::LabelEditing()
{
    LabelManagerDialog dlg(this, selectedImage, -1, deviceLabels);
    if(dlg.exec() == QDialog::Accepted)
    {
        deviceLabels = dlg.getLabels();
        ui->lineEdit_3->setText("已保存");
    }
}

void CreateDeviceDialog::onCaptureImage()
{
    // 使用QScopedPointer自动管理对话框内存
    QScopedPointer<LabelImageViewDialog> imageDialog(new LabelImageViewDialog(this));
    
    // 使用一次性连接，确保信号槽在对话框销毁后断开
    connect(imageDialog.data(), &LabelImageViewDialog::imageCaptured, 
            this, &CreateDeviceDialog::onImageCaptured,
            Qt::SingleShotConnection);
            
    imageDialog->exec();
}

Device CreateDeviceDialog::getDevice() const
{
    Device device;  // 创建一个新的Device对象
    device.id = -1;
    device.device_name = ui->lineEdit_name->text().toUtf8();
    device.device_driver_Voltage = 0;
    device.device_driver_Current = 0;
    device.device_driver_Power = 0;

    if (!selectedMat.empty()) {
        std::vector<uchar> buf;
        std::vector<int> params;
        params.push_back(cv::IMWRITE_JPEG_QUALITY);  // JPG质量参数
        params.push_back(100);  // 设置最高质量(0-100)
        
        cv::imencode(".jpg", selectedMat, buf, params);  // 使用JPG格式编码
        QByteArray imageData(reinterpret_cast<const char*>(buf.data()), buf.size());
        device.image = imageData;
    }
    return device;
}

