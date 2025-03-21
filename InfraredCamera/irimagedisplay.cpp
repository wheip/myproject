#include "irimagedisplay.h"
#include <QVBoxLayout>
#include <QDebug>
#include <QSizePolicy>
#include <QWidget>

IRImageDisplay::IRImageDisplay(QWidget *parent) : QWidget(parent) {
    displayLabel = new QLabel(this);
    displayLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    displayLabel->setAlignment(Qt::AlignCenter);
    
    // 创建温度标签
    maxTempLabel = new QLabel(this);
    maxTempLabel->hide();
    maxTempLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    
    minTempLabel = new QLabel(this);
    minTempLabel->hide();
    minTempLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    
    centerTempLabel = new QLabel(this);
    centerTempLabel->hide();
    centerTempLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    
    // 设置标签样式和大小策略
    QString labelStyle = "QLabel { background-color: rgba(0, 0, 0, 128); color: white; padding: 5px; border-radius: 3px; }";
    maxTempLabel->setStyleSheet(labelStyle);
    minTempLabel->setStyleSheet(labelStyle);
    centerTempLabel->setStyleSheet(labelStyle);
    
    // 设置固定宽度和自动调整高度
    maxTempLabel->setMinimumWidth(150);
    minTempLabel->setMinimumWidth(150);
    centerTempLabel->setMinimumWidth(150);
    
    maxTempLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    minTempLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    centerTempLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    
    // 设置标签位置到左上角，增加间距
    const int MARGIN_LEFT = 10;
    const int MARGIN_TOP = 10;
    const int LABEL_SPACING = 35;
    
    maxTempLabel->move(MARGIN_LEFT, MARGIN_TOP);
    minTempLabel->move(MARGIN_LEFT, MARGIN_TOP + LABEL_SPACING);
    centerTempLabel->move(MARGIN_LEFT, MARGIN_TOP + 2 * LABEL_SPACING);
    
    this->setWindowTitle("IR Image Display");
    this->setMinimumSize(768, 576);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    connect(&timer, &QTimer::timeout, this, &IRImageDisplay::Imageupdate);
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(displayLabel);
    this->setLayout(layout);
}

IRImageDisplay::~IRImageDisplay() {
    delete displayLabel;
    delete maxTempLabel;
    delete minTempLabel;
    delete centerTempLabel;
}

void IRImageDisplay::GetMousePosition(int &tempX, int &tempY, int &mouseX, int &mouseY) {
    QPoint pos = QCursor::pos(); // 获取鼠标的全局位置
    if (displayLabel->underMouse()) {
        QPoint labelPos = displayLabel->mapFromGlobal(pos); // 获取鼠标在label_image中的相对位置
        mouseX = static_cast<int>(labelPos.x());
        mouseY = static_cast<int>(labelPos.y());
        // 获取label_image中显示的图片
        QPixmap pixmap = displayLabel->pixmap();
        if (!pixmap.isNull()) {
            // 获取图片的尺寸
            QSize imageSize = pixmap.size();
            QSize labelSize = displayLabel->size();

            // 计算缩放比例
            qreal scaleX = labelPos.x() / static_cast<qreal>(labelSize.width());
            qreal scaleY = labelPos.y() / static_cast<qreal>(labelSize.height());

            // 计算相对坐标
            tempX = static_cast<int>(tempWidth * scaleX);
            tempY = static_cast<int>(tempHeight * scaleY);
        }
    }
}

void IRImageDisplay::setImage(const QImage& image, const std::vector<uint16_t>& tempData, uint32_t tempWidth, uint32_t tempHeight) {
    this->image = image;
    this->tempData.clear();
    this->tempData.assign(tempData.begin(), tempData.end());
    this->tempWidth = tempWidth;
    this->tempHeight = tempHeight;
    timer.start(100);
    updateTempLabels(); // 更新温度标签
}

float IRImageDisplay::getTemp(int x, int y) {
    if (x >= 0 && x < tempWidth && y >= 0 && y < tempHeight) {
        return 0.1f * tempData[y * tempWidth + x];
    }
    return 0.0f;
}

void IRImageDisplay::updateTempLabels() {
    if (tempData.empty() || tempWidth == 0 || tempHeight == 0) {
        maxTempLabel->hide();
        minTempLabel->hide();
        centerTempLabel->hide();
        return;
    }
    
    // 查找最高和最低温度
    float maxTemp = -273.15f;
    float minTemp = 1000.0f;
    for (const auto& temp : tempData) {
        float currentTemp = 0.1f * temp;
        maxTemp = std::max(maxTemp, currentTemp);
        minTemp = std::min(minTemp, currentTemp);
    }
    
    // 计算中心温度
    int centerX = tempWidth / 2;
    int centerY = tempHeight / 2;
    float centerTemp = getTemp(centerX, centerY);
    
    // 更新标签文本
    maxTempLabel->setText(tr("最高温度: %1 °C").arg(QString::number(maxTemp, 'f', 1)));
    minTempLabel->setText(tr("最低温度: %1 °C").arg(QString::number(minTemp, 'f', 1)));
    centerTempLabel->setText(tr("中心温度: %1 °C").arg(QString::number(centerTemp, 'f', 1)));
    
    // 调整标签大小以适应内容
    maxTempLabel->adjustSize();
    minTempLabel->adjustSize();
    centerTempLabel->adjustSize();
    
    // 更新标签位置，使用相同的间距
    const int MARGIN_LEFT = 10;
    const int MARGIN_TOP = 10;
    const int LABEL_SPACING = 35;
    
    maxTempLabel->move(MARGIN_LEFT, MARGIN_TOP);
    minTempLabel->move(MARGIN_LEFT, MARGIN_TOP + LABEL_SPACING);
    centerTempLabel->move(MARGIN_LEFT, MARGIN_TOP + 2 * LABEL_SPACING);
    
    // 显示标签
    maxTempLabel->show();
    minTempLabel->show();
    centerTempLabel->show();
}

void IRImageDisplay::Imageupdate() {
    int tempX = 0, tempY = 0, mouseX = 0, mouseY = 0;
    GetMousePosition(tempX, tempY, mouseX, mouseY);
    if (tempX < 0 || tempX >= tempWidth || tempY < 0 || tempY >= tempHeight) {
        return;
    }
    float displayTemp = 0.1 * this->tempData[tempY * tempWidth + tempX];
    QString tempText = QString::number(displayTemp, 'f', 2) + " °C";
    QSize image_size = displayLabel->size();
    QPixmap pixmap = QPixmap::fromImage(image).scaled(image_size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    
    if(image_size == displayLabel->size())
    {
        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::TextAntialiasing);
        
        // 如果需要标记最高温度点
        if (isMarkMaxTemp && !tempData.empty()) {
            // 找到最高温度点
            auto maxTempIt = std::max_element(tempData.begin(), tempData.end());
            int maxTempIndex = std::distance(tempData.begin(), maxTempIt);
            int maxTempY = maxTempIndex / tempWidth;
            int maxTempX = maxTempIndex % tempWidth;
            
            // 计算在显示图像上的坐标
            float scaleX = static_cast<float>(image_size.width()) / tempWidth;
            float scaleY = static_cast<float>(image_size.height()) / tempHeight;
            
            int displayX = static_cast<int>(maxTempX * scaleX);
            int displayY = static_cast<int>(maxTempY * scaleY);
            
            // 绘制十字光标
            const int crossSize = 10;
            painter.setPen(QPen(Qt::red, 2));
            painter.drawLine(displayX - crossSize, displayY, displayX + crossSize, displayY);
            painter.drawLine(displayX, displayY - crossSize, displayX, displayY + crossSize);
            
            // 绘制温度值
            float maxTemp = 0.1f * (*maxTempIt);
            QString tempText = QString::number(maxTemp, 'f', 1) + "°C";
            
            // 计算文本位置
            QFontMetrics fm(painter.font());
            int textWidth = fm.horizontalAdvance(tempText);
            int textHeight = fm.height();
            
            // 绘制背景矩形
            QRect textRect(displayX + 5, displayY - textHeight - 5, textWidth + 10, textHeight + 10);
            painter.fillRect(textRect, QColor(0, 0, 0, 128));
            
            // 绘制温度文本
            painter.setPen(Qt::red);
            painter.drawText(displayX + 10, displayY - 5, tempText);
        }

        // 计算温度显示的矩形区域
        const int rectWidth = 80;
        const int rectHeight = 20;
        const int xOffset = 10; // 向右偏移量
        const int MARGIN = 10;  // 边缘安全距离
        
        // 默认显示在鼠标右侧
        int rectX = mouseX + xOffset;
        int rectY = mouseY - rectHeight/2;
        
        // 检查是否超出右边界
        if (rectX + rectWidth + MARGIN > image_size.width()) {
            // 如果超出右边界，显示在鼠标左侧
            rectX = mouseX - rectWidth - xOffset;
        }

        // 检查是否与标签重叠
        QRect tempRect(rectX, rectY, rectWidth, rectHeight);
        QRect maxLabelRect = maxTempLabel->geometry();
        QRect minLabelRect = minTempLabel->geometry();
        QRect centerLabelRect = centerTempLabel->geometry();
        
        if (tempRect.intersects(maxLabelRect) || 
            tempRect.intersects(minLabelRect) || 
            tempRect.intersects(centerLabelRect)) {
            // 如果与标签重叠，显示在最下面一个标签的下方
            const int MARGIN_LEFT = 10;
            const int MARGIN_TOP = 10;
            const int LABEL_SPACING = 35;
            rectY = MARGIN_TOP + 3 * LABEL_SPACING + 5; // 最后一个标签下方5像素
            rectX = MARGIN_LEFT; // 与标签对齐
        }
        
        // 绘制半透明背景
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 0, 0, 128));
        painter.drawRect(QRect(rectX, rectY, rectWidth, rectHeight));
        
        // 绘制温度文本
        painter.setPen(QPen(Qt::green, 2));
        painter.drawText(QPoint(rectX + 5, rectY + rectHeight - 5), tempText);
        
        displayLabel->setPixmap(pixmap);
    }
}

void IRImageDisplay::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    
    QSize size = event->size();
    int w = size.width();
    int h = size.height();
    
    if (w * 288 > h * 384) {
        w = h * 384 / 288;
    } else {
        h = w * 288 / 384;
    }
    
    // 更新标签位置到左上角，使用相同的间距
    const int MARGIN_LEFT = 10;
    const int MARGIN_TOP = 10;
    const int LABEL_SPACING = 35;
    
    maxTempLabel->move(MARGIN_LEFT, MARGIN_TOP);
    minTempLabel->move(MARGIN_LEFT, MARGIN_TOP + LABEL_SPACING);
    centerTempLabel->move(MARGIN_LEFT, MARGIN_TOP + 2 * LABEL_SPACING);
    
    QWidget::resize(w, h);
}

void IRImageDisplay::clear() {
    this->image = QImage();
    this->tempData.clear();
    this->tempWidth = 0;
    this->tempHeight = 0;
    displayLabel->clear();
    maxTempLabel->hide();
    minTempLabel->hide();
    centerTempLabel->hide();
    timer.stop();
}

void IRImageDisplay::MarkMaxTemp(bool isMark) {
    isMarkMaxTemp = isMark;
}