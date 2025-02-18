#include "camera.h"
#include "ui_camera.h"
#include <QMessageBox>
#include <QPainter>
#include <QDebug>
#include <QThread>
#include "RtNet.h"
#include <QFileDialog>

Camera::Camera(QWidget *parent, bool is_emit)
    : QWidget(parent)
    , ui(new Ui::Camera)
{
    ui->setupUi(this);
    this->close();
    this->is_emit = is_emit;
    if (RtNet_Init()) {
        QMessageBox::critical(0, "失败", "相机初始化失败");
        return;
    }
}

Camera::~Camera()
{
    stopStreamAsync();
    delete ui;
}

static void JpegCallback(uint8_t *data, uint32_t dataLen, uint32_t width, uint32_t height, uint64_t pts, void *arg) {
    Camera* camera = static_cast<Camera*>(arg);
    cv::Mat image = cv::imdecode(cv::Mat(1, dataLen, CV_8UC1, data), cv::IMREAD_COLOR);
    if (!image.empty() && camera->format == "RGB") {
        camera->displayImage(image);
        camera->signalImage = QImage(image.data, image.cols, image.rows, image.step, QImage::Format_RGB888).rgbSwapped();
        cv::waitKey(1);
    }
}

static void IrJpegCallback(uint8_t *data, uint32_t dataLen, uint32_t width, uint32_t height, uint64_t pts, void *arg) {
    Camera* camera = static_cast<Camera*>(arg);
    cv::Mat image = cv::imdecode(cv::Mat(1, dataLen, CV_8UC1, data), cv::IMREAD_COLOR);
    if (!image.empty() && camera->format == "IR") {
        camera->displayImage(image);
        camera->signalImage = QImage(image.data, image.cols, image.rows, image.step, QImage::Format_RGB888).rgbSwapped();
        cv::waitKey(1);
    }
}

static void TempCallback(uint16_t *paru16Data, uint32_t u32Width, uint32_t u32Height, uint64_t u64Pts, void *pArg)
{
    Camera* camera = static_cast<Camera*>(pArg);
    camera->tempWidth = u32Width;
    camera->tempHeight = u32Height;
    camera->tempData = paru16Data;
    
    QThread* thread = QThread::create([camera]() {
        camera->temp_monitor(camera->signalImage, camera->tempData, camera->tempWidth, camera->tempHeight);
    });
    
    thread->setObjectName("TempMonitorThread");
    QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}

void Camera::startStream() {
    const char* ip = "192.168.0.200";
    ST_FLIP_INFO flipInfo;
    flipInfo.ucFlipType = 2;
    int rs = RtNet_SetImgFlip(ip, &flipInfo);
    if(rs != 0)
    {
        std::cout << "设置图像翻转失败" << std::endl;
    }
    rgbStreamer = RtNet_StartRgbJpegStream(ip, JpegCallback, this);
    irStreamer = RtNet_StartIrJpegStream(ip, IrJpegCallback, this);
    tempStreamer = RtNet_StartTemperatureStream(ip, TempCallback, this);
    is_stream_running = true;
    is_stream_stopping = false;
}

void Camera::on_pbsave_clicked()
{
    if(signalImage.isNull()) {
        QMessageBox::critical(0, "失败", "图像为空");
        return;
    }
    if(is_emit) {
        if(format == "RGB")
        {
            QMessageBox::information(this, "提示", "由于RGB模式下温度数据与图片位置不一致，请先切换到IR模式");
            return;
        }
        else
        {
            emit Image_rgb(signalImage, tempData, tempWidth, tempHeight);
        }
    }
    else {
        QString filePath = QFileDialog::getSaveFileName(this, "保存图片", "", "图片文件 (*.png *.jpg *.bmp)");
        if (!filePath.isEmpty()) {
            if (signalImage.save(filePath)) {
                QMessageBox::information(this, "成功", "图片已成功保存");
            } else {
                QMessageBox::critical(this, "失败", "保存图片时出错");
            }
        }
    }
}

void Camera::stopStream() {
    RtNet_StopRgbJpegStream(rgbStreamer);
    RtNet_StopIrJpegStream(irStreamer);
    RtNet_StopTemperatureData(tempStreamer);
}

void Camera::stopStreamAsync() {
    QThread *thread = QThread::create([this]() {
        stopStream();
        std::this_thread::sleep_for(std::chrono::seconds(10));
        is_stream_running = false;
        std::cout << "InfraredCamera stopped." << std::endl;
    });
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
    emit signal_close();
}

void Camera::on_pbcanael_clicked()
{
    this->close();
}

void Camera::GetMousePosition(int &tempX, int &tempY, int &mouseX, int &mouseY) {
    QPoint pos = QCursor::pos(); // 获取鼠标的全局位置
    if (ui->label_image->underMouse()) {
        QPoint labelPos = ui->label_image->mapFromGlobal(pos); // 获取鼠标在label_image中的相对位置
        mouseX = static_cast<int>(labelPos.x());
        mouseY = static_cast<int>(labelPos.y());
        // 获取label_image中显示的图片
        QPixmap pixmap = ui->label_image->pixmap();
        if (!pixmap.isNull()) {
            // 获取图片的尺寸
            QSize imageSize = pixmap.size();
            QSize labelSize = ui->label_image->size();

            // 计算缩放比例
            qreal scaleX = labelPos.x() / static_cast<qreal>(labelSize.width());
            qreal scaleY = labelPos.y() / static_cast<qreal>(labelSize.height());

            // 计算相对坐标
            tempX = static_cast<int>(tempWidth * scaleX);
            tempY = static_cast<int>(tempHeight * scaleY);
        }
    }
}

void Camera::displayImage(cv::Mat &image) {
    if(is_only_emit_data) {
        return;
    }
    this->setMaximumSize(1920, 1080); // 设置最大窗口大小为1920x1080
    this->setMinimumSize(640, 480); // 设置最小窗口大小为640x480
    if(format == "IR")
    {    // 获取鼠标在图像上的位置
        int tempX = 0, tempY = 0;
        int mouseX = 0, mouseY = 0;
        GetMousePosition(tempX, tempY, mouseX, mouseY);
        if (tempX < 0 || tempX >= tempWidth || tempY < 0 || tempY >= tempHeight) {
            qDebug() << "Mouse position is out of image bounds!";
            return;
        }
        float displayTemp = 0.1 * this->tempData[tempY * tempWidth + tempX]; // 根据鼠标位置获取温度值
        cv::Mat tempImage;
        cv::cvtColor(image, tempImage, cv::COLOR_BGR2RGB); // 将图像从BGR格式转换为RGB格式
        rgbImage = QImage((const unsigned char*)tempImage.data, tempImage.cols, tempImage.rows,
                          tempImage.step[0], QImage::Format_RGB888); // 更新rgbImage

        // 在label_image上绘制温度标签
        QString tempText = QString::number(displayTemp, 'f', 2) + " °C"; // 温度值文本
        QSize image_size = ui->label_image->size();
        QPixmap pixmap = QPixmap::fromImage(rgbImage).scaled(image_size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        if(image_size == ui->label_image->size())
        {
            QPainter painter(&pixmap);
            painter.setPen(QPen(Qt::green, 2));
            painter.drawText(QPoint(mouseX, mouseY), tempText); // 在label_image的指定位置绘制温度值
            ui->label_image->setPixmap(pixmap);
        }
    }
    else
    {
        cv::Mat tempImage;
        cv::cvtColor(image, tempImage, cv::COLOR_BGR2RGB); // 将图像从BGR格式转换为RGB格式
        rgbImage = QImage((const unsigned char*)tempImage.data, tempImage.cols, tempImage.rows,
                          tempImage.step[0], QImage::Format_RGB888); // 更新rgbImage
        QSize image_size = ui->label_image->size();
        QPixmap pixmap = QPixmap::fromImage(rgbImage).scaled(image_size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        if(image_size == ui->label_image->size())
        {
            ui->label_image->setPixmap(pixmap);
        }
    }
}

void Camera::setonlyemitdata(bool is_only_emit) {
    is_only_emit_data = is_only_emit;
    format = "IR";
    ui->pbformatconversion->setText("IR");
}

void Camera::showWindow() {
    if(!is_only_emit_data) {
        this->show();
    }
    if(!is_stream_running) {
        startStream();
    }
    else {
        std::cout << "InfraredCamera is already started. Please stop it first." << std::endl;
    }
}

void Camera::closeWindow() {
    this->close();
    if(is_stream_running) {
        is_stream_running = false;
        stopStreamAsync();
    }
    else
    {
        std::cout << "InfraredCamera is not started. Please start it first." << std::endl;
    }
}


void Camera::on_pbformatconversion_clicked()
{
    if(format == "RGB") {
        format = "IR";
        ui->pbformatconversion->setText("IR");
    } else {
        format = "RGB";
        ui->pbformatconversion->setText("RGB");
    }
}

void Camera::closeEvent(QCloseEvent *event) {
    if(is_stream_running) {
        is_stream_running = false;
        stopStreamAsync();
    }
    else
    {
        std::cout << "InfraredCamera is not started. Please start it first." << std::endl;
    }
    event->accept();
}

void Camera::temp_monitor(QImage image, uint16_t *tempData, uint32_t tempWidth, uint32_t tempHeight) {
    if(!is_stream_running) return;
    float maxTemp = -273.15f;
    std::vector<uint16_t> tempData_vec(tempData, tempData + tempWidth * tempHeight);
    for (const auto& temp : tempData_vec) {
        float currentTemp = 0.1f * temp;
        maxTemp = std::max(maxTemp, currentTemp);
    }
    if(maxTemp > hypertherm_temp) {
        emit Image_ir_hypertherm(image, tempData_vec, tempWidth, tempHeight);
    }
}
