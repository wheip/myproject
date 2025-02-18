#ifndef VISIOVIEW_H
#define VISIOVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>
#include <QMouseEvent>

class VisioView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit VisioView(QWidget *parent = nullptr);

protected:
    // 按住 Ctrl 键滚动鼠标，实现缩放
    void wheelEvent(QWheelEvent *event) override;
    // 左键或中键拖动实现平移
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    bool panning;
    QPoint lastPanPoint;
    Qt::MouseButton panButton;  // 记录当前是用哪个按钮在平移
};

#endif // VISIOVIEW_H 