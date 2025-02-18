/********************************************************************************
** Form generated from reading UI file 'camera.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CAMERA_H
#define UI_CAMERA_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Camera
{
public:
    QGridLayout *gridLayout;
    QPushButton *pbsave;
    QLabel *label_image;
    QPushButton *pbcanael;
    QPushButton *pbformatconversion;

    void setupUi(QWidget *Camera)
    {
        if (Camera->objectName().isEmpty())
            Camera->setObjectName("Camera");
        Camera->resize(400, 300);
        gridLayout = new QGridLayout(Camera);
        gridLayout->setObjectName("gridLayout");
        pbsave = new QPushButton(Camera);
        pbsave->setObjectName("pbsave");

        gridLayout->addWidget(pbsave, 1, 0, 1, 1);

        label_image = new QLabel(Camera);
        label_image->setObjectName("label_image");

        gridLayout->addWidget(label_image, 0, 0, 1, 3);

        pbcanael = new QPushButton(Camera);
        pbcanael->setObjectName("pbcanael");

        gridLayout->addWidget(pbcanael, 1, 2, 1, 1);

        pbformatconversion = new QPushButton(Camera);
        pbformatconversion->setObjectName("pbformatconversion");

        gridLayout->addWidget(pbformatconversion, 1, 1, 1, 1);


        retranslateUi(Camera);

        QMetaObject::connectSlotsByName(Camera);
    } // setupUi

    void retranslateUi(QWidget *Camera)
    {
        Camera->setWindowTitle(QCoreApplication::translate("Camera", "Form", nullptr));
        pbsave->setText(QCoreApplication::translate("Camera", "\346\213\215\346\221\204", nullptr));
        label_image->setText(QCoreApplication::translate("Camera", "\350\257\267\350\277\236\346\216\245\346\221\204\345\203\217\345\244\264", nullptr));
        pbcanael->setText(QCoreApplication::translate("Camera", "\345\205\263\351\227\255", nullptr));
        pbformatconversion->setText(QCoreApplication::translate("Camera", "JPG", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Camera: public Ui_Camera {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CAMERA_H
