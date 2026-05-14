/****************************************************************************
** Meta object code from reading C++ file 'bleuart.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.18)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "bleuart.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'bleuart.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.18. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_BleUart_t {
    QByteArrayData data[47];
    char stringdata0[646];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_BleUart_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_BleUart_t qt_meta_stringdata_BleUart = {
    {
QT_MOC_LITERAL(0, 0, 7), // "BleUart"
QT_MOC_LITERAL(1, 8, 6), // "dataRx"
QT_MOC_LITERAL(2, 15, 0), // ""
QT_MOC_LITERAL(3, 16, 4), // "data"
QT_MOC_LITERAL(4, 21, 8), // "scanDone"
QT_MOC_LITERAL(5, 30, 4), // "devs"
QT_MOC_LITERAL(6, 35, 4), // "done"
QT_MOC_LITERAL(7, 40, 8), // "bleError"
QT_MOC_LITERAL(8, 49, 4), // "info"
QT_MOC_LITERAL(9, 54, 9), // "connected"
QT_MOC_LITERAL(10, 64, 9), // "writeData"
QT_MOC_LITERAL(11, 74, 9), // "addDevice"
QT_MOC_LITERAL(12, 84, 20), // "QBluetoothDeviceInfo"
QT_MOC_LITERAL(13, 105, 3), // "dev"
QT_MOC_LITERAL(14, 109, 12), // "scanFinished"
QT_MOC_LITERAL(15, 122, 15), // "deviceScanError"
QT_MOC_LITERAL(16, 138, 37), // "QBluetoothDeviceDiscoveryAgen..."
QT_MOC_LITERAL(17, 176, 1), // "e"
QT_MOC_LITERAL(18, 178, 17), // "serviceDiscovered"
QT_MOC_LITERAL(19, 196, 14), // "QBluetoothUuid"
QT_MOC_LITERAL(20, 211, 15), // "serviceScanDone"
QT_MOC_LITERAL(21, 227, 15), // "controllerError"
QT_MOC_LITERAL(22, 243, 27), // "QLowEnergyController::Error"
QT_MOC_LITERAL(23, 271, 15), // "deviceConnected"
QT_MOC_LITERAL(24, 287, 18), // "deviceDisconnected"
QT_MOC_LITERAL(25, 306, 19), // "serviceStateChanged"
QT_MOC_LITERAL(26, 326, 31), // "QLowEnergyService::ServiceState"
QT_MOC_LITERAL(27, 358, 1), // "s"
QT_MOC_LITERAL(28, 360, 10), // "updateData"
QT_MOC_LITERAL(29, 371, 24), // "QLowEnergyCharacteristic"
QT_MOC_LITERAL(30, 396, 1), // "c"
QT_MOC_LITERAL(31, 398, 5), // "value"
QT_MOC_LITERAL(32, 404, 24), // "confirmedDescriptorWrite"
QT_MOC_LITERAL(33, 429, 20), // "QLowEnergyDescriptor"
QT_MOC_LITERAL(34, 450, 1), // "d"
QT_MOC_LITERAL(35, 452, 19), // "controlStateChanged"
QT_MOC_LITERAL(36, 472, 37), // "QLowEnergyController::Control..."
QT_MOC_LITERAL(37, 510, 5), // "state"
QT_MOC_LITERAL(38, 516, 17), // "connectionUpdated"
QT_MOC_LITERAL(39, 534, 30), // "QLowEnergyConnectionParameters"
QT_MOC_LITERAL(40, 565, 13), // "newParameters"
QT_MOC_LITERAL(41, 579, 9), // "startScan"
QT_MOC_LITERAL(42, 589, 12), // "startConnect"
QT_MOC_LITERAL(43, 602, 4), // "addr"
QT_MOC_LITERAL(44, 607, 13), // "disconnectBle"
QT_MOC_LITERAL(45, 621, 11), // "isConnected"
QT_MOC_LITERAL(46, 633, 12) // "isConnecting"

    },
    "BleUart\0dataRx\0\0data\0scanDone\0devs\0"
    "done\0bleError\0info\0connected\0writeData\0"
    "addDevice\0QBluetoothDeviceInfo\0dev\0"
    "scanFinished\0deviceScanError\0"
    "QBluetoothDeviceDiscoveryAgent::Error\0"
    "e\0serviceDiscovered\0QBluetoothUuid\0"
    "serviceScanDone\0controllerError\0"
    "QLowEnergyController::Error\0deviceConnected\0"
    "deviceDisconnected\0serviceStateChanged\0"
    "QLowEnergyService::ServiceState\0s\0"
    "updateData\0QLowEnergyCharacteristic\0"
    "c\0value\0confirmedDescriptorWrite\0"
    "QLowEnergyDescriptor\0d\0controlStateChanged\0"
    "QLowEnergyController::ControllerState\0"
    "state\0connectionUpdated\0"
    "QLowEnergyConnectionParameters\0"
    "newParameters\0startScan\0startConnect\0"
    "addr\0disconnectBle\0isConnected\0"
    "isConnecting"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_BleUart[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      23,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  129,    2, 0x06 /* Public */,
       4,    2,  132,    2, 0x06 /* Public */,
       7,    1,  137,    2, 0x06 /* Public */,
       9,    0,  140,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    1,  141,    2, 0x0a /* Public */,
      11,    1,  144,    2, 0x08 /* Private */,
      14,    0,  147,    2, 0x08 /* Private */,
      15,    1,  148,    2, 0x08 /* Private */,
      18,    1,  151,    2, 0x08 /* Private */,
      20,    0,  154,    2, 0x08 /* Private */,
      21,    1,  155,    2, 0x08 /* Private */,
      23,    0,  158,    2, 0x08 /* Private */,
      24,    0,  159,    2, 0x08 /* Private */,
      25,    1,  160,    2, 0x08 /* Private */,
      28,    2,  163,    2, 0x08 /* Private */,
      32,    2,  168,    2, 0x08 /* Private */,
      35,    1,  173,    2, 0x08 /* Private */,
      38,    1,  176,    2, 0x08 /* Private */,

 // methods: name, argc, parameters, tag, flags
      41,    0,  179,    2, 0x02 /* Public */,
      42,    1,  180,    2, 0x02 /* Public */,
      44,    0,  183,    2, 0x02 /* Public */,
      45,    0,  184,    2, 0x02 /* Public */,
      46,    0,  185,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QByteArray,    3,
    QMetaType::Void, QMetaType::QVariantMap, QMetaType::Bool,    5,    6,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::QByteArray,    3,
    QMetaType::Void, 0x80000000 | 12,   13,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void, 0x80000000 | 19,    2,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 22,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 26,   27,
    QMetaType::Void, 0x80000000 | 29, QMetaType::QByteArray,   30,   31,
    QMetaType::Void, 0x80000000 | 33, QMetaType::QByteArray,   34,   31,
    QMetaType::Void, 0x80000000 | 36,   37,
    QMetaType::Void, 0x80000000 | 39,   40,

 // methods: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   43,
    QMetaType::Void,
    QMetaType::Bool,
    QMetaType::Bool,

       0        // eod
};

void BleUart::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<BleUart *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->dataRx((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 1: _t->scanDone((*reinterpret_cast< QVariantMap(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 2: _t->bleError((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->connected(); break;
        case 4: _t->writeData((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 5: _t->addDevice((*reinterpret_cast< const QBluetoothDeviceInfo(*)>(_a[1]))); break;
        case 6: _t->scanFinished(); break;
        case 7: _t->deviceScanError((*reinterpret_cast< QBluetoothDeviceDiscoveryAgent::Error(*)>(_a[1]))); break;
        case 8: _t->serviceDiscovered((*reinterpret_cast< const QBluetoothUuid(*)>(_a[1]))); break;
        case 9: _t->serviceScanDone(); break;
        case 10: _t->controllerError((*reinterpret_cast< QLowEnergyController::Error(*)>(_a[1]))); break;
        case 11: _t->deviceConnected(); break;
        case 12: _t->deviceDisconnected(); break;
        case 13: _t->serviceStateChanged((*reinterpret_cast< QLowEnergyService::ServiceState(*)>(_a[1]))); break;
        case 14: _t->updateData((*reinterpret_cast< const QLowEnergyCharacteristic(*)>(_a[1])),(*reinterpret_cast< const QByteArray(*)>(_a[2]))); break;
        case 15: _t->confirmedDescriptorWrite((*reinterpret_cast< const QLowEnergyDescriptor(*)>(_a[1])),(*reinterpret_cast< const QByteArray(*)>(_a[2]))); break;
        case 16: _t->controlStateChanged((*reinterpret_cast< QLowEnergyController::ControllerState(*)>(_a[1]))); break;
        case 17: _t->connectionUpdated((*reinterpret_cast< const QLowEnergyConnectionParameters(*)>(_a[1]))); break;
        case 18: _t->startScan(); break;
        case 19: _t->startConnect((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 20: _t->disconnectBle(); break;
        case 21: { bool _r = _t->isConnected();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 22: { bool _r = _t->isConnecting();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QBluetoothDeviceInfo >(); break;
            }
            break;
        case 8:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QBluetoothUuid >(); break;
            }
            break;
        case 10:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QLowEnergyController::Error >(); break;
            }
            break;
        case 13:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QLowEnergyService::ServiceState >(); break;
            }
            break;
        case 14:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QLowEnergyCharacteristic >(); break;
            }
            break;
        case 15:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QLowEnergyDescriptor >(); break;
            }
            break;
        case 16:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QLowEnergyController::ControllerState >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (BleUart::*)(QByteArray );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BleUart::dataRx)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (BleUart::*)(QVariantMap , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BleUart::scanDone)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (BleUart::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BleUart::bleError)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (BleUart::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BleUart::connected)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject BleUart::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_BleUart.data,
    qt_meta_data_BleUart,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *BleUart::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BleUart::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_BleUart.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int BleUart::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 23)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 23;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 23)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 23;
    }
    return _id;
}

// SIGNAL 0
void BleUart::dataRx(QByteArray _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void BleUart::scanDone(QVariantMap _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void BleUart::bleError(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void BleUart::connected()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
