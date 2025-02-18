/********************************************************************************
** Form generated from reading UI file 'PXIe5711.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PXIE5711_H
#define UI_PXIE5711_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PXIe5711
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *waveformdisplay;

    void setupUi(QWidget *PXIe5711)
    {
        if (PXIe5711->objectName().isEmpty())
            PXIe5711->setObjectName("PXIe5711");
        PXIe5711->resize(400, 300);
        gridLayout = new QGridLayout(PXIe5711);
        gridLayout->setObjectName("gridLayout");
        waveformdisplay = new QVBoxLayout();
        waveformdisplay->setObjectName("waveformdisplay");

        gridLayout->addLayout(waveformdisplay, 0, 0, 1, 1);


        retranslateUi(PXIe5711);

        QMetaObject::connectSlotsByName(PXIe5711);
    } // setupUi

    void retranslateUi(QWidget *PXIe5711)
    {
        PXIe5711->setWindowTitle(QCoreApplication::translate("PXIe5711", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PXIe5711: public Ui_PXIe5711 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PXIE5711_H
