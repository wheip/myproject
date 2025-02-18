#ifndef CAMERA_H
#define CAMERA_H

#include <QWidget>
#include <opencv2/opencv.hpp>
#include<opencv2/imgproc/imgproc_c.h>
#include "RtNet.h"
#include <QImage>
#include <qDebug>
#include <QCloseEvent>
#include <QShowEvent>

namespace Ui {
class Camera;
}

class Camera : public QWidget
{
    Q_OBJECT

public:
    static Camera& getInstance(QWidget *parent = nullptr, bool is_emit = false) {
        static Camera instance(parent, is_emit);
        return instance;
    }

    virtual void displayImage(cv::Mat &image);
    void showWindow();
    void closeWindow();

    void setonlyemitdata(bool is_only_emit = false);

    void temp_monitor(QImage image, uint16_t *tempData, uint32_t tempWidth, uint32_t tempHeight);

    QImage rgbImage;
    QImage signalImage;
    QString format = "RGB";
    uint16_t *tempData;
    uint32_t tempWidth;
    uint32_t tempHeight;

    Ui::Camera *ui;
    explicit Camera(QWidget *parent = nullptr, bool is_emit = false);
    virtual ~Camera();


public slots:
    virtual void on_pbsave_clicked();
    void on_pbcanael_clicked();
    void on_pbformatconversion_clicked();

signals:
    void Image_rgb(const QImage &image, uint16_t *tempData, uint32_t tempWidth, uint32_t tempHeight);

    void signal_close();

    void Image_ir_hypertherm(QImage image, std::vector<uint16_t> tempData, uint32_t tempWidth, uint32_t tempHeight);

private:

    JPEG_STREAMER rgbStreamer;
    JPEG_STREAMER irStreamer;
    TEMP_STREAMER tempStreamer;

    float hypertherm_temp = 90;

    bool is_stream_running = false;
    bool is_stream_stopping = true;
    bool is_emit;
    bool is_only_emit_data = false;


    // 禁止拷贝构造和赋值操作
    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;

protected:
    void startStream();
    void stopStream();
    void stopStreamAsync();
    void GetMousePosition(int &tempX, int &tempY, int &mouseX, int &mouseY);
    void closeEvent(QCloseEvent *event) override;
};

#endif // CAMERA_H
