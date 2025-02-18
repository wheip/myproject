#ifndef IRIMAGEDISPLAY_H
#define IRIMAGEDISPLAY_H

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QPen>
#include <QString>
#include <QLabel>
#include <QPoint>
#include <QSize>
#include <QPixmap>
#include <QTimer>
#include <QResizeEvent>

class IRImageDisplay : public QWidget
{
    Q_OBJECT
public:
    IRImageDisplay(QWidget *parent = nullptr);
    ~IRImageDisplay();

    void setImage(const QImage& image, const std::vector<uint16_t>& tempData, uint32_t tempWidth, uint32_t tempHeight);

    void clear();

    void MarkMaxTemp(bool isMark = true);

private:
    QImage image;
    QTimer timer;
    std::vector<uint16_t> tempData;
    uint32_t tempWidth = 0;
    uint32_t tempHeight = 0;
    QLabel *displayLabel;
    QLabel *maxTempLabel;
    QLabel *minTempLabel;
    QLabel *centerTempLabel;
    bool isMarkMaxTemp = false;
    QPoint maxTempPoint;


    void GetMousePosition(int &tempX, int &tempY, int &mouseX, int &mouseY);
    void Imageupdate();
    void updateTempLabels();
    float getTemp(int x, int y);

protected:
    void resizeEvent(QResizeEvent *event) override;
};

#endif // IRIMAGEDISPLAY_H
