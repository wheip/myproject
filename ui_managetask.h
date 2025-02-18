/********************************************************************************
** Form generated from reading UI file 'managetask.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MANAGETASK_H
#define UI_MANAGETASK_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ManageTask
{
public:
    QGridLayout *gridLayout_44;
    QGridLayout *gridLayout_wireconnect;
    QWidget *widget_2;
    QGridLayout *gridLayout_9;
    QWidget *widget;
    QGridLayout *gridLayout_7;
    QHBoxLayout *horizontalLayout;
    QLabel *label_4;
    QComboBox *cbdeviceid;
    QPushButton *pblabelinfoshow;
    QPushButton *pbrefresh;
    QSpacerItem *horizontalSpacer;
    QLabel *label;
    QComboBox *comboBox_taskid;
    QPushButton *pdRuntask;
    QPushButton *pbnextstep;
    QPushButton *pbInterrupt;
    QPushButton *pbViewTask;
    QWidget *widget_imageshow;
    QHBoxLayout *horizontalLayout_6;
    QWidget *widget_btnimage;
    QGridLayout *gridLayout_11;
    QPushButton *showOrHideBtnimage;
    QGroupBox *groupBox_imageshow;
    QGridLayout *gridLayout_3;
    QWidget *widget_3;
    QGridLayout *gridLayout_8;
    QGridLayout *gridLayout_image;
    QWidget *widget_iconimage;
    QGridLayout *gridLayout;
    QGridLayout *gridLayout_iconimage;
    QGroupBox *groupBox_4;
    QHBoxLayout *horizontalLayout_2;
    QHBoxLayout *horizontalLayout_TaskProgress;
    QHBoxLayout *horizontalLayout_3;
    QWidget *containerWdget;
    QHBoxLayout *horizontalLayout_4;
    QWidget *ptzWidget;
    QHBoxLayout *horizontalLayout_5;
    QTreeWidget *treeWidget;
    QWidget *btnWidget;
    QGridLayout *gridLayout_10;
    QPushButton *showOrHideBtn;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_4;
    QGridLayout *gridLayout_8902;
    QPushButton *pb8902expand;
    QVBoxLayout *verticalLayout_8902scroll;
    QScrollBar *horizontalScrollBar_8902;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_5;
    QGridLayout *gridLayout_5322;
    QPushButton *pb5322expand;
    QVBoxLayout *verticalLayout_5322scroll;
    QScrollBar *horizontalScrollBar_5322;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_6;
    QPushButton *pb5323expand;
    QGridLayout *gridLayout_5323;
    QVBoxLayout *verticalLayout_5323scroll;
    QScrollBar *horizontalScrollBar_5323;

    void setupUi(QWidget *ManageTask)
    {
        if (ManageTask->objectName().isEmpty())
            ManageTask->setObjectName("ManageTask");
        ManageTask->resize(1725, 940);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ManageTask->sizePolicy().hasHeightForWidth());
        ManageTask->setSizePolicy(sizePolicy);
        gridLayout_44 = new QGridLayout(ManageTask);
        gridLayout_44->setObjectName("gridLayout_44");
        gridLayout_wireconnect = new QGridLayout();
        gridLayout_wireconnect->setObjectName("gridLayout_wireconnect");
        widget_2 = new QWidget(ManageTask);
        widget_2->setObjectName("widget_2");
        gridLayout_9 = new QGridLayout(widget_2);
        gridLayout_9->setObjectName("gridLayout_9");
        widget = new QWidget(widget_2);
        widget->setObjectName("widget");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy1);
        gridLayout_7 = new QGridLayout(widget);
        gridLayout_7->setObjectName("gridLayout_7");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        label_4 = new QLabel(widget);
        label_4->setObjectName("label_4");

        horizontalLayout->addWidget(label_4);

        cbdeviceid = new QComboBox(widget);
        cbdeviceid->setObjectName("cbdeviceid");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(cbdeviceid->sizePolicy().hasHeightForWidth());
        cbdeviceid->setSizePolicy(sizePolicy2);
        cbdeviceid->setMinimumSize(QSize(70, 0));
        cbdeviceid->setMaximumSize(QSize(70, 16777215));

        horizontalLayout->addWidget(cbdeviceid);

        pblabelinfoshow = new QPushButton(widget);
        pblabelinfoshow->setObjectName("pblabelinfoshow");
        sizePolicy2.setHeightForWidth(pblabelinfoshow->sizePolicy().hasHeightForWidth());
        pblabelinfoshow->setSizePolicy(sizePolicy2);
        pblabelinfoshow->setMinimumSize(QSize(146, 0));
        pblabelinfoshow->setMaximumSize(QSize(146, 16777215));

        horizontalLayout->addWidget(pblabelinfoshow);

        pbrefresh = new QPushButton(widget);
        pbrefresh->setObjectName("pbrefresh");

        horizontalLayout->addWidget(pbrefresh);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        label = new QLabel(widget);
        label->setObjectName("label");

        horizontalLayout->addWidget(label);

        comboBox_taskid = new QComboBox(widget);
        comboBox_taskid->setObjectName("comboBox_taskid");
        QSizePolicy sizePolicy3(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(comboBox_taskid->sizePolicy().hasHeightForWidth());
        comboBox_taskid->setSizePolicy(sizePolicy3);

        horizontalLayout->addWidget(comboBox_taskid);

        pdRuntask = new QPushButton(widget);
        pdRuntask->setObjectName("pdRuntask");

        horizontalLayout->addWidget(pdRuntask);

        pbnextstep = new QPushButton(widget);
        pbnextstep->setObjectName("pbnextstep");

        horizontalLayout->addWidget(pbnextstep);

        pbInterrupt = new QPushButton(widget);
        pbInterrupt->setObjectName("pbInterrupt");

        horizontalLayout->addWidget(pbInterrupt);

        pbViewTask = new QPushButton(widget);
        pbViewTask->setObjectName("pbViewTask");

        horizontalLayout->addWidget(pbViewTask);


        gridLayout_7->addLayout(horizontalLayout, 0, 0, 1, 1);


        gridLayout_9->addWidget(widget, 0, 0, 1, 1);

        widget_imageshow = new QWidget(widget_2);
        widget_imageshow->setObjectName("widget_imageshow");
        widget_imageshow->setMinimumSize(QSize(0, 0));
        widget_imageshow->setMaximumSize(QSize(16777215, 16777215));
        horizontalLayout_6 = new QHBoxLayout(widget_imageshow);
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        widget_btnimage = new QWidget(widget_imageshow);
        widget_btnimage->setObjectName("widget_btnimage");
        sizePolicy.setHeightForWidth(widget_btnimage->sizePolicy().hasHeightForWidth());
        widget_btnimage->setSizePolicy(sizePolicy);
        gridLayout_11 = new QGridLayout(widget_btnimage);
        gridLayout_11->setObjectName("gridLayout_11");
        showOrHideBtnimage = new QPushButton(widget_btnimage);
        showOrHideBtnimage->setObjectName("showOrHideBtnimage");
        sizePolicy2.setHeightForWidth(showOrHideBtnimage->sizePolicy().hasHeightForWidth());
        showOrHideBtnimage->setSizePolicy(sizePolicy2);
        showOrHideBtnimage->setMinimumSize(QSize(38, 104));
        showOrHideBtnimage->setMaximumSize(QSize(24, 85));
        QFont font;
        font.setPointSize(14);
        showOrHideBtnimage->setFont(font);

        gridLayout_11->addWidget(showOrHideBtnimage, 0, 0, 1, 1);


        horizontalLayout_6->addWidget(widget_btnimage);

        groupBox_imageshow = new QGroupBox(widget_imageshow);
        groupBox_imageshow->setObjectName("groupBox_imageshow");
        QSizePolicy sizePolicy4(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Preferred);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(groupBox_imageshow->sizePolicy().hasHeightForWidth());
        groupBox_imageshow->setSizePolicy(sizePolicy4);
        groupBox_imageshow->setMinimumSize(QSize(500, 0));
        QFont font1;
        font1.setPointSize(14);
        font1.setBold(true);
        font1.setItalic(false);
        font1.setUnderline(true);
        groupBox_imageshow->setFont(font1);
        gridLayout_3 = new QGridLayout(groupBox_imageshow);
        gridLayout_3->setObjectName("gridLayout_3");
        widget_3 = new QWidget(groupBox_imageshow);
        widget_3->setObjectName("widget_3");
        gridLayout_8 = new QGridLayout(widget_3);
        gridLayout_8->setObjectName("gridLayout_8");
        gridLayout_image = new QGridLayout();
        gridLayout_image->setObjectName("gridLayout_image");

        gridLayout_8->addLayout(gridLayout_image, 0, 0, 1, 1);


        gridLayout_3->addWidget(widget_3, 0, 0, 1, 1);

        widget_iconimage = new QWidget(groupBox_imageshow);
        widget_iconimage->setObjectName("widget_iconimage");
        widget_iconimage->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
        gridLayout = new QGridLayout(widget_iconimage);
        gridLayout->setObjectName("gridLayout");
        gridLayout_iconimage = new QGridLayout();
        gridLayout_iconimage->setObjectName("gridLayout_iconimage");

        gridLayout->addLayout(gridLayout_iconimage, 0, 0, 1, 1);


        gridLayout_3->addWidget(widget_iconimage, 1, 0, 1, 1);


        horizontalLayout_6->addWidget(groupBox_imageshow);


        gridLayout_9->addWidget(widget_imageshow, 0, 1, 3, 1);

        groupBox_4 = new QGroupBox(widget_2);
        groupBox_4->setObjectName("groupBox_4");
        sizePolicy1.setHeightForWidth(groupBox_4->sizePolicy().hasHeightForWidth());
        groupBox_4->setSizePolicy(sizePolicy1);
        groupBox_4->setMinimumSize(QSize(0, 90));
        groupBox_4->setMaximumSize(QSize(16777215, 16777215));
        groupBox_4->setBaseSize(QSize(0, 0));
        horizontalLayout_2 = new QHBoxLayout(groupBox_4);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_TaskProgress = new QHBoxLayout();
        horizontalLayout_TaskProgress->setObjectName("horizontalLayout_TaskProgress");

        horizontalLayout_2->addLayout(horizontalLayout_TaskProgress);


        gridLayout_9->addWidget(groupBox_4, 1, 0, 1, 1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        containerWdget = new QWidget(widget_2);
        containerWdget->setObjectName("containerWdget");
        sizePolicy4.setHeightForWidth(containerWdget->sizePolicy().hasHeightForWidth());
        containerWdget->setSizePolicy(sizePolicy4);
        containerWdget->setMinimumSize(QSize(800, 0));
        containerWdget->setMaximumSize(QSize(800, 16777215));
        horizontalLayout_4 = new QHBoxLayout(containerWdget);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        ptzWidget = new QWidget(containerWdget);
        ptzWidget->setObjectName("ptzWidget");
        sizePolicy4.setHeightForWidth(ptzWidget->sizePolicy().hasHeightForWidth());
        ptzWidget->setSizePolicy(sizePolicy4);
        ptzWidget->setMinimumSize(QSize(700, 0));
        ptzWidget->setMaximumSize(QSize(705, 16777215));
        horizontalLayout_5 = new QHBoxLayout(ptzWidget);
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        treeWidget = new QTreeWidget(ptzWidget);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QString::fromUtf8("1"));
        treeWidget->setHeaderItem(__qtreewidgetitem);
        treeWidget->setObjectName("treeWidget");
        QSizePolicy sizePolicy5(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(treeWidget->sizePolicy().hasHeightForWidth());
        treeWidget->setSizePolicy(sizePolicy5);
        treeWidget->setMinimumSize(QSize(700, 0));
        treeWidget->setMaximumSize(QSize(16777215, 16777215));

        horizontalLayout_5->addWidget(treeWidget);


        horizontalLayout_4->addWidget(ptzWidget);

        btnWidget = new QWidget(containerWdget);
        btnWidget->setObjectName("btnWidget");
        sizePolicy5.setHeightForWidth(btnWidget->sizePolicy().hasHeightForWidth());
        btnWidget->setSizePolicy(sizePolicy5);
        btnWidget->setMinimumSize(QSize(0, 0));
        btnWidget->setMaximumSize(QSize(53, 16777215));
        gridLayout_10 = new QGridLayout(btnWidget);
        gridLayout_10->setObjectName("gridLayout_10");
        showOrHideBtn = new QPushButton(btnWidget);
        showOrHideBtn->setObjectName("showOrHideBtn");
        sizePolicy2.setHeightForWidth(showOrHideBtn->sizePolicy().hasHeightForWidth());
        showOrHideBtn->setSizePolicy(sizePolicy2);
        showOrHideBtn->setMinimumSize(QSize(38, 93));
        showOrHideBtn->setMaximumSize(QSize(24, 104));
        showOrHideBtn->setFont(font);

        gridLayout_10->addWidget(showOrHideBtn, 0, 0, 1, 1);


        horizontalLayout_4->addWidget(btnWidget);


        horizontalLayout_3->addWidget(containerWdget);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        groupBox = new QGroupBox(widget_2);
        groupBox->setObjectName("groupBox");
        QSizePolicy sizePolicy6(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy6);
        gridLayout_4 = new QGridLayout(groupBox);
        gridLayout_4->setObjectName("gridLayout_4");
        gridLayout_8902 = new QGridLayout();
        gridLayout_8902->setObjectName("gridLayout_8902");

        gridLayout_4->addLayout(gridLayout_8902, 0, 0, 1, 2);

        pb8902expand = new QPushButton(groupBox);
        pb8902expand->setObjectName("pb8902expand");
        sizePolicy2.setHeightForWidth(pb8902expand->sizePolicy().hasHeightForWidth());
        pb8902expand->setSizePolicy(sizePolicy2);
        pb8902expand->setMinimumSize(QSize(25, 25));
        pb8902expand->setMaximumSize(QSize(25, 25));
        pb8902expand->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        gridLayout_4->addWidget(pb8902expand, 2, 0, 1, 1);

        verticalLayout_8902scroll = new QVBoxLayout();
        verticalLayout_8902scroll->setObjectName("verticalLayout_8902scroll");
        horizontalScrollBar_8902 = new QScrollBar(groupBox);
        horizontalScrollBar_8902->setObjectName("horizontalScrollBar_8902");
        sizePolicy1.setHeightForWidth(horizontalScrollBar_8902->sizePolicy().hasHeightForWidth());
        horizontalScrollBar_8902->setSizePolicy(sizePolicy1);
        horizontalScrollBar_8902->setOrientation(Qt::Orientation::Horizontal);

        verticalLayout_8902scroll->addWidget(horizontalScrollBar_8902);


        gridLayout_4->addLayout(verticalLayout_8902scroll, 2, 1, 1, 1);


        verticalLayout->addWidget(groupBox);

        groupBox_2 = new QGroupBox(widget_2);
        groupBox_2->setObjectName("groupBox_2");
        sizePolicy6.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy6);
        gridLayout_5 = new QGridLayout(groupBox_2);
        gridLayout_5->setObjectName("gridLayout_5");
        gridLayout_5322 = new QGridLayout();
        gridLayout_5322->setObjectName("gridLayout_5322");

        gridLayout_5->addLayout(gridLayout_5322, 0, 0, 1, 2);

        pb5322expand = new QPushButton(groupBox_2);
        pb5322expand->setObjectName("pb5322expand");
        sizePolicy2.setHeightForWidth(pb5322expand->sizePolicy().hasHeightForWidth());
        pb5322expand->setSizePolicy(sizePolicy2);
        pb5322expand->setMinimumSize(QSize(25, 25));
        pb5322expand->setMaximumSize(QSize(25, 25));
        pb5322expand->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        gridLayout_5->addWidget(pb5322expand, 2, 0, 1, 1);

        verticalLayout_5322scroll = new QVBoxLayout();
        verticalLayout_5322scroll->setObjectName("verticalLayout_5322scroll");
        horizontalScrollBar_5322 = new QScrollBar(groupBox_2);
        horizontalScrollBar_5322->setObjectName("horizontalScrollBar_5322");
        sizePolicy1.setHeightForWidth(horizontalScrollBar_5322->sizePolicy().hasHeightForWidth());
        horizontalScrollBar_5322->setSizePolicy(sizePolicy1);
        horizontalScrollBar_5322->setOrientation(Qt::Orientation::Horizontal);

        verticalLayout_5322scroll->addWidget(horizontalScrollBar_5322);


        gridLayout_5->addLayout(verticalLayout_5322scroll, 2, 1, 1, 1);


        verticalLayout->addWidget(groupBox_2);

        groupBox_3 = new QGroupBox(widget_2);
        groupBox_3->setObjectName("groupBox_3");
        sizePolicy6.setHeightForWidth(groupBox_3->sizePolicy().hasHeightForWidth());
        groupBox_3->setSizePolicy(sizePolicy6);
        gridLayout_6 = new QGridLayout(groupBox_3);
        gridLayout_6->setObjectName("gridLayout_6");
        pb5323expand = new QPushButton(groupBox_3);
        pb5323expand->setObjectName("pb5323expand");
        sizePolicy2.setHeightForWidth(pb5323expand->sizePolicy().hasHeightForWidth());
        pb5323expand->setSizePolicy(sizePolicy2);
        pb5323expand->setMinimumSize(QSize(25, 25));
        pb5323expand->setMaximumSize(QSize(25, 25));
        pb5323expand->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        gridLayout_6->addWidget(pb5323expand, 2, 0, 1, 1);

        gridLayout_5323 = new QGridLayout();
        gridLayout_5323->setObjectName("gridLayout_5323");

        gridLayout_6->addLayout(gridLayout_5323, 0, 0, 1, 3);

        verticalLayout_5323scroll = new QVBoxLayout();
        verticalLayout_5323scroll->setObjectName("verticalLayout_5323scroll");
        horizontalScrollBar_5323 = new QScrollBar(groupBox_3);
        horizontalScrollBar_5323->setObjectName("horizontalScrollBar_5323");
        sizePolicy1.setHeightForWidth(horizontalScrollBar_5323->sizePolicy().hasHeightForWidth());
        horizontalScrollBar_5323->setSizePolicy(sizePolicy1);
        horizontalScrollBar_5323->setOrientation(Qt::Orientation::Horizontal);

        verticalLayout_5323scroll->addWidget(horizontalScrollBar_5323);


        gridLayout_6->addLayout(verticalLayout_5323scroll, 2, 1, 1, 2);


        verticalLayout->addWidget(groupBox_3);


        horizontalLayout_3->addLayout(verticalLayout);


        gridLayout_9->addLayout(horizontalLayout_3, 2, 0, 1, 1);


        gridLayout_wireconnect->addWidget(widget_2, 0, 0, 1, 1);


        gridLayout_44->addLayout(gridLayout_wireconnect, 0, 0, 1, 1);


        retranslateUi(ManageTask);

        QMetaObject::connectSlotsByName(ManageTask);
    } // setupUi

    void retranslateUi(QWidget *ManageTask)
    {
        ManageTask->setWindowTitle(QCoreApplication::translate("ManageTask", "Form", nullptr));
        label_4->setText(QCoreApplication::translate("ManageTask", "\346\265\213\350\257\225\345\205\203\344\273\266id:", nullptr));
        pblabelinfoshow->setText(QCoreApplication::translate("ManageTask", "\345\234\250\345\233\276\344\270\255\345\217\214\345\207\273\351\200\211\346\213\251", nullptr));
        pbrefresh->setText(QCoreApplication::translate("ManageTask", "\345\210\267\346\226\260", nullptr));
        label->setText(QCoreApplication::translate("ManageTask", "\351\200\211\346\213\251\344\273\273\345\212\241ID:", nullptr));
        pdRuntask->setText(QCoreApplication::translate("ManageTask", "\346\211\247\350\241\214", nullptr));
        pbnextstep->setText(QCoreApplication::translate("ManageTask", "\347\273\247\347\273\255", nullptr));
        pbInterrupt->setText(QCoreApplication::translate("ManageTask", "\347\273\223\346\235\237", nullptr));
        pbViewTask->setText(QCoreApplication::translate("ManageTask", "\346\237\245\347\234\213", nullptr));
        showOrHideBtnimage->setText(QCoreApplication::translate("ManageTask", ">>", nullptr));
        groupBox_imageshow->setTitle(QCoreApplication::translate("ManageTask", "\346\270\251\345\272\246\345\233\276\345\261\225\347\244\272(\344\270\212\351\235\242\346\230\257\345\217\202\350\200\203\345\233\276\347\211\207\357\274\214\344\270\213\351\235\242\346\230\257\346\265\213\350\257\225\346\211\200\345\276\227\346\270\251\345\272\246\345\233\276\347\211\207\357\274\211", nullptr));
        groupBox_4->setTitle(QCoreApplication::translate("ManageTask", "\344\273\273\345\212\241\346\211\247\350\241\214\350\277\233\345\272\246", nullptr));
        showOrHideBtn->setText(QCoreApplication::translate("ManageTask", "<<", nullptr));
        groupBox->setTitle(QCoreApplication::translate("ManageTask", "\344\270\207\347\224\250\350\241\250", nullptr));
        pb8902expand->setText(QString());
        groupBox_2->setTitle(QCoreApplication::translate("ManageTask", "\346\225\260\345\255\227\350\276\223\345\205\245", nullptr));
        pb5322expand->setText(QString());
        groupBox_3->setTitle(QCoreApplication::translate("ManageTask", "\346\250\241\346\213\237\350\276\223\345\205\245", nullptr));
        pb5323expand->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class ManageTask: public Ui_ManageTask {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MANAGETASK_H
