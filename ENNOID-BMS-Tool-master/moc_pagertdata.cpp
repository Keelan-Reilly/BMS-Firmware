/****************************************************************************
** Meta object code from reading C++ file 'pagertdata.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.18)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "pages/pagertdata.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'pagertdata.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.18. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_PageRtData_t {
    QByteArrayData data[22];
    char stringdata0[333];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PageRtData_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PageRtData_t qt_meta_stringdata_PageRtData = {
    {
QT_MOC_LITERAL(0, 0, 10), // "PageRtData"
QT_MOC_LITERAL(1, 11, 9), // "timerSlot"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 14), // "valuesReceived"
QT_MOC_LITERAL(4, 37, 10), // "BMS_VALUES"
QT_MOC_LITERAL(5, 48, 6), // "values"
QT_MOC_LITERAL(6, 55, 13), // "cellsReceived"
QT_MOC_LITERAL(7, 69, 9), // "cellCount"
QT_MOC_LITERAL(8, 79, 15), // "QVector<double>"
QT_MOC_LITERAL(9, 95, 16), // "cellVoltageArray"
QT_MOC_LITERAL(10, 112, 11), // "auxReceived"
QT_MOC_LITERAL(11, 124, 8), // "auxCount"
QT_MOC_LITERAL(12, 133, 15), // "auxVoltageArray"
QT_MOC_LITERAL(13, 149, 15), // "expTempReceived"
QT_MOC_LITERAL(14, 165, 12), // "expTempCount"
QT_MOC_LITERAL(15, 178, 19), // "expTempVoltageArray"
QT_MOC_LITERAL(16, 198, 22), // "on_zoomHButton_toggled"
QT_MOC_LITERAL(17, 221, 7), // "checked"
QT_MOC_LITERAL(18, 229, 22), // "on_zoomVButton_toggled"
QT_MOC_LITERAL(19, 252, 24), // "on_rescaleButton_clicked"
QT_MOC_LITERAL(20, 277, 25), // "on_tempShowBMSBox_toggled"
QT_MOC_LITERAL(21, 303, 29) // "on_tempShowBatteryBox_toggled"

    },
    "PageRtData\0timerSlot\0\0valuesReceived\0"
    "BMS_VALUES\0values\0cellsReceived\0"
    "cellCount\0QVector<double>\0cellVoltageArray\0"
    "auxReceived\0auxCount\0auxVoltageArray\0"
    "expTempReceived\0expTempCount\0"
    "expTempVoltageArray\0on_zoomHButton_toggled\0"
    "checked\0on_zoomVButton_toggled\0"
    "on_rescaleButton_clicked\0"
    "on_tempShowBMSBox_toggled\0"
    "on_tempShowBatteryBox_toggled"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PageRtData[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x08 /* Private */,
       3,    1,   65,    2, 0x08 /* Private */,
       6,    2,   68,    2, 0x08 /* Private */,
      10,    2,   73,    2, 0x08 /* Private */,
      13,    2,   78,    2, 0x08 /* Private */,
      16,    1,   83,    2, 0x08 /* Private */,
      18,    1,   86,    2, 0x08 /* Private */,
      19,    0,   89,    2, 0x08 /* Private */,
      20,    1,   90,    2, 0x08 /* Private */,
      21,    1,   93,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 8,    7,    9,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 8,   11,   12,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 8,   14,   15,
    QMetaType::Void, QMetaType::Bool,   17,
    QMetaType::Void, QMetaType::Bool,   17,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   17,
    QMetaType::Void, QMetaType::Bool,   17,

       0        // eod
};

void PageRtData::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PageRtData *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->timerSlot(); break;
        case 1: _t->valuesReceived((*reinterpret_cast< BMS_VALUES(*)>(_a[1]))); break;
        case 2: _t->cellsReceived((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QVector<double>(*)>(_a[2]))); break;
        case 3: _t->auxReceived((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QVector<double>(*)>(_a[2]))); break;
        case 4: _t->expTempReceived((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QVector<double>(*)>(_a[2]))); break;
        case 5: _t->on_zoomHButton_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->on_zoomVButton_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->on_rescaleButton_clicked(); break;
        case 8: _t->on_tempShowBMSBox_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->on_tempShowBatteryBox_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< BMS_VALUES >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<double> >(); break;
            }
            break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<double> >(); break;
            }
            break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<double> >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject PageRtData::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_PageRtData.data,
    qt_meta_data_PageRtData,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *PageRtData::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PageRtData::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PageRtData.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int PageRtData::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
