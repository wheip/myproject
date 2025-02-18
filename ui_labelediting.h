/********************************************************************************
** Form generated from reading UI file 'labelediting.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LABELEDITING_H
#define UI_LABELEDITING_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LabelEditing
{
public:

    void setupUi(QWidget *LabelEditing)
    {
        if (LabelEditing->objectName().isEmpty())
            LabelEditing->setObjectName("LabelEditing");
        LabelEditing->resize(400, 300);

        retranslateUi(LabelEditing);

        QMetaObject::connectSlotsByName(LabelEditing);
    } // setupUi

    void retranslateUi(QWidget *LabelEditing)
    {
        LabelEditing->setWindowTitle(QCoreApplication::translate("LabelEditing", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LabelEditing: public Ui_LabelEditing {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LABELEDITING_H
