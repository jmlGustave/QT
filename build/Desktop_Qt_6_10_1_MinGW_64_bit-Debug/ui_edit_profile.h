/********************************************************************************
** Form generated from reading UI file 'edit_profile.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EDIT_PROFILE_H
#define UI_EDIT_PROFILE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_edit_profile
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *edit_profile)
    {
        if (edit_profile->objectName().isEmpty())
            edit_profile->setObjectName("edit_profile");
        edit_profile->resize(800, 600);
        centralwidget = new QWidget(edit_profile);
        centralwidget->setObjectName("centralwidget");
        edit_profile->setCentralWidget(centralwidget);
        menubar = new QMenuBar(edit_profile);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 26));
        edit_profile->setMenuBar(menubar);
        statusbar = new QStatusBar(edit_profile);
        statusbar->setObjectName("statusbar");
        edit_profile->setStatusBar(statusbar);

        retranslateUi(edit_profile);

        QMetaObject::connectSlotsByName(edit_profile);
    } // setupUi

    void retranslateUi(QMainWindow *edit_profile)
    {
        edit_profile->setWindowTitle(QCoreApplication::translate("edit_profile", "edit_profile", nullptr));
    } // retranslateUi

};

namespace Ui {
    class edit_profile: public Ui_edit_profile {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EDIT_PROFILE_H
