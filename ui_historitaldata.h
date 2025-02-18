/********************************************************************************
** Form generated from reading UI file 'historitaldata.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HISTORITALDATA_H
#define UI_HISTORITALDATA_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_HistoritalData
{
public:
    QGridLayout *gridLayout;
    QPushButton *pbrefresh;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *horizontalSpacer;
    QTableView *tableView;

    void setupUi(QWidget *HistoritalData)
    {
        if (HistoritalData->objectName().isEmpty())
            HistoritalData->setObjectName("HistoritalData");
        HistoritalData->resize(400, 300);
        gridLayout = new QGridLayout(HistoritalData);
        gridLayout->setObjectName("gridLayout");
        pbrefresh = new QPushButton(HistoritalData);
        pbrefresh->setObjectName("pbrefresh");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(pbrefresh->sizePolicy().hasHeightForWidth());
        pbrefresh->setSizePolicy(sizePolicy);

        gridLayout->addWidget(pbrefresh, 1, 1, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 1, 2, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer, 1, 0, 1, 1);

        tableView = new QTableView(HistoritalData);
        tableView->setObjectName("tableView");
        QFont font;
        font.setHintingPreference(QFont::PreferDefaultHinting);
        tableView->setFont(font);

        gridLayout->addWidget(tableView, 0, 0, 1, 3);


        retranslateUi(HistoritalData);

        QMetaObject::connectSlotsByName(HistoritalData);
    } // setupUi

    void retranslateUi(QWidget *HistoritalData)
    {
        HistoritalData->setWindowTitle(QCoreApplication::translate("HistoritalData", "Form", nullptr));
        pbrefresh->setText(QCoreApplication::translate("HistoritalData", "\345\210\267\346\226\260", nullptr));
    } // retranslateUi

};

namespace Ui {
    class HistoritalData: public Ui_HistoritalData {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HISTORITALDATA_H
