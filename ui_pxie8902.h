/********************************************************************************
** Form generated from reading UI file 'pxie8902.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PXIE8902_H
#define UI_PXIE8902_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PXIe8902
{
public:
    QGridLayout *gridLayout_3;
    QGridLayout *gridLayout_2;
    QFrame *frame;
    QPushButton *btStart;
    QPushButton *btStop;
    QPushButton *pushButton;
    QWidget *wgChart;
    QGridLayout *gridLayout;
    QGridLayout *glChart;
    QWidget *widget_2;
    QVBoxLayout *verticalLayout;
    QGroupBox *gbBasicParam;
    QGridLayout *gridLayout_5;
    QLabel *label;
    QComboBox *cbtestobj;
    QLabel *label_2;
    QComboBox *cbPowerLineFreq;
    QLabel *label_4;
    QComboBox *cbRange;
    QRadioButton *radioButton;
    QComboBox *cbAperture;
    QRadioButton *radioButton_2;
    QComboBox *cbNPLC;
    QGroupBox *gbsavedata;
    QGridLayout *gridLayout_4;
    QPushButton *pbsave;
    QTextEdit *textEdit_notion;
    QLabel *label_8;
    QComboBox *cbsavemode;
    QLabel *label_7;

    void setupUi(QWidget *PXIe8902)
    {
        if (PXIe8902->objectName().isEmpty())
            PXIe8902->setObjectName("PXIe8902");
        PXIe8902->resize(1150, 585);
        gridLayout_3 = new QGridLayout(PXIe8902);
        gridLayout_3->setObjectName("gridLayout_3");
        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName("gridLayout_2");
        frame = new QFrame(PXIe8902);
        frame->setObjectName("frame");
        frame->setMinimumSize(QSize(291, 51));
        frame->setMaximumSize(QSize(16777215, 51));
        frame->setFrameShape(QFrame::Shape::NoFrame);
        frame->setFrameShadow(QFrame::Shadow::Raised);
        btStart = new QPushButton(frame);
        btStart->setObjectName("btStart");
        btStart->setGeometry(QRect(10, 10, 79, 31));
        btStart->setFlat(false);
        btStop = new QPushButton(frame);
        btStop->setObjectName("btStop");
        btStop->setEnabled(false);
        btStop->setGeometry(QRect(218, 10, 61, 31));
        pushButton = new QPushButton(frame);
        pushButton->setObjectName("pushButton");
        pushButton->setGeometry(QRect(97, 11, 116, 30));
        pushButton->setMinimumSize(QSize(116, 0));
        pushButton->setMaximumSize(QSize(116, 16777215));

        gridLayout_2->addWidget(frame, 1, 1, 1, 1);

        wgChart = new QWidget(PXIe8902);
        wgChart->setObjectName("wgChart");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(wgChart->sizePolicy().hasHeightForWidth());
        wgChart->setSizePolicy(sizePolicy);
        wgChart->setMinimumSize(QSize(688, 358));
        gridLayout = new QGridLayout(wgChart);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        glChart = new QGridLayout();
        glChart->setObjectName("glChart");

        gridLayout->addLayout(glChart, 0, 0, 1, 1);


        gridLayout_2->addWidget(wgChart, 0, 0, 2, 1);

        widget_2 = new QWidget(PXIe8902);
        widget_2->setObjectName("widget_2");
        widget_2->setMinimumSize(QSize(281, 0));
        widget_2->setMaximumSize(QSize(281, 16777215));
        verticalLayout = new QVBoxLayout(widget_2);
        verticalLayout->setObjectName("verticalLayout");
        gbBasicParam = new QGroupBox(widget_2);
        gbBasicParam->setObjectName("gbBasicParam");
        gbBasicParam->setMinimumSize(QSize(281, 241));
        gbBasicParam->setMaximumSize(QSize(281, 260));
        gbBasicParam->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        gridLayout_5 = new QGridLayout(gbBasicParam);
        gridLayout_5->setObjectName("gridLayout_5");
        label = new QLabel(gbBasicParam);
        label->setObjectName("label");

        gridLayout_5->addWidget(label, 0, 0, 1, 1);

        cbtestobj = new QComboBox(gbBasicParam);
        cbtestobj->setObjectName("cbtestobj");

        gridLayout_5->addWidget(cbtestobj, 0, 1, 1, 1);

        label_2 = new QLabel(gbBasicParam);
        label_2->setObjectName("label_2");
        label_2->setMinimumSize(QSize(111, 0));

        gridLayout_5->addWidget(label_2, 1, 0, 1, 1);

        cbPowerLineFreq = new QComboBox(gbBasicParam);
        cbPowerLineFreq->setObjectName("cbPowerLineFreq");

        gridLayout_5->addWidget(cbPowerLineFreq, 1, 1, 1, 1);

        label_4 = new QLabel(gbBasicParam);
        label_4->setObjectName("label_4");

        gridLayout_5->addWidget(label_4, 2, 0, 1, 1);

        cbRange = new QComboBox(gbBasicParam);
        cbRange->setObjectName("cbRange");
        cbRange->setMaximumSize(QSize(154, 16777215));

        gridLayout_5->addWidget(cbRange, 2, 1, 1, 1);

        radioButton = new QRadioButton(gbBasicParam);
        radioButton->setObjectName("radioButton");
        radioButton->setChecked(true);

        gridLayout_5->addWidget(radioButton, 3, 0, 1, 1);

        cbAperture = new QComboBox(gbBasicParam);
        cbAperture->setObjectName("cbAperture");
        cbAperture->setMaximumSize(QSize(154, 16777215));
        cbAperture->setEditable(false);
        cbAperture->setModelColumn(0);

        gridLayout_5->addWidget(cbAperture, 3, 1, 1, 1);

        radioButton_2 = new QRadioButton(gbBasicParam);
        radioButton_2->setObjectName("radioButton_2");

        gridLayout_5->addWidget(radioButton_2, 4, 0, 1, 1);

        cbNPLC = new QComboBox(gbBasicParam);
        cbNPLC->setObjectName("cbNPLC");
        cbNPLC->setMaximumSize(QSize(154, 16777215));
        cbNPLC->setEditable(false);
        cbNPLC->setModelColumn(0);

        gridLayout_5->addWidget(cbNPLC, 4, 1, 1, 1);


        verticalLayout->addWidget(gbBasicParam);

        gbsavedata = new QGroupBox(widget_2);
        gbsavedata->setObjectName("gbsavedata");
        gridLayout_4 = new QGridLayout(gbsavedata);
        gridLayout_4->setObjectName("gridLayout_4");
        pbsave = new QPushButton(gbsavedata);
        pbsave->setObjectName("pbsave");

        gridLayout_4->addWidget(pbsave, 2, 0, 1, 2);

        textEdit_notion = new QTextEdit(gbsavedata);
        textEdit_notion->setObjectName("textEdit_notion");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(textEdit_notion->sizePolicy().hasHeightForWidth());
        textEdit_notion->setSizePolicy(sizePolicy1);

        gridLayout_4->addWidget(textEdit_notion, 1, 1, 1, 1);

        label_8 = new QLabel(gbsavedata);
        label_8->setObjectName("label_8");

        gridLayout_4->addWidget(label_8, 1, 0, 1, 1);

        cbsavemode = new QComboBox(gbsavedata);
        cbsavemode->setObjectName("cbsavemode");

        gridLayout_4->addWidget(cbsavemode, 0, 1, 1, 1);

        label_7 = new QLabel(gbsavedata);
        label_7->setObjectName("label_7");

        gridLayout_4->addWidget(label_7, 0, 0, 1, 1);


        verticalLayout->addWidget(gbsavedata);


        gridLayout_2->addWidget(widget_2, 0, 1, 1, 1);


        gridLayout_3->addLayout(gridLayout_2, 0, 0, 1, 1);


        retranslateUi(PXIe8902);

        cbAperture->setCurrentIndex(-1);
        cbNPLC->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(PXIe8902);
    } // setupUi

    void retranslateUi(QWidget *PXIe8902)
    {
        PXIe8902->setWindowTitle(QCoreApplication::translate("PXIe8902", "Form", nullptr));
        btStart->setText(QCoreApplication::translate("PXIe8902", "\346\265\213\350\257\225", nullptr));
        btStop->setText(QCoreApplication::translate("PXIe8902", "\345\201\234\346\255\242", nullptr));
        pushButton->setText(QCoreApplication::translate("PXIe8902", "\345\216\206\345\217\262\346\225\260\346\215\256", nullptr));
        gbBasicParam->setTitle(QCoreApplication::translate("PXIe8902", "\345\237\272\347\241\200\345\217\230\351\207\217\351\205\215\347\275\256", nullptr));
        label->setText(QCoreApplication::translate("PXIe8902", "\346\265\213\350\257\225\351\241\271\347\233\256", nullptr));
        label_2->setText(QCoreApplication::translate("PXIe8902", "\347\224\265\346\272\220\347\272\277\351\242\221\347\216\207", nullptr));
        label_4->setText(QCoreApplication::translate("PXIe8902", "\350\214\203\345\233\264", nullptr));
        radioButton->setText(QCoreApplication::translate("PXIe8902", "\345\255\224\345\276\204", nullptr));
        cbAperture->setCurrentText(QString());
        radioButton_2->setText(QCoreApplication::translate("PXIe8902", "\345\205\211\345\234\210", nullptr));
        cbNPLC->setCurrentText(QString());
        gbsavedata->setTitle(QCoreApplication::translate("PXIe8902", "\346\225\260\346\215\256\344\277\235\345\255\230", nullptr));
        pbsave->setText(QCoreApplication::translate("PXIe8902", "\344\277\235\345\255\230", nullptr));
        label_8->setText(QCoreApplication::translate("PXIe8902", "\345\244\207\346\263\250", nullptr));
        label_7->setText(QCoreApplication::translate("PXIe8902", "\344\277\235\345\255\230\346\226\271\345\274\217", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PXIe8902: public Ui_PXIe8902 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PXIE8902_H
