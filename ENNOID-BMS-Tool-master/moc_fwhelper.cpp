/****************************************************************************
** Meta object code from reading C++ file 'fwhelper.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.18)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "mobile/fwhelper.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'fwhelper.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.18. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_FwHelper_t {
    QByteArrayData data[12];
    char stringdata0[125];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FwHelper_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FwHelper_t qt_meta_stringdata_FwHelper = {
    {
QT_MOC_LITERAL(0, 0, 8), // "FwHelper"
QT_MOC_LITERAL(1, 9, 12), // "getHardwares"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 2), // "hw"
QT_MOC_LITERAL(4, 26, 12), // "getFirmwares"
QT_MOC_LITERAL(5, 39, 14), // "getBootloaders"
QT_MOC_LITERAL(6, 54, 14), // "uploadFirmware"
QT_MOC_LITERAL(7, 69, 8), // "filename"
QT_MOC_LITERAL(8, 78, 13), // "BMSInterface*"
QT_MOC_LITERAL(9, 92, 8), // "dieBieMS"
QT_MOC_LITERAL(10, 101, 12), // "isBootloader"
QT_MOC_LITERAL(11, 114, 10) // "isIncluded"

    },
    "FwHelper\0getHardwares\0\0hw\0getFirmwares\0"
    "getBootloaders\0uploadFirmware\0filename\0"
    "BMSInterface*\0dieBieMS\0isBootloader\0"
    "isIncluded"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FwHelper[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // methods: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x02 /* Public */,
       1,    0,   42,    2, 0x22 /* Public | MethodCloned */,
       4,    1,   43,    2, 0x02 /* Public */,
       5,    1,   46,    2, 0x02 /* Public */,
       6,    4,   49,    2, 0x02 /* Public */,

 // methods: parameters
    QMetaType::QVariantMap, QMetaType::QString,    3,
    QMetaType::QVariantMap,
    QMetaType::QVariantMap, QMetaType::QString,    3,
    QMetaType::QVariantMap, QMetaType::QString,    3,
    QMetaType::Bool, QMetaType::QString, 0x80000000 | 8, QMetaType::Bool, QMetaType::Bool,    7,    9,   10,   11,

       0        // eod
};

void FwHelper::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FwHelper *>(_o);
        (void)_t;
        switch (_id) {
        case 0: { QVariantMap _r = _t->getHardwares((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 1: { QVariantMap _r = _t->getHardwares();
            if (_a[0]) *reinterpret_cast< QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 2: { QVariantMap _r = _t->getFirmwares((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 3: { QVariantMap _r = _t->getBootloaders((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 4: { bool _r = _t->uploadFirmware((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< BMSInterface*(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< BMSInterface* >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject FwHelper::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_FwHelper.data,
    qt_meta_data_FwHelper,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *FwHelper::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FwHelper::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FwHelper.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int FwHelper::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
