/****************************************************************************
** Meta object code from reading C++ file 'customheaderbar.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../customheaderbar.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'customheaderbar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN15CustomHeaderBarE_t {};
} // unnamed namespace

template <> constexpr inline auto CustomHeaderBar::qt_create_metaobjectdata<qt_meta_tag_ZN15CustomHeaderBarE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "CustomHeaderBar",
        "profileOptionSelected",
        "",
        "optionId",
        "loginOptionSelected",
        "registrationRequested",
        "editRequested",
        "user_send",
        "historyRequested",
        "User*",
        "u",
        "showProfileMenu",
        "handleMenuAction",
        "QAction*",
        "action",
        "showLoginMenu",
        "handleLoginAction",
        "log_out",
        "showHelp"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'profileOptionSelected'
        QtMocHelpers::SignalData<void(int)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Signal 'loginOptionSelected'
        QtMocHelpers::SignalData<void(int)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Signal 'registrationRequested'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'editRequested'
        QtMocHelpers::SignalData<void(QString)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Signal 'historyRequested'
        QtMocHelpers::SignalData<void(User *)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 9, 10 },
        }}),
        // Slot 'showProfileMenu'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'handleMenuAction'
        QtMocHelpers::SlotData<void(QAction *)>(12, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 13, 14 },
        }}),
        // Slot 'showLoginMenu'
        QtMocHelpers::SlotData<void()>(15, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'handleLoginAction'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'log_out'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'showHelp'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<CustomHeaderBar, qt_meta_tag_ZN15CustomHeaderBarE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject CustomHeaderBar::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15CustomHeaderBarE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15CustomHeaderBarE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN15CustomHeaderBarE_t>.metaTypes,
    nullptr
} };

void CustomHeaderBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<CustomHeaderBar *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->profileOptionSelected((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->loginOptionSelected((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->registrationRequested(); break;
        case 3: _t->editRequested((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->historyRequested((*reinterpret_cast<std::add_pointer_t<User*>>(_a[1]))); break;
        case 5: _t->showProfileMenu(); break;
        case 6: _t->handleMenuAction((*reinterpret_cast<std::add_pointer_t<QAction*>>(_a[1]))); break;
        case 7: _t->showLoginMenu(); break;
        case 8: _t->handleLoginAction(); break;
        case 9: _t->log_out(); break;
        case 10: _t->showHelp(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAction* >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (CustomHeaderBar::*)(int )>(_a, &CustomHeaderBar::profileOptionSelected, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (CustomHeaderBar::*)(int )>(_a, &CustomHeaderBar::loginOptionSelected, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (CustomHeaderBar::*)()>(_a, &CustomHeaderBar::registrationRequested, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (CustomHeaderBar::*)(QString )>(_a, &CustomHeaderBar::editRequested, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (CustomHeaderBar::*)(User * )>(_a, &CustomHeaderBar::historyRequested, 4))
            return;
    }
}

const QMetaObject *CustomHeaderBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CustomHeaderBar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15CustomHeaderBarE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int CustomHeaderBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void CustomHeaderBar::profileOptionSelected(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void CustomHeaderBar::loginOptionSelected(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void CustomHeaderBar::registrationRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void CustomHeaderBar::editRequested(QString _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void CustomHeaderBar::historyRequested(User * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}
QT_WARNING_POP
