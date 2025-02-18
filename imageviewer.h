#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QWidget>
#include <QLabel>
#include <QPixmap>

class ImageViewer : public QWidget {
    Q_OBJECT

public:
    explicit ImageViewer(QWidget *parent = nullptr, QString title = "放大图像");
    void setImage(const QPixmap &pixmap);

private:
    QLabel *label;
    QString title;
    QPixmap originalPixmap;
    double scaleFactor;
    QPoint dragStartPosition;
    bool isDragging;

    void updateImageDisplay(bool isInit = false);
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // IMAGEVIEWER_H
