#ifndef SEARCHDEVICE_H
#define SEARCHDEVICE_H

#include <QWidget>
#include "database.h"
#include "imageviewer.h"
#include <QTableWidgetItem>
#include "detectcamera.h"
#include "adddevice.h"

namespace Ui {
class SearchDevice; 
}

class SearchDevice : public QWidget
{
    Q_OBJECT

public:
    explicit SearchDevice(QWidget *parent = nullptr);
    ~SearchDevice();
    ImageViewer *viewer;
private slots:
    void current_Image(const cv::Mat &image, const std::vector<Label>& labels);

    void on_pdsearch_clicked();

    void on_tableWidget_devices_itemDoubleClicked(QTableWidgetItem *item);

    void on_pbadddevice_clicked();

private:
    void populateTable(const vector<Device>& devices);
    void clearTableWidgets(QTableWidget *table);
    void populateDeviceRow(int row, const Device& device);
    void showContextMenu(const QPoint &pos, int row, QTableWidgetItem *itemImage);
    void deleteImage(int row, QTableWidgetItem *itemImage);
    void replaceImageFromLocal(int row, QTableWidgetItem *itemImage);
    void replaceImageFromCamera();
    void saveImageToLocal(int row, QTableWidgetItem *itemImage);
    Device getDeviceFromRow(int row);
    void updateDevice(const Device& device, int row);
    void deleteDevice(const Device& device, int row);
    void saveImage();
    
    std::shared_ptr<Adddevice> adddevice;
    std::unique_ptr<LabelEditingWindow> labelediting;
    int replaceImageFromCamera_row;
    QTableWidgetItem *replaceImageFromCamera_itemImage;
    bool replaceImageFromCamera_flag = false;
    QWidget *parent;
    Ui::SearchDevice *ui;
    Database db;
    std::shared_ptr<PCBComponentsDetect> pcbcomponentsdetect;
    std::shared_ptr<YOLOModel>& yolomodel;
    Device currentDevice;
    QImage currentImage;
    std::vector<Label> currentLabels;
    std::vector<Label> currentLabels_add;
    std::vector<int> delete_id;
    QStringList headerLabels;
};

#endif // SEARCHDEVICE_H
