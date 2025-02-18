/********************************************************************************
** Form generated from reading UI file 'searchdevice.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SEARCHDEVICE_H
#define UI_SEARCHDEVICE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SearchDevice
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *lineEdit_id;
    QLabel *label_2;
    QLineEdit *lineEdit_name;
    QPushButton *pdsearch;
    QPushButton *pbadddevice;
    QTableWidget *tableWidget_devices;

    void setupUi(QWidget *SearchDevice)
    {
        if (SearchDevice->objectName().isEmpty())
            SearchDevice->setObjectName("SearchDevice");
        SearchDevice->resize(700, 376);
        verticalLayout = new QVBoxLayout(SearchDevice);
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        label = new QLabel(SearchDevice);
        label->setObjectName("label");

        horizontalLayout->addWidget(label);

        lineEdit_id = new QLineEdit(SearchDevice);
        lineEdit_id->setObjectName("lineEdit_id");

        horizontalLayout->addWidget(lineEdit_id);

        label_2 = new QLabel(SearchDevice);
        label_2->setObjectName("label_2");

        horizontalLayout->addWidget(label_2);

        lineEdit_name = new QLineEdit(SearchDevice);
        lineEdit_name->setObjectName("lineEdit_name");

        horizontalLayout->addWidget(lineEdit_name);

        pdsearch = new QPushButton(SearchDevice);
        pdsearch->setObjectName("pdsearch");

        horizontalLayout->addWidget(pdsearch);

        pbadddevice = new QPushButton(SearchDevice);
        pbadddevice->setObjectName("pbadddevice");

        horizontalLayout->addWidget(pbadddevice);


        verticalLayout->addLayout(horizontalLayout);

        tableWidget_devices = new QTableWidget(SearchDevice);
        tableWidget_devices->setObjectName("tableWidget_devices");

        verticalLayout->addWidget(tableWidget_devices);


        retranslateUi(SearchDevice);

        QMetaObject::connectSlotsByName(SearchDevice);
    } // setupUi

    void retranslateUi(QWidget *SearchDevice)
    {
        SearchDevice->setWindowTitle(QCoreApplication::translate("SearchDevice", "Form", nullptr));
        label->setText(QCoreApplication::translate("SearchDevice", "\350\256\276\345\244\207ID:", nullptr));
        label_2->setText(QCoreApplication::translate("SearchDevice", "\350\256\276\345\244\207\345\220\215\347\247\260\357\274\232", nullptr));
        pdsearch->setText(QCoreApplication::translate("SearchDevice", "\346\237\245\350\257\242", nullptr));
        pbadddevice->setText(QCoreApplication::translate("SearchDevice", "\346\267\273\345\212\240\350\256\276\345\244\207", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SearchDevice: public Ui_SearchDevice {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SEARCHDEVICE_H
