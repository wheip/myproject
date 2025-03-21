#ifndef ICONIMAGE_H
#define ICONIMAGE_H
#include "ClassList.h"
#include <QFileDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QImage>
#include <database.h>
#include <QDesktopServices>
#include <QUrl>

class IconImage : public QLabel
{
public:
//     IconImage(QWidget *parent, Image displayimage, ImageViewer *imageViewer, IRImageDisplay *irimagedisplay, QStackedWidget *stackedWidget, Database &database, QString device_id)
//                     : QLabel(parent)
//                     , parent(parent)
//                     , imageViewer(imageViewer)
//                     , irimagedisplay(irimagedisplay)
//                     , stackedWidget(stackedWidget)
//                     , image(displayimage)
//                     , device_id(device_id)
//                     , database(database)
// {
//     setMouseTracking(true); // 启用鼠标跟踪
//     QVBoxLayout *layout = new QVBoxLayout(this); // 创建垂直布局
//     QLabel *imageLabel = new QLabel; // 创建用于显示图片的标签
    
//     // 使用QScrollArea包装文本标签
//     QScrollArea *scrollArea = new QScrollArea;
//     QLabel *textLabel = new QLabel; // 创建用于显示文本的标签
//     scrollArea->setWidget(textLabel);
//     scrollArea->setWidgetResizable(true);
//     scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
//     scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//     scrollArea->setMaximumHeight(50); // 设置滚动区域的最大高度为50
//     scrollArea->setFrameShape(QFrame::NoFrame); // 去掉边框

//     QPixmap pixmap = QPixmap::fromImage(QImage::fromData(image.image_data));
//     pixmap = pixmap.scaled(100, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation); // 图片大小调整为100x50
//     imageLabel->setPixmap(pixmap);
//     imageLabel->setAlignment(Qt::AlignCenter); // 图片居中显示
//     imageLabel->setToolTip(image.id); // 显示图片名称

//     textLabel->setText(image.id);
//     textLabel->setAlignment(Qt::AlignCenter); // 文本居中显示

//     layout->addWidget(imageLabel);
//     layout->addWidget(scrollArea);
//     setLayout(layout);

//     // 固定布局的大小
//     this->setFixedSize(120, 120); // 设置固定大小为120x120，以适应更高的文本标签
// }

// protected:
//     ImageViewer *imageViewer;
//     IRImageDisplay *irimagedisplay;
//     QStackedWidget *stackedWidget;
//     Image image;
//     QString device_id;
//     QWidget *parent;
//     Database &database;
//     void mousePressEvent(QMouseEvent *event) override {
//         if (event->button() == Qt::LeftButton && event->type() == QEvent::MouseButtonDblClick) {
//             // 处理左键双击事件
//             if(imageViewer != nullptr)
//             {
//                 irimagedisplay->close();
//                 imageViewer->setImage(QPixmap::fromImage(QImage::fromData(image.image_data)));
//                 stackedWidget->setCurrentIndex(0);
//                 imageViewer->show();
//             }
//             if(irimagedisplay != nullptr && image.device == "infraredcamera")
//             {
//                 imageViewer->close();
//                 QImage qImage = QImage::fromData(image.image_data);
//                 std::vector<uint16_t> tempVector(reinterpret_cast<uint16_t*>(image.temp_data.data()),
//                                                  reinterpret_cast<uint16_t*>(image.temp_data.data() + image.temp_data.size()));
//                 irimagedisplay->setImage(qImage, tempVector, image.temp_width, image.temp_height);
//                 stackedWidget->setCurrentIndex(1);
//                 irimagedisplay->show();
//             }
//         } else if (event->button() == Qt::RightButton) {
//             // 处理右键点击事件
//             QMenu menu(this);
//             QAction *deleteAction = menu.addAction("删除");
//             QAction *saveAction = menu.addAction("保存到本地");

//             QAction *selectedAction = menu.exec(event->globalPos());

//             if (selectedAction == deleteAction) {
//                 QString table_name_image = device_id + "$$image";
//                 database.delete_image(table_name_image, QString("id = '%1'").arg(image.id));
//                 QMessageBox::information(this, "提示", "图像已删除,请刷新列表", QMessageBox::Ok);
//             } else if (selectedAction == saveAction) {
//                 QString savePath = QFileDialog::getExistingDirectory(this, "选择保存路径", QDir::homePath());
//                 if (!savePath.isEmpty()) {
//                     QString fileName = QFileDialog::getSaveFileName(this, "保存图像",
//                                                                     savePath + "/" + image.id + ".jpg",
//                                                                     "图像文件 (*.jpg *.png)");
//                     if (!fileName.isEmpty()) {
//                         QImage qImage = QImage::fromData(image.image_data);
//                         qImage.save(fileName);
//                         QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
//                     }
//                     if(image.device == "infraredcamera")
//                     {
//                         QImage qImage = QImage(reinterpret_cast<uchar*>(image.temp_data.data()), image.temp_width, image.temp_height, QImage::Format_Grayscale16);
//                         qImage.save(fileName.replace(".jpg", "_temp.png"));
//                     }
//                 }
//             }
//         }
//     }
    
};
#endif // ICONIMAGE_H
