/********************************************************************************
** Form generated from reading UI file 'addtask.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADDTASK_H
#define UI_ADDTASK_H

#include <QtCore/QDate>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTimeEdit>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AddTask
{
public:
    QGridLayout *gridLayout_8;
    QGroupBox *groupBox_6;
    QGridLayout *gridLayout_9;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout;
    QLabel *label_14;
    QTableWidget *Tableoutputport;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_15;
    QTableWidget *Tablecollectport;
    QGroupBox *groupBox_7;
    QGridLayout *gridLayout_10;
    QProgressBar *progressBar_singlestep;
    QPushButton *pbcancel_singlestep;
    QTreeWidget *treeWidget_taskinfo;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_5;
    QHBoxLayout *horizontalLayout;
    QLabel *label_18;
    QComboBox *cbdeviceid;
    QPushButton *pbshowpcbimage;
    QLabel *label_19;
    QComboBox *cbtesttaskid;
    QPushButton *pbrefreshdeviceid;
    QPushButton *pbvieworedit;
    QPushButton *pbdelete;
    QLabel *label_10;
    QLineEdit *lineEdit_taskid;
    QPushButton *pbcreattask;
    QSpacerItem *horizontalSpacer_9;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_6;
    QWidget *widget;
    QGridLayout *gridLayout_12;
    QGridLayout *gridLayout_locationimage;
    QPushButton *pbselectlocation;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout_3;
    QGroupBox *groupBox_2;
    QFormLayout *formLayout_2;
    QComboBox *comboBox_outputport;
    QLabel *label;
    QGroupBox *groupBox_digitaloutput;
    QGridLayout *gridLayout;
    QLabel *label_2;
    QComboBox *comboBox_outputtype;
    QLabel *label_3;
    QDoubleSpinBox *doubleSpinBox_outputvoltage;
    QLabel *label_6;
    QLabel *label_4;
    QSpinBox *spinBox_outputfrequency;
    QLabel *label_7;
    QLabel *label_13;
    QDoubleSpinBox *doubleSpinBox_outputdutycycle;
    QGroupBox *groupBox_analogoutput;
    QGridLayout *gridLayout_2;
    QLabel *label_5;
    QLabel *label_8;
    QDoubleSpinBox *spinBox_current;
    QHBoxLayout *horizontalLayout_8;
    QPushButton *pdaddwave;
    QGroupBox *groupBox_5;
    QGridLayout *gridLayout_4;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_11;
    QComboBox *comboBox_collectdevice;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_9;
    QComboBox *comboBox_collectport;
    QHBoxLayout *horizontalLayout_9;
    QLabel *label_16;
    QTimeEdit *Steptime;
    QHBoxLayout *horizontalLayout_7;
    QPushButton *pbaddcollectport;
    QHBoxLayout *horizontalLayout_10;
    QSpacerItem *horizontalSpacer_7;
    QLabel *label_17;
    QComboBox *comboBox_ispaused;
    QSpacerItem *horizontalSpacer_8;
    QLabel *label_20;
    QComboBox *comboBox_isthermometry;
    QLabel *label_21;
    QDoubleSpinBox *doubleSpinBox_pausetime;
    QLabel *label_22;
    QSpacerItem *horizontalSpacer_6;
    QHBoxLayout *horizontalLayout_6;
    QSpacerItem *horizontalSpacer_4;
    QPushButton *pbsave;
    QSpacerItem *horizontalSpacer_5;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *pbaddstep;
    QSpacerItem *horizontalSpacer;
    QPushButton *pbsaveandrun;
    QSpacerItem *horizontalSpacer_10;
    QPushButton *pbaddagain;
    QSpacerItem *horizontalSpacer_3;

    void setupUi(QWidget *AddTask)
    {
        if (AddTask->objectName().isEmpty())
            AddTask->setObjectName("AddTask");
        AddTask->resize(1892, 1130);
        gridLayout_8 = new QGridLayout(AddTask);
        gridLayout_8->setObjectName("gridLayout_8");
        groupBox_6 = new QGroupBox(AddTask);
        groupBox_6->setObjectName("groupBox_6");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupBox_6->sizePolicy().hasHeightForWidth());
        groupBox_6->setSizePolicy(sizePolicy);
        gridLayout_9 = new QGridLayout(groupBox_6);
        gridLayout_9->setObjectName("gridLayout_9");
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        label_14 = new QLabel(groupBox_6);
        label_14->setObjectName("label_14");

        verticalLayout->addWidget(label_14);

        Tableoutputport = new QTableWidget(groupBox_6);
        Tableoutputport->setObjectName("Tableoutputport");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(Tableoutputport->sizePolicy().hasHeightForWidth());
        Tableoutputport->setSizePolicy(sizePolicy1);

        verticalLayout->addWidget(Tableoutputport);


        horizontalLayout_3->addLayout(verticalLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName("verticalLayout_2");
        label_15 = new QLabel(groupBox_6);
        label_15->setObjectName("label_15");

        verticalLayout_2->addWidget(label_15);

        Tablecollectport = new QTableWidget(groupBox_6);
        Tablecollectport->setObjectName("Tablecollectport");
        sizePolicy1.setHeightForWidth(Tablecollectport->sizePolicy().hasHeightForWidth());
        Tablecollectport->setSizePolicy(sizePolicy1);

        verticalLayout_2->addWidget(Tablecollectport);


        horizontalLayout_3->addLayout(verticalLayout_2);


        gridLayout_9->addLayout(horizontalLayout_3, 0, 0, 1, 1);


        gridLayout_8->addWidget(groupBox_6, 3, 0, 1, 1);

        groupBox_7 = new QGroupBox(AddTask);
        groupBox_7->setObjectName("groupBox_7");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(groupBox_7->sizePolicy().hasHeightForWidth());
        groupBox_7->setSizePolicy(sizePolicy2);
        groupBox_7->setMinimumSize(QSize(400, 0));
        gridLayout_10 = new QGridLayout(groupBox_7);
        gridLayout_10->setObjectName("gridLayout_10");
        progressBar_singlestep = new QProgressBar(groupBox_7);
        progressBar_singlestep->setObjectName("progressBar_singlestep");
        QSizePolicy sizePolicy3(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(progressBar_singlestep->sizePolicy().hasHeightForWidth());
        progressBar_singlestep->setSizePolicy(sizePolicy3);
        progressBar_singlestep->setMinimumSize(QSize(0, 0));
        progressBar_singlestep->setValue(50);
        progressBar_singlestep->setOrientation(Qt::Orientation::Horizontal);
        progressBar_singlestep->setTextDirection(QProgressBar::Direction::TopToBottom);

        gridLayout_10->addWidget(progressBar_singlestep, 1, 0, 1, 1);

        pbcancel_singlestep = new QPushButton(groupBox_7);
        pbcancel_singlestep->setObjectName("pbcancel_singlestep");
        pbcancel_singlestep->setMinimumSize(QSize(0, 0));

        gridLayout_10->addWidget(pbcancel_singlestep, 1, 1, 1, 1);

        treeWidget_taskinfo = new QTreeWidget(groupBox_7);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QString::fromUtf8("1"));
        treeWidget_taskinfo->setHeaderItem(__qtreewidgetitem);
        treeWidget_taskinfo->setObjectName("treeWidget_taskinfo");
        sizePolicy3.setHeightForWidth(treeWidget_taskinfo->sizePolicy().hasHeightForWidth());
        treeWidget_taskinfo->setSizePolicy(sizePolicy3);

        gridLayout_10->addWidget(treeWidget_taskinfo, 0, 0, 1, 2);


        gridLayout_8->addWidget(groupBox_7, 0, 1, 4, 1);

        groupBox = new QGroupBox(AddTask);
        groupBox->setObjectName("groupBox");
        QSizePolicy sizePolicy4(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy4);
        gridLayout_5 = new QGridLayout(groupBox);
        gridLayout_5->setObjectName("gridLayout_5");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        label_18 = new QLabel(groupBox);
        label_18->setObjectName("label_18");

        horizontalLayout->addWidget(label_18);

        cbdeviceid = new QComboBox(groupBox);
        cbdeviceid->setObjectName("cbdeviceid");
        QSizePolicy sizePolicy5(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(cbdeviceid->sizePolicy().hasHeightForWidth());
        cbdeviceid->setSizePolicy(sizePolicy5);
        cbdeviceid->setMinimumSize(QSize(70, 0));
        cbdeviceid->setMaximumSize(QSize(70, 16777215));

        horizontalLayout->addWidget(cbdeviceid);

        pbshowpcbimage = new QPushButton(groupBox);
        pbshowpcbimage->setObjectName("pbshowpcbimage");
        sizePolicy5.setHeightForWidth(pbshowpcbimage->sizePolicy().hasHeightForWidth());
        pbshowpcbimage->setSizePolicy(sizePolicy5);
        pbshowpcbimage->setMinimumSize(QSize(146, 0));
        pbshowpcbimage->setMaximumSize(QSize(146, 16777215));

        horizontalLayout->addWidget(pbshowpcbimage);

        label_19 = new QLabel(groupBox);
        label_19->setObjectName("label_19");

        horizontalLayout->addWidget(label_19);

        cbtesttaskid = new QComboBox(groupBox);
        cbtesttaskid->setObjectName("cbtesttaskid");
        QSizePolicy sizePolicy6(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(cbtesttaskid->sizePolicy().hasHeightForWidth());
        cbtesttaskid->setSizePolicy(sizePolicy6);

        horizontalLayout->addWidget(cbtesttaskid);

        pbrefreshdeviceid = new QPushButton(groupBox);
        pbrefreshdeviceid->setObjectName("pbrefreshdeviceid");

        horizontalLayout->addWidget(pbrefreshdeviceid);

        pbvieworedit = new QPushButton(groupBox);
        pbvieworedit->setObjectName("pbvieworedit");

        horizontalLayout->addWidget(pbvieworedit);

        pbdelete = new QPushButton(groupBox);
        pbdelete->setObjectName("pbdelete");

        horizontalLayout->addWidget(pbdelete);

        label_10 = new QLabel(groupBox);
        label_10->setObjectName("label_10");

        horizontalLayout->addWidget(label_10);

        lineEdit_taskid = new QLineEdit(groupBox);
        lineEdit_taskid->setObjectName("lineEdit_taskid");
        sizePolicy6.setHeightForWidth(lineEdit_taskid->sizePolicy().hasHeightForWidth());
        lineEdit_taskid->setSizePolicy(sizePolicy6);

        horizontalLayout->addWidget(lineEdit_taskid);


        gridLayout_5->addLayout(horizontalLayout, 2, 0, 1, 1);

        pbcreattask = new QPushButton(groupBox);
        pbcreattask->setObjectName("pbcreattask");
        sizePolicy4.setHeightForWidth(pbcreattask->sizePolicy().hasHeightForWidth());
        pbcreattask->setSizePolicy(sizePolicy4);

        gridLayout_5->addWidget(pbcreattask, 2, 2, 2, 1);

        horizontalSpacer_9 = new QSpacerItem(30, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        gridLayout_5->addItem(horizontalSpacer_9, 2, 1, 1, 1);


        gridLayout_8->addWidget(groupBox, 0, 0, 1, 1);

        groupBox_3 = new QGroupBox(AddTask);
        groupBox_3->setObjectName("groupBox_3");
        sizePolicy.setHeightForWidth(groupBox_3->sizePolicy().hasHeightForWidth());
        groupBox_3->setSizePolicy(sizePolicy);
        gridLayout_6 = new QGridLayout(groupBox_3);
        gridLayout_6->setObjectName("gridLayout_6");
        widget = new QWidget(groupBox_3);
        widget->setObjectName("widget");
        sizePolicy5.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy5);
        widget->setMinimumSize(QSize(700, 450));
        widget->setMaximumSize(QSize(500, 450));
        gridLayout_12 = new QGridLayout(widget);
        gridLayout_12->setObjectName("gridLayout_12");
        gridLayout_locationimage = new QGridLayout();
        gridLayout_locationimage->setObjectName("gridLayout_locationimage");

        gridLayout_12->addLayout(gridLayout_locationimage, 0, 0, 1, 1);

        pbselectlocation = new QPushButton(widget);
        pbselectlocation->setObjectName("pbselectlocation");

        gridLayout_12->addWidget(pbselectlocation, 1, 0, 1, 1);


        gridLayout_6->addWidget(widget, 0, 2, 1, 1);

        groupBox_4 = new QGroupBox(groupBox_3);
        groupBox_4->setObjectName("groupBox_4");
        QSizePolicy sizePolicy7(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy7.setHorizontalStretch(0);
        sizePolicy7.setVerticalStretch(0);
        sizePolicy7.setHeightForWidth(groupBox_4->sizePolicy().hasHeightForWidth());
        groupBox_4->setSizePolicy(sizePolicy7);
        gridLayout_3 = new QGridLayout(groupBox_4);
        gridLayout_3->setObjectName("gridLayout_3");
        groupBox_2 = new QGroupBox(groupBox_4);
        groupBox_2->setObjectName("groupBox_2");
        sizePolicy3.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy3);
        formLayout_2 = new QFormLayout(groupBox_2);
        formLayout_2->setObjectName("formLayout_2");
        comboBox_outputport = new QComboBox(groupBox_2);
        comboBox_outputport->setObjectName("comboBox_outputport");

        formLayout_2->setWidget(0, QFormLayout::FieldRole, comboBox_outputport);

        label = new QLabel(groupBox_2);
        label->setObjectName("label");

        formLayout_2->setWidget(0, QFormLayout::LabelRole, label);


        gridLayout_3->addWidget(groupBox_2, 0, 0, 1, 1);

        groupBox_digitaloutput = new QGroupBox(groupBox_4);
        groupBox_digitaloutput->setObjectName("groupBox_digitaloutput");
        sizePolicy.setHeightForWidth(groupBox_digitaloutput->sizePolicy().hasHeightForWidth());
        groupBox_digitaloutput->setSizePolicy(sizePolicy);
        gridLayout = new QGridLayout(groupBox_digitaloutput);
        gridLayout->setObjectName("gridLayout");
        label_2 = new QLabel(groupBox_digitaloutput);
        label_2->setObjectName("label_2");

        gridLayout->addWidget(label_2, 0, 0, 1, 1);

        comboBox_outputtype = new QComboBox(groupBox_digitaloutput);
        comboBox_outputtype->setObjectName("comboBox_outputtype");
        sizePolicy6.setHeightForWidth(comboBox_outputtype->sizePolicy().hasHeightForWidth());
        comboBox_outputtype->setSizePolicy(sizePolicy6);

        gridLayout->addWidget(comboBox_outputtype, 0, 1, 1, 2);

        label_3 = new QLabel(groupBox_digitaloutput);
        label_3->setObjectName("label_3");

        gridLayout->addWidget(label_3, 1, 0, 1, 1);

        doubleSpinBox_outputvoltage = new QDoubleSpinBox(groupBox_digitaloutput);
        doubleSpinBox_outputvoltage->setObjectName("doubleSpinBox_outputvoltage");
        sizePolicy6.setHeightForWidth(doubleSpinBox_outputvoltage->sizePolicy().hasHeightForWidth());
        doubleSpinBox_outputvoltage->setSizePolicy(sizePolicy6);
        doubleSpinBox_outputvoltage->setMaximum(10.000000000000000);
        doubleSpinBox_outputvoltage->setSingleStep(0.010000000000000);
        doubleSpinBox_outputvoltage->setValue(5.000000000000000);

        gridLayout->addWidget(doubleSpinBox_outputvoltage, 1, 1, 1, 1);

        label_6 = new QLabel(groupBox_digitaloutput);
        label_6->setObjectName("label_6");

        gridLayout->addWidget(label_6, 1, 2, 1, 1);

        label_4 = new QLabel(groupBox_digitaloutput);
        label_4->setObjectName("label_4");

        gridLayout->addWidget(label_4, 2, 0, 1, 1);

        spinBox_outputfrequency = new QSpinBox(groupBox_digitaloutput);
        spinBox_outputfrequency->setObjectName("spinBox_outputfrequency");
        sizePolicy6.setHeightForWidth(spinBox_outputfrequency->sizePolicy().hasHeightForWidth());
        spinBox_outputfrequency->setSizePolicy(sizePolicy6);
        spinBox_outputfrequency->setMaximum(1000000000);
        spinBox_outputfrequency->setValue(1000);

        gridLayout->addWidget(spinBox_outputfrequency, 2, 1, 1, 1);

        label_7 = new QLabel(groupBox_digitaloutput);
        label_7->setObjectName("label_7");

        gridLayout->addWidget(label_7, 2, 2, 1, 1);

        label_13 = new QLabel(groupBox_digitaloutput);
        label_13->setObjectName("label_13");

        gridLayout->addWidget(label_13, 3, 0, 1, 1);

        doubleSpinBox_outputdutycycle = new QDoubleSpinBox(groupBox_digitaloutput);
        doubleSpinBox_outputdutycycle->setObjectName("doubleSpinBox_outputdutycycle");
        sizePolicy6.setHeightForWidth(doubleSpinBox_outputdutycycle->sizePolicy().hasHeightForWidth());
        doubleSpinBox_outputdutycycle->setSizePolicy(sizePolicy6);
        doubleSpinBox_outputdutycycle->setMaximum(1.000000000000000);
        doubleSpinBox_outputdutycycle->setValue(0.500000000000000);

        gridLayout->addWidget(doubleSpinBox_outputdutycycle, 3, 1, 1, 2);


        gridLayout_3->addWidget(groupBox_digitaloutput, 0, 1, 3, 1);

        groupBox_analogoutput = new QGroupBox(groupBox_4);
        groupBox_analogoutput->setObjectName("groupBox_analogoutput");
        sizePolicy3.setHeightForWidth(groupBox_analogoutput->sizePolicy().hasHeightForWidth());
        groupBox_analogoutput->setSizePolicy(sizePolicy3);
        gridLayout_2 = new QGridLayout(groupBox_analogoutput);
        gridLayout_2->setObjectName("gridLayout_2");
        label_5 = new QLabel(groupBox_analogoutput);
        label_5->setObjectName("label_5");

        gridLayout_2->addWidget(label_5, 0, 0, 1, 1);

        label_8 = new QLabel(groupBox_analogoutput);
        label_8->setObjectName("label_8");

        gridLayout_2->addWidget(label_8, 0, 2, 1, 1);

        spinBox_current = new QDoubleSpinBox(groupBox_analogoutput);
        spinBox_current->setObjectName("spinBox_current");
        spinBox_current->setMinimum(-10.000000000000000);
        spinBox_current->setMaximum(10.000000000000000);

        gridLayout_2->addWidget(spinBox_current, 0, 1, 1, 1);


        gridLayout_3->addWidget(groupBox_analogoutput, 1, 0, 1, 1);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName("horizontalLayout_8");
        pdaddwave = new QPushButton(groupBox_4);
        pdaddwave->setObjectName("pdaddwave");
        sizePolicy5.setHeightForWidth(pdaddwave->sizePolicy().hasHeightForWidth());
        pdaddwave->setSizePolicy(sizePolicy5);

        horizontalLayout_8->addWidget(pdaddwave);


        gridLayout_3->addLayout(horizontalLayout_8, 2, 0, 1, 1);


        gridLayout_6->addWidget(groupBox_4, 0, 0, 1, 1);

        groupBox_5 = new QGroupBox(groupBox_3);
        groupBox_5->setObjectName("groupBox_5");
        sizePolicy.setHeightForWidth(groupBox_5->sizePolicy().hasHeightForWidth());
        groupBox_5->setSizePolicy(sizePolicy);
        gridLayout_4 = new QGridLayout(groupBox_5);
        gridLayout_4->setObjectName("gridLayout_4");
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        label_11 = new QLabel(groupBox_5);
        label_11->setObjectName("label_11");

        horizontalLayout_4->addWidget(label_11);

        comboBox_collectdevice = new QComboBox(groupBox_5);
        comboBox_collectdevice->addItem(QString());
        comboBox_collectdevice->addItem(QString());
        comboBox_collectdevice->addItem(QString());
        comboBox_collectdevice->setObjectName("comboBox_collectdevice");
        sizePolicy6.setHeightForWidth(comboBox_collectdevice->sizePolicy().hasHeightForWidth());
        comboBox_collectdevice->setSizePolicy(sizePolicy6);

        horizontalLayout_4->addWidget(comboBox_collectdevice);


        gridLayout_4->addLayout(horizontalLayout_4, 1, 0, 1, 1);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        label_9 = new QLabel(groupBox_5);
        label_9->setObjectName("label_9");

        horizontalLayout_5->addWidget(label_9);

        comboBox_collectport = new QComboBox(groupBox_5);
        comboBox_collectport->setObjectName("comboBox_collectport");
        sizePolicy6.setHeightForWidth(comboBox_collectport->sizePolicy().hasHeightForWidth());
        comboBox_collectport->setSizePolicy(sizePolicy6);

        horizontalLayout_5->addWidget(comboBox_collectport);


        gridLayout_4->addLayout(horizontalLayout_5, 2, 0, 1, 1);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName("horizontalLayout_9");
        label_16 = new QLabel(groupBox_5);
        label_16->setObjectName("label_16");

        horizontalLayout_9->addWidget(label_16);

        Steptime = new QTimeEdit(groupBox_5);
        Steptime->setObjectName("Steptime");
        sizePolicy6.setHeightForWidth(Steptime->sizePolicy().hasHeightForWidth());
        Steptime->setSizePolicy(sizePolicy6);
        Steptime->setDateTime(QDateTime(QDate(2004, 12, 31), QTime(0, 0, 0)));
        Steptime->setMinimumDateTime(QDateTime(QDate(2004, 12, 31), QTime(0, 0, 0)));
        Steptime->setMinimumDate(QDate(2004, 12, 31));
        Steptime->setMaximumTime(QTime(1, 0, 0));
        Steptime->setMinimumTime(QTime(0, 0, 0));
        Steptime->setCurrentSection(QDateTimeEdit::Section::SecondSection);
        Steptime->setTime(QTime(0, 0, 0));

        horizontalLayout_9->addWidget(Steptime);


        gridLayout_4->addLayout(horizontalLayout_9, 0, 0, 1, 1);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName("horizontalLayout_7");
        pbaddcollectport = new QPushButton(groupBox_5);
        pbaddcollectport->setObjectName("pbaddcollectport");
        sizePolicy5.setHeightForWidth(pbaddcollectport->sizePolicy().hasHeightForWidth());
        pbaddcollectport->setSizePolicy(sizePolicy5);

        horizontalLayout_7->addWidget(pbaddcollectport);


        gridLayout_4->addLayout(horizontalLayout_7, 3, 0, 1, 1);


        gridLayout_6->addWidget(groupBox_5, 0, 1, 1, 1);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName("horizontalLayout_10");
        horizontalLayout_10->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);
        horizontalLayout_10->setContentsMargins(-1, -1, -1, 0);
        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer_7);

        label_17 = new QLabel(groupBox_3);
        label_17->setObjectName("label_17");
        sizePolicy4.setHeightForWidth(label_17->sizePolicy().hasHeightForWidth());
        label_17->setSizePolicy(sizePolicy4);

        horizontalLayout_10->addWidget(label_17);

        comboBox_ispaused = new QComboBox(groupBox_3);
        comboBox_ispaused->addItem(QString());
        comboBox_ispaused->addItem(QString());
        comboBox_ispaused->setObjectName("comboBox_ispaused");
        sizePolicy5.setHeightForWidth(comboBox_ispaused->sizePolicy().hasHeightForWidth());
        comboBox_ispaused->setSizePolicy(sizePolicy5);
        comboBox_ispaused->setMinimumSize(QSize(80, 0));

        horizontalLayout_10->addWidget(comboBox_ispaused);

        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer_8);

        label_20 = new QLabel(groupBox_3);
        label_20->setObjectName("label_20");

        horizontalLayout_10->addWidget(label_20);

        comboBox_isthermometry = new QComboBox(groupBox_3);
        comboBox_isthermometry->addItem(QString());
        comboBox_isthermometry->addItem(QString());
        comboBox_isthermometry->setObjectName("comboBox_isthermometry");

        horizontalLayout_10->addWidget(comboBox_isthermometry);

        label_21 = new QLabel(groupBox_3);
        label_21->setObjectName("label_21");

        horizontalLayout_10->addWidget(label_21);

        doubleSpinBox_pausetime = new QDoubleSpinBox(groupBox_3);
        doubleSpinBox_pausetime->setObjectName("doubleSpinBox_pausetime");
        doubleSpinBox_pausetime->setValue(50.000000000000000);

        horizontalLayout_10->addWidget(doubleSpinBox_pausetime);

        label_22 = new QLabel(groupBox_3);
        label_22->setObjectName("label_22");

        horizontalLayout_10->addWidget(label_22);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer_6);


        gridLayout_6->addLayout(horizontalLayout_10, 1, 0, 1, 3);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_4);

        pbsave = new QPushButton(groupBox_3);
        pbsave->setObjectName("pbsave");

        horizontalLayout_6->addWidget(pbsave);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_5);


        gridLayout_6->addLayout(horizontalLayout_6, 5, 0, 1, 3);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        pbaddstep = new QPushButton(groupBox_3);
        pbaddstep->setObjectName("pbaddstep");

        horizontalLayout_2->addWidget(pbaddstep);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        pbsaveandrun = new QPushButton(groupBox_3);
        pbsaveandrun->setObjectName("pbsaveandrun");

        horizontalLayout_2->addWidget(pbsaveandrun);

        horizontalSpacer_10 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_10);

        pbaddagain = new QPushButton(groupBox_3);
        pbaddagain->setObjectName("pbaddagain");

        horizontalLayout_2->addWidget(pbaddagain);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);


        gridLayout_6->addLayout(horizontalLayout_2, 4, 0, 1, 3);


        gridLayout_8->addWidget(groupBox_3, 1, 0, 1, 1);


        retranslateUi(AddTask);

        QMetaObject::connectSlotsByName(AddTask);
    } // setupUi

    void retranslateUi(QWidget *AddTask)
    {
        AddTask->setWindowTitle(QCoreApplication::translate("AddTask", "Form", nullptr));
        groupBox_6->setTitle(QCoreApplication::translate("AddTask", "\345\275\223\345\211\215\346\255\245\351\252\244\346\267\273\345\212\240\347\232\204\347\253\257\345\217\243", nullptr));
        label_14->setText(QCoreApplication::translate("AddTask", "\344\277\241\345\217\267\350\276\223\345\207\272\347\253\257\345\217\243", nullptr));
        label_15->setText(QCoreApplication::translate("AddTask", "\351\207\207\351\233\206\347\253\257\345\217\243", nullptr));
        groupBox_7->setTitle(QCoreApplication::translate("AddTask", "\345\275\223\345\211\215\344\273\273\345\212\241\350\257\246\346\203\205", nullptr));
        pbcancel_singlestep->setText(QCoreApplication::translate("AddTask", "\345\217\226\346\266\210", nullptr));
        groupBox->setTitle(QCoreApplication::translate("AddTask", "\346\265\213\350\257\225\344\273\273\345\212\241\351\205\215\347\275\256", nullptr));
        label_18->setText(QCoreApplication::translate("AddTask", "\351\200\211\346\213\251\346\265\213\350\257\225\345\205\203\344\273\266id\357\274\232", nullptr));
        pbshowpcbimage->setText(QCoreApplication::translate("AddTask", "\345\234\250\345\233\276\344\270\255\345\217\214\345\207\273\351\200\211\346\213\251", nullptr));
        label_19->setText(QCoreApplication::translate("AddTask", "\344\273\273\345\212\241id:", nullptr));
        pbrefreshdeviceid->setText(QCoreApplication::translate("AddTask", "\345\210\267\346\226\260", nullptr));
        pbvieworedit->setText(QCoreApplication::translate("AddTask", "\346\237\245\347\234\213/\347\274\226\350\276\221", nullptr));
        pbdelete->setText(QCoreApplication::translate("AddTask", "\345\210\240\351\231\244", nullptr));
        label_10->setText(QCoreApplication::translate("AddTask", "\346\226\260\345\273\272\344\273\273\345\212\241ID:", nullptr));
        pbcreattask->setText(QCoreApplication::translate("AddTask", "\346\267\273\345\212\240\346\265\213\350\257\225\344\273\273\345\212\241", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("AddTask", "\346\267\273\345\212\240\346\265\213\350\257\225\346\255\245\351\252\244", nullptr));
        pbselectlocation->setText(QCoreApplication::translate("AddTask", "\351\200\211\346\213\251", nullptr));
        groupBox_4->setTitle(QCoreApplication::translate("AddTask", "\350\276\223\345\207\272\344\277\241\345\217\267\351\205\215\347\275\256", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("AddTask", "\350\276\223\345\207\272\344\277\241\345\217\267\345\237\272\347\241\200\351\205\215\347\275\256", nullptr));
        label->setText(QCoreApplication::translate("AddTask", "\344\277\241\345\217\267\350\276\223\345\207\272\347\253\257\345\217\243\357\274\232", nullptr));
        groupBox_digitaloutput->setTitle(QCoreApplication::translate("AddTask", "\346\225\260\345\255\227\344\277\241\345\217\267\351\200\211\351\241\271", nullptr));
        label_2->setText(QCoreApplication::translate("AddTask", "\350\276\223\345\207\272\344\277\241\345\217\267\347\261\273\345\236\213\357\274\232", nullptr));
        label_3->setText(QCoreApplication::translate("AddTask", "\350\276\223\345\207\272\344\277\241\345\217\267\345\271\205\345\200\274\357\274\232", nullptr));
        label_6->setText(QCoreApplication::translate("AddTask", "V", nullptr));
        label_4->setText(QCoreApplication::translate("AddTask", "\350\276\223\345\207\272\344\277\241\345\217\267\351\242\221\347\216\207\357\274\232", nullptr));
        label_7->setText(QCoreApplication::translate("AddTask", "HZ", nullptr));
        label_13->setText(QCoreApplication::translate("AddTask", "\345\215\240\347\251\272\346\257\224\357\274\232", nullptr));
        groupBox_analogoutput->setTitle(QCoreApplication::translate("AddTask", "\346\250\241\346\213\237\344\277\241\345\217\267\351\200\211\351\241\271", nullptr));
        label_5->setText(QCoreApplication::translate("AddTask", "\350\276\223\345\207\272\347\224\265\345\216\213\345\244\247\345\260\217\357\274\232", nullptr));
        label_8->setText(QCoreApplication::translate("AddTask", "V", nullptr));
        pdaddwave->setText(QCoreApplication::translate("AddTask", "\346\267\273\345\212\240\344\277\241\345\217\267", nullptr));
        groupBox_5->setTitle(QCoreApplication::translate("AddTask", "\351\207\207\351\233\206\344\277\241\345\217\267\351\205\215\347\275\256", nullptr));
        label_11->setText(QCoreApplication::translate("AddTask", "\351\207\207\351\233\206\345\215\241\345\236\213\345\217\267\357\274\232", nullptr));
        comboBox_collectdevice->setItemText(0, QCoreApplication::translate("AddTask", "\346\250\241\346\213\237\351\207\217\350\276\223\345\205\245", nullptr));
        comboBox_collectdevice->setItemText(1, QCoreApplication::translate("AddTask", "\346\225\260\345\255\227\351\207\217\350\276\223\345\205\245", nullptr));
        comboBox_collectdevice->setItemText(2, QCoreApplication::translate("AddTask", "\344\270\207\347\224\250\350\241\250", nullptr));

        label_9->setText(QCoreApplication::translate("AddTask", "\351\207\207\351\233\206\347\253\257\345\217\243\357\274\232", nullptr));
        label_16->setText(QCoreApplication::translate("AddTask", "\346\255\245\351\252\244\351\207\207\351\233\206\346\227\266\351\227\264(ss:zzz):", nullptr));
        Steptime->setDisplayFormat(QCoreApplication::translate("AddTask", "ss:zzz", nullptr));
        pbaddcollectport->setText(QCoreApplication::translate("AddTask", "\346\267\273\345\212\240\351\207\207\351\233\206\347\253\257\345\217\243", nullptr));
        label_17->setText(QCoreApplication::translate("AddTask", "\346\255\244\346\255\245\351\252\244\350\277\220\350\241\214\345\211\215\346\230\257\345\220\246\346\232\202\345\201\234\357\274\232", nullptr));
        comboBox_ispaused->setItemText(0, QCoreApplication::translate("AddTask", "\346\230\257", nullptr));
        comboBox_ispaused->setItemText(1, QCoreApplication::translate("AddTask", "\345\220\246", nullptr));

        label_20->setText(QCoreApplication::translate("AddTask", "\346\230\257\345\220\246\351\234\200\350\246\201\346\265\213\346\270\251\357\274\232", nullptr));
        comboBox_isthermometry->setItemText(0, QCoreApplication::translate("AddTask", "\346\230\257", nullptr));
        comboBox_isthermometry->setItemText(1, QCoreApplication::translate("AddTask", "\345\220\246", nullptr));

        label_21->setText(QCoreApplication::translate("AddTask", "\346\265\213\346\270\251\345\273\266\346\227\266", nullptr));
        label_22->setText(QCoreApplication::translate("AddTask", "%", nullptr));
        pbsave->setText(QCoreApplication::translate("AddTask", "\345\256\214\346\210\220", nullptr));
        pbaddstep->setText(QCoreApplication::translate("AddTask", "\346\267\273\345\212\240\346\265\213\350\257\225\346\255\245\351\252\244", nullptr));
        pbsaveandrun->setText(QCoreApplication::translate("AddTask", "\344\277\235\345\255\230\345\271\266\346\211\247\350\241\214\346\234\254\346\255\245\351\252\244", nullptr));
        pbaddagain->setText(QCoreApplication::translate("AddTask", "\351\207\215\346\226\260\346\267\273\345\212\240\346\234\254\346\255\245\351\252\244", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AddTask: public Ui_AddTask {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADDTASK_H
