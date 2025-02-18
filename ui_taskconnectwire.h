/********************************************************************************
** Form generated from reading UI file 'taskconnectwire.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TASKCONNECTWIRE_H
#define UI_TASKCONNECTWIRE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TaskConnectWire
{
public:
    QHBoxLayout *horizontalLayout_2;
    QWidget *widget;
    QGridLayout *gridLayout_3;
    QGridLayout *gridLayout;
    QWidget *widget_2;
    QGridLayout *gridLayout_2;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QTextBrowser *textBrowser;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QPushButton *pbcomplete;
    QSpacerItem *horizontalSpacer;
    QPushButton *pbbegin;
    QSpacerItem *verticalSpacer_2;

    void setupUi(QWidget *TaskConnectWire)
    {
        if (TaskConnectWire->objectName().isEmpty())
            TaskConnectWire->setObjectName("TaskConnectWire");
        TaskConnectWire->resize(2053, 927);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(TaskConnectWire->sizePolicy().hasHeightForWidth());
        TaskConnectWire->setSizePolicy(sizePolicy);
        TaskConnectWire->setMinimumSize(QSize(0, 0));
        horizontalLayout_2 = new QHBoxLayout(TaskConnectWire);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        widget = new QWidget(TaskConnectWire);
        widget->setObjectName("widget");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy1);
        widget->setMinimumSize(QSize(1600, 0));
        gridLayout_3 = new QGridLayout(widget);
        gridLayout_3->setObjectName("gridLayout_3");
        gridLayout = new QGridLayout();
        gridLayout->setObjectName("gridLayout");

        gridLayout_3->addLayout(gridLayout, 0, 0, 1, 1);


        horizontalLayout_2->addWidget(widget);

        widget_2 = new QWidget(TaskConnectWire);
        widget_2->setObjectName("widget_2");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(widget_2->sizePolicy().hasHeightForWidth());
        widget_2->setSizePolicy(sizePolicy2);
        gridLayout_2 = new QGridLayout(widget_2);
        gridLayout_2->setObjectName("gridLayout_2");
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        textBrowser = new QTextBrowser(widget_2);
        textBrowser->setObjectName("textBrowser");
        QSizePolicy sizePolicy3(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Expanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(textBrowser->sizePolicy().hasHeightForWidth());
        textBrowser->setSizePolicy(sizePolicy3);
        textBrowser->setMinimumSize(QSize(407, 0));

        verticalLayout->addWidget(textBrowser);

        verticalSpacer = new QSpacerItem(20, 147, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);

        verticalLayout->addItem(verticalSpacer);


        verticalLayout_2->addLayout(verticalLayout);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        pbcomplete = new QPushButton(widget_2);
        pbcomplete->setObjectName("pbcomplete");

        horizontalLayout->addWidget(pbcomplete);

        horizontalSpacer = new QSpacerItem(89, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pbbegin = new QPushButton(widget_2);
        pbbegin->setObjectName("pbbegin");

        horizontalLayout->addWidget(pbbegin);


        verticalLayout_2->addLayout(horizontalLayout);

        verticalSpacer_2 = new QSpacerItem(20, 114, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);

        verticalLayout_2->addItem(verticalSpacer_2);


        gridLayout_2->addLayout(verticalLayout_2, 0, 0, 1, 1);


        horizontalLayout_2->addWidget(widget_2);

        horizontalLayout_2->setStretch(0, 3);
        horizontalLayout_2->setStretch(1, 1);

        retranslateUi(TaskConnectWire);

        QMetaObject::connectSlotsByName(TaskConnectWire);
    } // setupUi

    void retranslateUi(QWidget *TaskConnectWire)
    {
        TaskConnectWire->setWindowTitle(QCoreApplication::translate("TaskConnectWire", "Form", nullptr));
        pbcomplete->setText(QCoreApplication::translate("TaskConnectWire", "\345\267\262\350\277\236\346\216\245", nullptr));
        pbbegin->setText(QCoreApplication::translate("TaskConnectWire", "\345\274\200\345\247\213", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TaskConnectWire: public Ui_TaskConnectWire {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TASKCONNECTWIRE_H
