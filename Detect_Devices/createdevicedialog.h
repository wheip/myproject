#ifndef CREATEDEVICEDIALOG_H
#define CREATEDEVICEDIALOG_H

#include <QDialog>
#include "ClassList.h"
#include "labelimageviewdialog.h"
#include "yolomodel.h"
#include "LabelManagerDialog.h"
#include <QScopedPointer>

namespace Ui {
class CreateDeviceDialog;
}

class CreateDeviceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateDeviceDialog(QWidget *parent = nullptr);
    ~CreateDeviceDialog();

    Device getDevice() const;

    std::vector<Label> getDeviceLabels() const { return deviceLabels; };

private slots:
    void onCaptureImage();
    void onSelectImage();
    void onImageCaptured(const cv::Mat& image, const std::vector<Label>& labels);

private:
    Ui::CreateDeviceDialog *ui;
    QImage selectedImage;
    cv::Mat selectedMat;
    std::vector<Label> deviceLabels;

    void LabelEditing();
};

#endif // CREATEDEVICEDIALOG_H
