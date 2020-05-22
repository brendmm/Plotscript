/****************************************************************************
** Meta object code from reading C++ file 'notebook_app.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../notebook_app.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'notebook_app.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_NotebookApp_t {
    QByteArrayData data[12];
    char stringdata0[116];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_NotebookApp_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_NotebookApp_t qt_meta_stringdata_NotebookApp = {
    {
QT_MOC_LITERAL(0, 0, 11), // "NotebookApp"
QT_MOC_LITERAL(1, 12, 10), // "sendOutput"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 10), // "Expression"
QT_MOC_LITERAL(4, 35, 3), // "exp"
QT_MOC_LITERAL(5, 39, 10), // "realChange"
QT_MOC_LITERAL(6, 50, 7), // "command"
QT_MOC_LITERAL(7, 58, 6), // "myLoop"
QT_MOC_LITERAL(8, 65, 11), // "handleStart"
QT_MOC_LITERAL(9, 77, 10), // "handleStop"
QT_MOC_LITERAL(10, 88, 11), // "handleReset"
QT_MOC_LITERAL(11, 100, 15) // "handleInterrupt"

    },
    "NotebookApp\0sendOutput\0\0Expression\0"
    "exp\0realChange\0command\0myLoop\0handleStart\0"
    "handleStop\0handleReset\0handleInterrupt"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_NotebookApp[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,   52,    2, 0x0a /* Public */,
       7,    0,   55,    2, 0x0a /* Public */,
       8,    0,   56,    2, 0x0a /* Public */,
       9,    0,   57,    2, 0x0a /* Public */,
      10,    0,   58,    2, 0x0a /* Public */,
      11,    0,   59,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void NotebookApp::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        NotebookApp *_t = static_cast<NotebookApp *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sendOutput((*reinterpret_cast< Expression(*)>(_a[1]))); break;
        case 1: _t->realChange((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->myLoop(); break;
        case 3: _t->handleStart(); break;
        case 4: _t->handleStop(); break;
        case 5: _t->handleReset(); break;
        case 6: _t->handleInterrupt(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (NotebookApp::*)(Expression );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NotebookApp::sendOutput)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject NotebookApp::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_NotebookApp.data,
      qt_meta_data_NotebookApp,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *NotebookApp::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NotebookApp::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_NotebookApp.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int NotebookApp::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void NotebookApp::sendOutput(Expression _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
