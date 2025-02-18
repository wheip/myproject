#include "VisioView.h"
#include <QScrollBar>
#include <cmath>

VisioView::VisioView(QWidget *parent) 
    : QGraphicsView(parent), panning(false), panButton(Qt::NoButton)
{
    // 启用抗锯齿效果，使图形显示更平滑
    setRenderHint(QPainter::Antialiasing);
    
    // 设置视口更新模式使得拖动时能及时重绘
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    
    // 始终显示滚动条，方便查看位置
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    
    // 设置变换锚点为视图中心（这样缩放时比较平滑）
    setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    setResizeAnchor(QGraphicsView::AnchorViewCenter);
    
    // 采用无拖拽模式，这样左键事件不会被默认的橡皮筋拖拽拦截
    setDragMode(QGraphicsView::NoDrag);
    
    // 其他设置（如优化和鼠标追踪）根据需要设置
    setMouseTracking(true);
}

void VisioView::wheelEvent(QWheelEvent *event)
{
    // 如果 Ctrl 被按下，则进行缩放
    if (event->modifiers() & Qt::ControlModifier) {
        const double scaleFactor = 1.15; // 缩放因子
        if (event->angleDelta().y() > 0) {
            scale(scaleFactor, scaleFactor);
        } else {
            scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        }
        event->accept();
    } else {
        QGraphicsView::wheelEvent(event);
    }
}

void VisioView::mousePressEvent(QMouseEvent *event)
{
    // 如果是左键点击在空白区域，或中键点击，都启动平移
    if ((event->button() == Qt::LeftButton &&
         !scene()->itemAt(mapToScene(event->pos()), transform()))
        || event->button() == Qt::MiddleButton)
    {
        panning = true;
        panButton = event->button();
        lastPanPoint = event->pos();
        viewport()->setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }
    QGraphicsView::mousePressEvent(event);
}

void VisioView::mouseMoveEvent(QMouseEvent *event)
{
    if (panning) {
        // 计算鼠标移动的像素差，然后调整水平、垂直滚动条
        int dx = event->pos().x() - lastPanPoint.x();
        int dy = event->pos().y() - lastPanPoint.y();
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - dx);
        verticalScrollBar()->setValue(verticalScrollBar()->value() - dy);
        lastPanPoint = event->pos();
        event->accept();
        return;
    }
    QGraphicsView::mouseMoveEvent(event);
}

void VisioView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == panButton) {
        panning = false;
        panButton = Qt::NoButton;
        viewport()->setCursor(Qt::ArrowCursor);
        event->accept();
        return;
    }
    QGraphicsView::mouseReleaseEvent(event);
} 