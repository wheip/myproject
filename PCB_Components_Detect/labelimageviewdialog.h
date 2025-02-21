#ifndef LABELIMAGEVIEWDIALOG_H
#define LABELIMAGEVIEWDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QScrollArea>
#include "detectcamera.h"
#include "yolomodel.h"

class LabelImageViewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LabelImageViewDialog(QWidget *parent = nullptr);
    ~LabelImageViewDialog();

signals:
    void imageCaptured(const cv::Mat& image, const std::vector<Label>& labels);

private slots:
    void onFrameReady(const cv::Mat& frame);
    void captureImage();
    void onLabelToggled(int index, bool checked);
    void selectAllLabels();
    void deselectAllLabels();
    
private:
    QLabel *previewLabel;
    QPushButton *captureButton;
    QPushButton *cancelButton;
    QWidget *labelPanel;
    std::vector<QCheckBox*> labelCheckBoxes;
    
    cv::Mat currentFrame;
    cv::Mat originalFrame;
    std::mutex frameMutex;
    std::atomic<bool> isProcessing{false};
    
    void setupUI();
    void setupLabelPanel();
    QImage cvMatToQImage(const cv::Mat& mat);
    QString getStyleSheet(const cv::Scalar& color);
};

#endif // LABELIMAGEVIEWDIALOG_H 