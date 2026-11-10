/********************************************************************************
** Form generated from reading UI file 'problem_list.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROBLEM_LIST_H
#define UI_PROBLEM_LIST_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_problem_list
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *problem_list)
    {
        if (problem_list->objectName().isEmpty())
            problem_list->setObjectName("problem_list");
        problem_list->resize(800, 600);
        centralwidget = new QWidget(problem_list);
        centralwidget->setObjectName("centralwidget");
        problem_list->setCentralWidget(centralwidget);
        menubar = new QMenuBar(problem_list);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 26));
        problem_list->setMenuBar(menubar);
        statusbar = new QStatusBar(problem_list);
        statusbar->setObjectName("statusbar");
        problem_list->setStatusBar(statusbar);

        retranslateUi(problem_list);

        QMetaObject::connectSlotsByName(problem_list);
    } // setupUi

    void retranslateUi(QMainWindow *problem_list)
    {
        problem_list->setWindowTitle(QCoreApplication::translate("problem_list", "problem_list", nullptr));
    } // retranslateUi

};

namespace Ui {
    class problem_list: public Ui_problem_list {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROBLEM_LIST_H
