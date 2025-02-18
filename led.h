#ifndef LED_H
#define LED_H
#include <QLabel>

class LED : public QLabel
{
    Q_OBJECT
public:
    LED(QWidget *parent = nullptr, QString text = "");
    ~LED();

    void set_led_status(QString status);
    QString get_led_status();

private:
    QString led_status;
    QString led_text;
    void paintEvent(QPaintEvent *event) override;
};

#endif // LED_H
