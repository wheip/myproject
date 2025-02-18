#ifndef ADDDEVICE_H
#define ADDDEVICE_H

#include <QWidget>
#include "database.h"
#include "camera.h"
#include "ClassList.h"
#include "database.h"
#include "pcbcomponentsdetect.h"
#include "yolomodel.h"
#include "labelediting.h"
#include <condition_variable>

namespace Ui {
class Adddevice;
}

class Adddevice : public QWidget
{
    Q_OBJECT

public:
    explicit Adddevice(QWidget *parent);
    ~Adddevice();

private slots:

    void save_image_label(const cv::Mat &image, const std::vector<Label>& labels);

    void on_pbadddevice_clicked();

    void on_pdcancel_clicked();

    void on_pushButton_selectFile_clicked();

    void on_pbcamera_clicked();
protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::Adddevice *ui;
    Database db;
    QByteArray byteArray; // 设置byteArray的大小为1080p（1920x1080，假设每个像素4字节）
    QString lastPath;
    QImage image_from_camera;
    bool camera_flag = false;
    std::shared_ptr<PCBComponentsDetect> pcbcomponentsdetect;
    std::shared_ptr<YOLOModel>& yolomodel;
    std::unique_ptr<LabelEditingWindow> labelediting;
    std::vector<Label> label_info;
    std::vector<Label> label_info_add;
    std::vector<int> delete_id;
    std::mutex mtx;
    std::condition_variable cv;

    void GetImage_from_camera();
    void SaveImage_from_camera();
};

#endif // ADDDEVICE_H
