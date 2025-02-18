#include "mainwindow.h"
#include <QDebug>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyleSheet("QWidget { font-size: 14pt; }");
    MainWindow w;
    w.show();
    return a.exec();
}
