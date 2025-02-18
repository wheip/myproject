/********************************************************************************
** Form generated from reading UI file 'taskmanagenew.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TASKMANAGENEW_H
#define UI_TASKMANAGENEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TaskManageNew
{
public:

    void setupUi(QWidget *TaskManageNew)
    {
        if (TaskManageNew->objectName().isEmpty())
            TaskManageNew->setObjectName("TaskManageNew");
        TaskManageNew->resize(1173, 918);

        retranslateUi(TaskManageNew);

        QMetaObject::connectSlotsByName(TaskManageNew);
    } // setupUi

    void retranslateUi(QWidget *TaskManageNew)
    {
        TaskManageNew->setWindowTitle(QCoreApplication::translate("TaskManageNew", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TaskManageNew: public Ui_TaskManageNew {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TASKMANAGENEW_H
