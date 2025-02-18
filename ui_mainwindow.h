/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionPCBshibie;
    QAction *actionPCB;
    QAction *actiondevicemanage;
    QAction *actiontesttaskmanage;
    QAction *actiontaskmanage;
    QAction *actiontest;
    QAction *actionautodetect;
    QAction *actiontyi;
    QAction *actionlkjfslj;
    QAction *actionjty;
    QWidget *centralwidget;
    QGridLayout *gridLayout_3;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_2;
    QGridLayout *display;
    QStatusBar *statusbar;
    QMenuBar *menubar;
    QMenu *menu;
    QMenu *menu_selectdevice;
    QMenu *menulistselect;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(2168, 1075);
        QFont font;
        font.setPointSize(14);
        MainWindow->setFont(font);
        actionPCBshibie = new QAction(MainWindow);
        actionPCBshibie->setObjectName("actionPCBshibie");
        actionPCB = new QAction(MainWindow);
        actionPCB->setObjectName("actionPCB");
        actionPCB->setFont(font);
        actiondevicemanage = new QAction(MainWindow);
        actiondevicemanage->setObjectName("actiondevicemanage");
        actiontesttaskmanage = new QAction(MainWindow);
        actiontesttaskmanage->setObjectName("actiontesttaskmanage");
        actiontaskmanage = new QAction(MainWindow);
        actiontaskmanage->setObjectName("actiontaskmanage");
        actiontest = new QAction(MainWindow);
        actiontest->setObjectName("actiontest");
        actionautodetect = new QAction(MainWindow);
        actionautodetect->setObjectName("actionautodetect");
        actiontyi = new QAction(MainWindow);
        actiontyi->setObjectName("actiontyi");
        actionlkjfslj = new QAction(MainWindow);
        actionlkjfslj->setObjectName("actionlkjfslj");
        actionjty = new QAction(MainWindow);
        actionjty->setObjectName("actionjty");
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        gridLayout_3 = new QGridLayout(centralwidget);
        gridLayout_3->setObjectName("gridLayout_3");
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName("groupBox");
        gridLayout_2 = new QGridLayout(groupBox);
        gridLayout_2->setObjectName("gridLayout_2");
        display = new QGridLayout();
        display->setObjectName("display");

        gridLayout_2->addLayout(display, 0, 0, 1, 1);


        gridLayout_3->addWidget(groupBox, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 2168, 24));
        menu = new QMenu(menubar);
        menu->setObjectName("menu");
        menu->setFont(font);
        menu_selectdevice = new QMenu(menubar);
        menu_selectdevice->setObjectName("menu_selectdevice");
        menulistselect = new QMenu(menu_selectdevice);
        menulistselect->setObjectName("menulistselect");
        MainWindow->setMenuBar(menubar);

        menubar->addAction(menu->menuAction());
        menubar->addAction(menu_selectdevice->menuAction());
        menu->addAction(actionPCB);
        menu->addAction(actiondevicemanage);
        menu->addAction(actiontesttaskmanage);
        menu->addAction(actiontaskmanage);
        menu->addAction(actiontest);
        menu_selectdevice->addAction(actionautodetect);
        menu_selectdevice->addAction(menulistselect->menuAction());

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        actionPCBshibie->setText(QCoreApplication::translate("MainWindow", "PCBshibie", nullptr));
        actionPCB->setText(QCoreApplication::translate("MainWindow", "PCB\350\257\206\345\210\253", nullptr));
        actiondevicemanage->setText(QCoreApplication::translate("MainWindow", "\350\242\253\346\265\213\350\256\276\345\244\207\347\256\241\347\220\206 ", nullptr));
        actiontesttaskmanage->setText(QCoreApplication::translate("MainWindow", "\346\265\213\350\257\225\344\273\273\345\212\241\347\256\241\347\220\206", nullptr));
        actiontaskmanage->setText(QCoreApplication::translate("MainWindow", "\344\273\273\345\212\241\346\211\247\350\241\214\344\270\216\346\237\245\347\234\213", nullptr));
        actiontest->setText(QCoreApplication::translate("MainWindow", "\346\265\213\350\257\225", nullptr));
        actionautodetect->setText(QCoreApplication::translate("MainWindow", "\350\207\252\345\212\250\350\257\206\345\210\253", nullptr));
        actiontyi->setText(QCoreApplication::translate("MainWindow", "tyi", nullptr));
        actionlkjfslj->setText(QCoreApplication::translate("MainWindow", "lkjfslj", nullptr));
        actionjty->setText(QCoreApplication::translate("MainWindow", "jty", nullptr));
        groupBox->setTitle(QString());
        menu->setTitle(QCoreApplication::translate("MainWindow", "\350\217\234\345\215\225", nullptr));
        menu_selectdevice->setTitle(QCoreApplication::translate("MainWindow", "\351\200\211\346\213\251\350\242\253\346\265\213\350\256\276\345\244\207", nullptr));
        menulistselect->setTitle(QCoreApplication::translate("MainWindow", "\345\210\227\350\241\250\351\200\211\346\213\251", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
