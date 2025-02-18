/********************************************************************************
** Form generated from reading UI file 'pcbcomponentsdetect.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PCBCOMPONENTSDETECT_H
#define UI_PCBCOMPONENTSDETECT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PCBComponentsDetect
{
public:
    QHBoxLayout *horizontalLayout;
    QWidget *widget;
    QGridLayout *gridLayout;
    QPushButton *pbcancle;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *pbsave;
    QSpacerItem *horizontalSpacer_3;
    QHBoxLayout *horizontalLayout_image;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout_label;

    void setupUi(QWidget *PCBComponentsDetect)
    {
        if (PCBComponentsDetect->objectName().isEmpty())
            PCBComponentsDetect->setObjectName("PCBComponentsDetect");
        PCBComponentsDetect->resize(940, 701);
        horizontalLayout = new QHBoxLayout(PCBComponentsDetect);
        horizontalLayout->setObjectName("horizontalLayout");
        widget = new QWidget(PCBComponentsDetect);
        widget->setObjectName("widget");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy);
        gridLayout = new QGridLayout(widget);
        gridLayout->setObjectName("gridLayout");
        pbcancle = new QPushButton(widget);
        pbcancle->setObjectName("pbcancle");

        gridLayout->addWidget(pbcancle, 1, 3, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer, 1, 0, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 1, 2, 1, 1);

        pbsave = new QPushButton(widget);
        pbsave->setObjectName("pbsave");

        gridLayout->addWidget(pbsave, 1, 1, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_3, 1, 4, 1, 1);

        horizontalLayout_image = new QHBoxLayout();
        horizontalLayout_image->setObjectName("horizontalLayout_image");

        gridLayout->addLayout(horizontalLayout_image, 0, 0, 1, 5);


        horizontalLayout->addWidget(widget);

        scrollArea = new QScrollArea(PCBComponentsDetect);
        scrollArea->setObjectName("scrollArea");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(scrollArea->sizePolicy().hasHeightForWidth());
        scrollArea->setSizePolicy(sizePolicy1);
        scrollArea->setMinimumSize(QSize(150, 0));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 148, 681));
        verticalLayout_2 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_label = new QVBoxLayout();
        verticalLayout_label->setObjectName("verticalLayout_label");

        verticalLayout_2->addLayout(verticalLayout_label);

        scrollArea->setWidget(scrollAreaWidgetContents);

        horizontalLayout->addWidget(scrollArea);


        retranslateUi(PCBComponentsDetect);

        QMetaObject::connectSlotsByName(PCBComponentsDetect);
    } // setupUi

    void retranslateUi(QWidget *PCBComponentsDetect)
    {
        PCBComponentsDetect->setWindowTitle(QCoreApplication::translate("PCBComponentsDetect", "Form", nullptr));
        pbcancle->setText(QCoreApplication::translate("PCBComponentsDetect", "\345\205\263\351\227\255", nullptr));
        pbsave->setText(QCoreApplication::translate("PCBComponentsDetect", "\346\213\215\346\221\204", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PCBComponentsDetect: public Ui_PCBComponentsDetect {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PCBCOMPONENTSDETECT_H
