/****************************************************************************
** Meta object code from reading C++ file 'utility.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.18)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "utility.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'utility.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.18. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Utility_t {
    QByteArrayData data[20];
    char stringdata0[182];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Utility_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Utility_t qt_meta_stringdata_Utility = {
    {
QT_MOC_LITERAL(0, 0, 7), // "Utility"
QT_MOC_LITERAL(1, 8, 12), // "checkVersion"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 13), // "BMSInterface*"
QT_MOC_LITERAL(4, 36, 8), // "dieBieMS"
QT_MOC_LITERAL(5, 45, 11), // "fwChangeLog"
QT_MOC_LITERAL(6, 57, 17), // "vescToolChangeLog"
QT_MOC_LITERAL(7, 75, 9), // "aboutText"
QT_MOC_LITERAL(8, 85, 8), // "uuid2Str"
QT_MOC_LITERAL(9, 94, 4), // "uuid"
QT_MOC_LITERAL(10, 99, 5), // "space"
QT_MOC_LITERAL(11, 105, 21), // "requestFilePermission"
QT_MOC_LITERAL(12, 127, 10), // "waitSignal"
QT_MOC_LITERAL(13, 138, 6), // "sender"
QT_MOC_LITERAL(14, 145, 6), // "signal"
QT_MOC_LITERAL(15, 152, 9), // "timeoutMs"
QT_MOC_LITERAL(16, 162, 11), // "almostEqual"
QT_MOC_LITERAL(17, 174, 1), // "A"
QT_MOC_LITERAL(18, 176, 1), // "B"
QT_MOC_LITERAL(19, 178, 3) // "eps"

    },
    "Utility\0checkVersion\0\0BMSInterface*\0"
    "dieBieMS\0fwChangeLog\0vescToolChangeLog\0"
    "aboutText\0uuid2Str\0uuid\0space\0"
    "requestFilePermission\0waitSignal\0"
    "sender\0signal\0timeoutMs\0almostEqual\0"
    "A\0B\0eps"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Utility[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // methods: name, argc, parameters, tag, flags
       1,    1,   59,    2, 0x02 /* Public */,
       1,    0,   62,    2, 0x22 /* Public | MethodCloned */,
       5,    0,   63,    2, 0x02 /* Public */,
       6,    0,   64,    2, 0x02 /* Public */,
       7,    0,   65,    2, 0x02 /* Public */,
       8,    2,   66,    2, 0x02 /* Public */,
      11,    0,   71,    2, 0x02 /* Public */,
      12,    3,   72,    2, 0x02 /* Public */,
      16,    3,   79,    2, 0x02 /* Public */,

 // methods: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::QString,
    QMetaType::QString,
    QMetaType::QString,
    QMetaType::QString, QMetaType::QByteArray, QMetaType::Bool,    9,   10,
    QMetaType::Bool,
    QMetaType::Bool, QMetaType::QObjectStar, QMetaType::QString, QMetaType::Int,   13,   14,   15,
    QMetaType::Bool, QMetaType::Double, QMetaType::Double, QMetaType::Double,   17,   18,   19,

       0        // eod
};

void Utility::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Utility *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->checkVersion((*reinterpret_cast< BMSInterface*(*)>(_a[1]))); break;
        case 1: _t->checkVersion(); break;
        case 2: { QString _r = _t->fwChangeLog();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 3: { QString _r = _t->vescToolChangeLog();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 4: { QString _r = _t->aboutText();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 5: { QString _r = _t->uuid2Str((*reinterpret_cast< QByteArray(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 6: { bool _r = _t->requestFilePermission();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 7: { bool _r = _t->waitSignal((*reinterpret_cast< QObject*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 8: { bool _r = _t->almostEqual((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< BMSInterface* >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Utility::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_Utility.data,
    qt_meta_data_Utility,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Utility::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Utility::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Utility.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Utility::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
