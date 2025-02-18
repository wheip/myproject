/********************************************************************************
** Form generated from reading UI file 'detectcamera.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DETECTCAMERA_H
#define UI_DETECTCAMERA_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DetectCamera
{
public:
    QGridLayout *gridLayout;
    QPushButton *pbsave;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *pbclose;
    QSpacerItem *horizontalSpacer_3;
    QLabel *label_image;

    void setupUi(QWidget *DetectCamera)
    {
        if (DetectCamera->objectName().isEmpty())
            DetectCamera->setObjectName("DetectCamera");
        DetectCamera->resize(400, 300);
        gridLayout = new QGridLayout(DetectCamera);
        gridLayout->setObjectName("gridLayout");
        pbsave = new QPushButton(DetectCamera);
        pbsave->setObjectName("pbsave");

        gridLayout->addWidget(pbsave, 1, 1, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer, 1, 2, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 1, 0, 1, 1);

        pbclose = new QPushButton(DetectCamera);
        pbclose->setObjectName("pbclose");

        gridLayout->addWidget(pbclose, 1, 3, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_3, 1, 4, 1, 1);

        label_image = new QLabel(DetectCamera);
        label_image->setObjectName("label_image");

        gridLayout->addWidget(label_image, 0, 0, 1, 5);


        retranslateUi(DetectCamera);

        QMetaObject::connectSlotsByName(DetectCamera);
    } // setupUi

    void retranslateUi(QWidget *DetectCamera)
    {
        DetectCamera->setWindowTitle(QCoreApplication::translate("DetectCamera", "Form", nullptr));
        pbsave->setText(QCoreApplication::translate("DetectCamera", "\346\213\215\346\221\204", nullptr));
        pbclose->setText(QCoreApplication::translate("DetectCamera", "\345\205\263\351\227\255", nullptr));
        label_image->setText(QCoreApplication::translate("DetectCamera", "\350\257\267\350\277\236\346\216\245\346\221\204\345\203\217\345\244\264", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DetectCamera: public Ui_DetectCamera {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DETECTCAMERA_H
