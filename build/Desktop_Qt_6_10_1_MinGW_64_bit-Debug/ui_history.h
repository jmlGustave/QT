/********************************************************************************
** Form generated from reading UI file 'history.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HISTORY_H
#define UI_HISTORY_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_history
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *history)
    {
        if (history->objectName().isEmpty())
            history->setObjectName("history");
        history->resize(800, 600);
        centralwidget = new QWidget(history);
        centralwidget->setObjectName("centralwidget");
        history->setCentralWidget(centralwidget);
        menubar = new QMenuBar(history);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 26));
        history->setMenuBar(menubar);
        statusbar = new QStatusBar(history);
        statusbar->setObjectName("statusbar");
        history->setStatusBar(statusbar);

        retranslateUi(history);

        QMetaObject::connectSlotsByName(history);
    } // setupUi

    void retranslateUi(QMainWindow *history)
    {
        history->setWindowTitle(QCoreApplication::translate("history", "history", nullptr));
    } // retranslateUi

};

namespace Ui {
    class history: public Ui_history {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HISTORY_H
