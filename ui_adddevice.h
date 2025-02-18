/********************************************************************************
** Form generated from reading UI file 'adddevice.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADDDEVICE_H
#define UI_ADDDEVICE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Adddevice
{
public:
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *label;
    QLineEdit *lineEdit_id;
    QLabel *label_2;
    QLineEdit *lineEdit_name;
    QLabel *label_6;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *lineEdit_image;
    QPushButton *pbcamera;
    QPushButton *pushButton_selectFile;
    QHBoxLayout *horizontalLayout;
    QPushButton *pbadddevice;
    QPushButton *pdcancel;

    void setupUi(QWidget *Adddevice)
    {
        if (Adddevice->objectName().isEmpty())
            Adddevice->setObjectName("Adddevice");
        Adddevice->resize(410, 140);
        verticalLayout = new QVBoxLayout(Adddevice);
        verticalLayout->setObjectName("verticalLayout");
        formLayout = new QFormLayout();
        formLayout->setObjectName("formLayout");
        label = new QLabel(Adddevice);
        label->setObjectName("label");

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        lineEdit_id = new QLineEdit(Adddevice);
        lineEdit_id->setObjectName("lineEdit_id");

        formLayout->setWidget(0, QFormLayout::FieldRole, lineEdit_id);

        label_2 = new QLabel(Adddevice);
        label_2->setObjectName("label_2");

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        lineEdit_name = new QLineEdit(Adddevice);
        lineEdit_name->setObjectName("lineEdit_name");

        formLayout->setWidget(1, QFormLayout::FieldRole, lineEdit_name);

        label_6 = new QLabel(Adddevice);
        label_6->setObjectName("label_6");

        formLayout->setWidget(2, QFormLayout::LabelRole, label_6);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        lineEdit_image = new QLineEdit(Adddevice);
        lineEdit_image->setObjectName("lineEdit_image");

        horizontalLayout_2->addWidget(lineEdit_image);

        pbcamera = new QPushButton(Adddevice);
        pbcamera->setObjectName("pbcamera");

        horizontalLayout_2->addWidget(pbcamera);

        pushButton_selectFile = new QPushButton(Adddevice);
        pushButton_selectFile->setObjectName("pushButton_selectFile");

        horizontalLayout_2->addWidget(pushButton_selectFile);


        formLayout->setLayout(2, QFormLayout::FieldRole, horizontalLayout_2);


        verticalLayout->addLayout(formLayout);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        pbadddevice = new QPushButton(Adddevice);
        pbadddevice->setObjectName("pbadddevice");

        horizontalLayout->addWidget(pbadddevice);

        pdcancel = new QPushButton(Adddevice);
        pdcancel->setObjectName("pdcancel");

        horizontalLayout->addWidget(pdcancel);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(Adddevice);

        QMetaObject::connectSlotsByName(Adddevice);
    } // setupUi

    void retranslateUi(QWidget *Adddevice)
    {
        Adddevice->setWindowTitle(QCoreApplication::translate("Adddevice", "Form", nullptr));
        label->setText(QCoreApplication::translate("Adddevice", "ID:", nullptr));
        lineEdit_id->setPlaceholderText(QCoreApplication::translate("Adddevice", "\350\257\267\350\276\223\345\205\245\350\256\276\345\244\207ID", nullptr));
        label_2->setText(QCoreApplication::translate("Adddevice", "\350\256\276\345\244\207\345\220\215\347\247\260\357\274\232", nullptr));
        lineEdit_name->setPlaceholderText(QCoreApplication::translate("Adddevice", "\350\257\267\350\276\223\345\205\245\350\256\276\345\244\207\345\220\215\347\247\260", nullptr));
        label_6->setText(QCoreApplication::translate("Adddevice", "\345\233\276\347\211\207\350\267\257\345\276\204\357\274\232", nullptr));
        lineEdit_image->setPlaceholderText(QCoreApplication::translate("Adddevice", "\350\257\267\351\200\211\346\213\251\350\256\276\345\244\207\347\205\247\347\211\207", nullptr));
        pbcamera->setText(QCoreApplication::translate("Adddevice", "\346\213\215\346\221\204", nullptr));
        pushButton_selectFile->setText(QCoreApplication::translate("Adddevice", "\346\265\217\350\247\210", nullptr));
        pbadddevice->setText(QCoreApplication::translate("Adddevice", "\346\267\273\345\212\240", nullptr));
        pdcancel->setText(QCoreApplication::translate("Adddevice", "\345\217\226\346\266\210", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Adddevice: public Ui_Adddevice {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADDDEVICE_H
