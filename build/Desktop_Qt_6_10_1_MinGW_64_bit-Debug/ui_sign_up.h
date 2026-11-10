/********************************************************************************
** Form generated from reading UI file 'sign_up.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SIGN_UP_H
#define UI_SIGN_UP_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>

QT_BEGIN_NAMESPACE

class Ui_Sign_Up
{
public:

    void setupUi(QDialog *Sign_Up)
    {
        if (Sign_Up->objectName().isEmpty())
            Sign_Up->setObjectName("Sign_Up");
        Sign_Up->resize(800, 600);

        retranslateUi(Sign_Up);

        QMetaObject::connectSlotsByName(Sign_Up);
    } // setupUi

    void retranslateUi(QDialog *Sign_Up)
    {
        Sign_Up->setWindowTitle(QCoreApplication::translate("Sign_Up", "Sign_Up", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Sign_Up: public Ui_Sign_Up {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SIGN_UP_H
