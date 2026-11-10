/********************************************************************************
** Form generated from reading UI file 'map.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAP_H
#define UI_MAP_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_map
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *map)
    {
        if (map->objectName().isEmpty())
            map->setObjectName("map");
        map->resize(800, 600);
        centralwidget = new QWidget(map);
        centralwidget->setObjectName("centralwidget");
        map->setCentralWidget(centralwidget);
        menubar = new QMenuBar(map);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 26));
        map->setMenuBar(menubar);
        statusbar = new QStatusBar(map);
        statusbar->setObjectName("statusbar");
        map->setStatusBar(statusbar);

        retranslateUi(map);

        QMetaObject::connectSlotsByName(map);
    } // setupUi

    void retranslateUi(QMainWindow *map)
    {
        map->setWindowTitle(QCoreApplication::translate("map", "map", nullptr));
    } // retranslateUi

};

namespace Ui {
    class map: public Ui_map {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAP_H
