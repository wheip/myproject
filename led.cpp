#include "led.h"
#include <QPainter>

LED::LED(QWidget *parent, QString text) : QLabel(parent), led_text(text)
{
    led_status = "off";
    // 设置LED为长方形
    setMinimumSize(50, 70);  // 增加最小尺寸以适应更大的字体
}

LED::~LED(){}

void LED::set_led_status(QString status)
{
    led_status = status;
    update();
}

QString LED::get_led_status()
{
    return led_status;
}

void LED::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);

    // 绘制LED圆形指示灯
    int radius = 15; // 半径设置为15
    QPoint center(25, 25); // 圆心设置在上半部分的中心，调整为新的中心位置
    if(led_status == "finished")
    {
        painter.setBrush(Qt::green);
    }
    else if(led_status == "running")
    {
        painter.setBrush(Qt::red);
    }
    else
    {
        painter.setBrush(Qt::gray);
    }
    painter.drawEllipse(center, radius, radius);

    // 绘制文本标签
    QRect textRect(0, 45, 50, 25); // 调整文本区域的位置和大小以适应更大的字体
    painter.setPen(Qt::black);
    painter.drawText(textRect, Qt::AlignCenter, led_text);
}
