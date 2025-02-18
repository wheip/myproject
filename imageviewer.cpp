#include "imageviewer.h"
#include <QVBoxLayout>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QDebug>
#include <QScrollArea>
#include <QCursor>

ImageViewer::ImageViewer(QWidget *parent, QString title) : QWidget(parent), title(title), scaleFactor(1.0), isDragging(false) {
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);  // 使滚动区域可调整大小
    label = new QLabel(scrollArea);
    label->setAlignment(Qt::AlignCenter);
    label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);  // 设置为Ignored以填充整个label
    label->setScaledContents(true);  // 设置为true以填充整个label
    label->setAlignment(Qt::AlignCenter);
    label->setText(title);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(scrollArea);
    setLayout(layout);
    setWindowTitle(title);
    // setMinimumSize(800, 600);  // 设置窗口最小大小

    // 启用鼠标滚轮事件
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    installEventFilter(this);
}

void ImageViewer::setImage(const QPixmap &pixmap) {
    originalPixmap = pixmap;
    updateImageDisplay(true);
}

void ImageViewer::updateImageDisplay(bool isInit) {
    if(originalPixmap.isNull())
    {
        return;
    }
    if(isInit)
    {
        scaleFactor = qMin((static_cast<double>(size().width()) - 20) / static_cast<double>(originalPixmap.width()), (static_cast<double>(size().height()) - 20) / static_cast<double>(originalPixmap.height()));
        QPixmap InitPixmap = originalPixmap.scaled(originalPixmap.size() * scaleFactor, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        label->setPixmap(InitPixmap);
        label->adjustSize();
        QPointF centerOffset = QPointF((size().width() - 20 - label->size().width()) / 2, (size().height() - 20 - label->size().height()) / 2); // 计算中心偏移
        label->move(centerOffset.toPoint());
        return;
    }
    QPixmap scaledPixmap = originalPixmap.scaled(originalPixmap.size() * scaleFactor, Qt::KeepAspectRatio, Qt::SmoothTransformation);  // 根据scaleFactor调整大小
    label->setPixmap(scaledPixmap);
    label->adjustSize();
}

bool ImageViewer::eventFilter(QObject *obj, QEvent *event) {
    // 处理鼠标滚轮事件以缩放图片
    if (event->type() == QEvent::Wheel) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
        double oldScaleFactor = scaleFactor;
        if (wheelEvent->angleDelta().y() > 0) {
            scaleFactor = qMin(scaleFactor * 1.1, 10.0);  // 放大10%，不超过10
        } else {
            scaleFactor = qMax(scaleFactor * 0.9, 0.1);  // 缩小10%，不低于0.1倍
        }
        QPointF cursorPos = mapFromGlobal(QCursor::pos()); // 获取全局鼠标位置并映射到本地
        QPointF labelPos = label->pos(); // 获取标签当前位置
        QPointF relPos = cursorPos - labelPos; // 计算鼠标位置相对于标签的位置
        QPointF newRelPos = relPos * scaleFactor / oldScaleFactor; // 根据缩放因子调整相对位置
        QPointF newLabelPos = cursorPos - newRelPos; // 计算新的标签位置
        label->move(newLabelPos.toPoint()); // 移动标签到新位置
        updateImageDisplay(false); // 更新图片显示
        return true;
    } else if (event->type() == QEvent::Resize) { // 处理窗口大小调整事件
        updateImageDisplay(false); // 窗口调整时更新图片显示
    } else if (event->type() == QEvent::MouseButtonPress) { // 处理鼠标按下事件
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            dragStartPosition = mouseEvent->pos(); // 记录拖拽开始位置
            isDragging = true; // 设置拖拽状态为真
        }
    } else if (event->type() == QEvent::MouseButtonDblClick) { // 处理鼠标双击事件
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            scaleFactor = 1.0; // 双击时重置缩放因子
            updateImageDisplay(true); // 更新显示
            return true;
        }
    } else if (event->type() == QEvent::MouseMove) { // 处理鼠标移动事件
        if (isDragging) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            QPoint moveDistance = mouseEvent->pos() - dragStartPosition; // 计算移动距离
            if (!moveDistance.isNull()) {
                QPoint newLabelPos = label->pos() + moveDistance; // 计算新的标签位置
                label->move(newLabelPos); // 移动标签
                dragStartPosition = mouseEvent->pos(); // 更新拖拽开始位置
                return true;
            }
        }
    } else if (event->type() == QEvent::MouseButtonRelease) {
        isDragging = false;
    }
    return QWidget::eventFilter(obj, event);
}
