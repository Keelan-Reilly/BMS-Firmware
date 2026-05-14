/****************************************************************************
** Meta object code from reading C++ file 'datatypes.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.18)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "datatypes.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'datatypes.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.18. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MC_VALUES_t {
    QByteArrayData data[18];
    char stringdata0[182];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MC_VALUES_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MC_VALUES_t qt_meta_stringdata_MC_VALUES = {
    {
QT_MOC_LITERAL(0, 0, 9), // "MC_VALUES"
QT_MOC_LITERAL(1, 10, 4), // "v_in"
QT_MOC_LITERAL(2, 15, 8), // "temp_mos"
QT_MOC_LITERAL(3, 24, 10), // "temp_motor"
QT_MOC_LITERAL(4, 35, 13), // "current_motor"
QT_MOC_LITERAL(5, 49, 10), // "current_in"
QT_MOC_LITERAL(6, 60, 2), // "id"
QT_MOC_LITERAL(7, 63, 2), // "iq"
QT_MOC_LITERAL(8, 66, 3), // "rpm"
QT_MOC_LITERAL(9, 70, 8), // "duty_now"
QT_MOC_LITERAL(10, 79, 9), // "amp_hours"
QT_MOC_LITERAL(11, 89, 17), // "amp_hours_charged"
QT_MOC_LITERAL(12, 107, 10), // "watt_hours"
QT_MOC_LITERAL(13, 118, 18), // "watt_hours_charged"
QT_MOC_LITERAL(14, 137, 10), // "tachometer"
QT_MOC_LITERAL(15, 148, 14), // "tachometer_abs"
QT_MOC_LITERAL(16, 163, 8), // "position"
QT_MOC_LITERAL(17, 172, 9) // "fault_str"

    },
    "MC_VALUES\0v_in\0temp_mos\0temp_motor\0"
    "current_motor\0current_in\0id\0iq\0rpm\0"
    "duty_now\0amp_hours\0amp_hours_charged\0"
    "watt_hours\0watt_hours_charged\0tachometer\0"
    "tachometer_abs\0position\0fault_str"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MC_VALUES[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
      17,   14, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       4,       // flags
       0,       // signalCount

 // properties: name, type, flags
       1, QMetaType::Double, 0x00095003,
       2, QMetaType::Double, 0x00095003,
       3, QMetaType::Double, 0x00095003,
       4, QMetaType::Double, 0x00095003,
       5, QMetaType::Double, 0x00095003,
       6, QMetaType::Double, 0x00095003,
       7, QMetaType::Double, 0x00095003,
       8, QMetaType::Double, 0x00095003,
       9, QMetaType::Double, 0x00095003,
      10, QMetaType::Double, 0x00095003,
      11, QMetaType::Double, 0x00095003,
      12, QMetaType::Double, 0x00095003,
      13, QMetaType::Double, 0x00095003,
      14, QMetaType::Int, 0x00095003,
      15, QMetaType::Int, 0x00095003,
      16, QMetaType::Double, 0x00095003,
      17, QMetaType::QString, 0x00095003,

       0        // eod
};

void MC_VALUES::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{

#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty) {
        auto *_t = reinterpret_cast<MC_VALUES *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< double*>(_v) = _t->v_in; break;
        case 1: *reinterpret_cast< double*>(_v) = _t->temp_mos; break;
        case 2: *reinterpret_cast< double*>(_v) = _t->temp_motor; break;
        case 3: *reinterpret_cast< double*>(_v) = _t->current_motor; break;
        case 4: *reinterpret_cast< double*>(_v) = _t->current_in; break;
        case 5: *reinterpret_cast< double*>(_v) = _t->id; break;
        case 6: *reinterpret_cast< double*>(_v) = _t->iq; break;
        case 7: *reinterpret_cast< double*>(_v) = _t->rpm; break;
        case 8: *reinterpret_cast< double*>(_v) = _t->duty_now; break;
        case 9: *reinterpret_cast< double*>(_v) = _t->amp_hours; break;
        case 10: *reinterpret_cast< double*>(_v) = _t->amp_hours_charged; break;
        case 11: *reinterpret_cast< double*>(_v) = _t->watt_hours; break;
        case 12: *reinterpret_cast< double*>(_v) = _t->watt_hours_charged; break;
        case 13: *reinterpret_cast< int*>(_v) = _t->tachometer; break;
        case 14: *reinterpret_cast< int*>(_v) = _t->tachometer_abs; break;
        case 15: *reinterpret_cast< double*>(_v) = _t->position; break;
        case 16: *reinterpret_cast< QString*>(_v) = _t->fault_str; break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = reinterpret_cast<MC_VALUES *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0:
            if (_t->v_in != *reinterpret_cast< double*>(_v)) {
                _t->v_in = *reinterpret_cast< double*>(_v);
            }
            break;
        case 1:
            if (_t->temp_mos != *reinterpret_cast< double*>(_v)) {
                _t->temp_mos = *reinterpret_cast< double*>(_v);
            }
            break;
        case 2:
            if (_t->temp_motor != *reinterpret_cast< double*>(_v)) {
                _t->temp_motor = *reinterpret_cast< double*>(_v);
            }
            break;
        case 3:
            if (_t->current_motor != *reinterpret_cast< double*>(_v)) {
                _t->current_motor = *reinterpret_cast< double*>(_v);
            }
            break;
        case 4:
            if (_t->current_in != *reinterpret_cast< double*>(_v)) {
                _t->current_in = *reinterpret_cast< double*>(_v);
            }
            break;
        case 5:
            if (_t->id != *reinterpret_cast< double*>(_v)) {
                _t->id = *reinterpret_cast< double*>(_v);
            }
            break;
        case 6:
            if (_t->iq != *reinterpret_cast< double*>(_v)) {
                _t->iq = *reinterpret_cast< double*>(_v);
            }
            break;
        case 7:
            if (_t->rpm != *reinterpret_cast< double*>(_v)) {
                _t->rpm = *reinterpret_cast< double*>(_v);
            }
            break;
        case 8:
            if (_t->duty_now != *reinterpret_cast< double*>(_v)) {
                _t->duty_now = *reinterpret_cast< double*>(_v);
            }
            break;
        case 9:
            if (_t->amp_hours != *reinterpret_cast< double*>(_v)) {
                _t->amp_hours = *reinterpret_cast< double*>(_v);
            }
            break;
        case 10:
            if (_t->amp_hours_charged != *reinterpret_cast< double*>(_v)) {
                _t->amp_hours_charged = *reinterpret_cast< double*>(_v);
            }
            break;
        case 11:
            if (_t->watt_hours != *reinterpret_cast< double*>(_v)) {
                _t->watt_hours = *reinterpret_cast< double*>(_v);
            }
            break;
        case 12:
            if (_t->watt_hours_charged != *reinterpret_cast< double*>(_v)) {
                _t->watt_hours_charged = *reinterpret_cast< double*>(_v);
            }
            break;
        case 13:
            if (_t->tachometer != *reinterpret_cast< int*>(_v)) {
                _t->tachometer = *reinterpret_cast< int*>(_v);
            }
            break;
        case 14:
            if (_t->tachometer_abs != *reinterpret_cast< int*>(_v)) {
                _t->tachometer_abs = *reinterpret_cast< int*>(_v);
            }
            break;
        case 15:
            if (_t->position != *reinterpret_cast< double*>(_v)) {
                _t->position = *reinterpret_cast< double*>(_v);
            }
            break;
        case 16:
            if (_t->fault_str != *reinterpret_cast< QString*>(_v)) {
                _t->fault_str = *reinterpret_cast< QString*>(_v);
            }
            break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject MC_VALUES::staticMetaObject = { {
    nullptr,
    qt_meta_stringdata_MC_VALUES.data,
    qt_meta_data_MC_VALUES,
    qt_static_metacall,
    nullptr,
    nullptr
} };

struct qt_meta_stringdata_BMS_VALUES_t {
    QByteArrayData data[31];
    char stringdata0[335];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_BMS_VALUES_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_BMS_VALUES_t qt_meta_stringdata_BMS_VALUES = {
    {
QT_MOC_LITERAL(0, 0, 10), // "BMS_VALUES"
QT_MOC_LITERAL(1, 11, 11), // "packVoltage"
QT_MOC_LITERAL(2, 23, 11), // "packCurrent"
QT_MOC_LITERAL(3, 35, 3), // "soC"
QT_MOC_LITERAL(4, 39, 5), // "AhCnt"
QT_MOC_LITERAL(5, 45, 5), // "WhCnt"
QT_MOC_LITERAL(6, 51, 8), // "AhCntChg"
QT_MOC_LITERAL(7, 60, 8), // "WhCntChg"
QT_MOC_LITERAL(8, 69, 8), // "AhCntDis"
QT_MOC_LITERAL(9, 78, 8), // "WhCntDis"
QT_MOC_LITERAL(10, 87, 6), // "cVHigh"
QT_MOC_LITERAL(11, 94, 9), // "cVAverage"
QT_MOC_LITERAL(12, 104, 5), // "cVLow"
QT_MOC_LITERAL(13, 110, 10), // "cVMisMatch"
QT_MOC_LITERAL(14, 121, 13), // "loadLCVoltage"
QT_MOC_LITERAL(15, 135, 13), // "loadLCCurrent"
QT_MOC_LITERAL(16, 149, 13), // "loadHCVoltage"
QT_MOC_LITERAL(17, 163, 13), // "loadHCCurrent"
QT_MOC_LITERAL(18, 177, 14), // "chargerVoltage"
QT_MOC_LITERAL(19, 192, 10), // "auxVoltage"
QT_MOC_LITERAL(20, 203, 10), // "auxCurrent"
QT_MOC_LITERAL(21, 214, 12), // "tempBattHigh"
QT_MOC_LITERAL(22, 227, 15), // "tempBattAverage"
QT_MOC_LITERAL(23, 243, 11), // "tempBattLow"
QT_MOC_LITERAL(24, 255, 11), // "tempBMSHigh"
QT_MOC_LITERAL(25, 267, 14), // "tempBMSAverage"
QT_MOC_LITERAL(26, 282, 10), // "tempBMSLow"
QT_MOC_LITERAL(27, 293, 8), // "humidity"
QT_MOC_LITERAL(28, 302, 7), // "opState"
QT_MOC_LITERAL(29, 310, 13), // "balanceActive"
QT_MOC_LITERAL(30, 324, 10) // "faultState"

    },
    "BMS_VALUES\0packVoltage\0packCurrent\0"
    "soC\0AhCnt\0WhCnt\0AhCntChg\0WhCntChg\0"
    "AhCntDis\0WhCntDis\0cVHigh\0cVAverage\0"
    "cVLow\0cVMisMatch\0loadLCVoltage\0"
    "loadLCCurrent\0loadHCVoltage\0loadHCCurrent\0"
    "chargerVoltage\0auxVoltage\0auxCurrent\0"
    "tempBattHigh\0tempBattAverage\0tempBattLow\0"
    "tempBMSHigh\0tempBMSAverage\0tempBMSLow\0"
    "humidity\0opState\0balanceActive\0"
    "faultState"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_BMS_VALUES[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
      30,   14, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       4,       // flags
       0,       // signalCount

 // properties: name, type, flags
       1, QMetaType::Double, 0x00095003,
       2, QMetaType::Double, 0x00095003,
       3, QMetaType::Int, 0x00095003,
       4, QMetaType::Double, 0x00095003,
       5, QMetaType::Double, 0x00095003,
       6, QMetaType::Double, 0x00095003,
       7, QMetaType::Double, 0x00095003,
       8, QMetaType::Double, 0x00095003,
       9, QMetaType::Double, 0x00095003,
      10, QMetaType::Double, 0x00095003,
      11, QMetaType::Double, 0x00095003,
      12, QMetaType::Double, 0x00095003,
      13, QMetaType::Double, 0x00095003,
      14, QMetaType::Double, 0x00095003,
      15, QMetaType::Double, 0x00095003,
      16, QMetaType::Double, 0x00095003,
      17, QMetaType::Double, 0x00095003,
      18, QMetaType::Double, 0x00095003,
      19, QMetaType::Double, 0x00095003,
      20, QMetaType::Double, 0x00095003,
      21, QMetaType::Double, 0x00095003,
      22, QMetaType::Double, 0x00095003,
      23, QMetaType::Double, 0x00095003,
      24, QMetaType::Double, 0x00095003,
      25, QMetaType::Double, 0x00095003,
      26, QMetaType::Double, 0x00095003,
      27, QMetaType::Double, 0x00095003,
      28, QMetaType::QString, 0x00095003,
      29, QMetaType::Int, 0x00095003,
      30, QMetaType::QString, 0x00095003,

       0        // eod
};

void BMS_VALUES::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{

#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty) {
        auto *_t = reinterpret_cast<BMS_VALUES *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< double*>(_v) = _t->packVoltage; break;
        case 1: *reinterpret_cast< double*>(_v) = _t->packCurrent; break;
        case 2: *reinterpret_cast< int*>(_v) = _t->soC; break;
        case 3: *reinterpret_cast< double*>(_v) = _t->AhCnt; break;
        case 4: *reinterpret_cast< double*>(_v) = _t->WhCnt; break;
        case 5: *reinterpret_cast< double*>(_v) = _t->AhCntChg; break;
        case 6: *reinterpret_cast< double*>(_v) = _t->WhCntChg; break;
        case 7: *reinterpret_cast< double*>(_v) = _t->AhCntDis; break;
        case 8: *reinterpret_cast< double*>(_v) = _t->WhCntDis; break;
        case 9: *reinterpret_cast< double*>(_v) = _t->cVHigh; break;
        case 10: *reinterpret_cast< double*>(_v) = _t->cVAverage; break;
        case 11: *reinterpret_cast< double*>(_v) = _t->cVLow; break;
        case 12: *reinterpret_cast< double*>(_v) = _t->cVMisMatch; break;
        case 13: *reinterpret_cast< double*>(_v) = _t->loadLCVoltage; break;
        case 14: *reinterpret_cast< double*>(_v) = _t->loadLCCurrent; break;
        case 15: *reinterpret_cast< double*>(_v) = _t->loadHCVoltage; break;
        case 16: *reinterpret_cast< double*>(_v) = _t->loadHCCurrent; break;
        case 17: *reinterpret_cast< double*>(_v) = _t->chargerVoltage; break;
        case 18: *reinterpret_cast< double*>(_v) = _t->auxVoltage; break;
        case 19: *reinterpret_cast< double*>(_v) = _t->auxCurrent; break;
        case 20: *reinterpret_cast< double*>(_v) = _t->tempBattHigh; break;
        case 21: *reinterpret_cast< double*>(_v) = _t->tempBattAverage; break;
        case 22: *reinterpret_cast< double*>(_v) = _t->tempBattLow; break;
        case 23: *reinterpret_cast< double*>(_v) = _t->tempBMSHigh; break;
        case 24: *reinterpret_cast< double*>(_v) = _t->tempBMSAverage; break;
        case 25: *reinterpret_cast< double*>(_v) = _t->tempBMSLow; break;
        case 26: *reinterpret_cast< double*>(_v) = _t->humidity; break;
        case 27: *reinterpret_cast< QString*>(_v) = _t->opState; break;
        case 28: *reinterpret_cast< int*>(_v) = _t->balanceActive; break;
        case 29: *reinterpret_cast< QString*>(_v) = _t->faultState; break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = reinterpret_cast<BMS_VALUES *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0:
            if (_t->packVoltage != *reinterpret_cast< double*>(_v)) {
                _t->packVoltage = *reinterpret_cast< double*>(_v);
            }
            break;
        case 1:
            if (_t->packCurrent != *reinterpret_cast< double*>(_v)) {
                _t->packCurrent = *reinterpret_cast< double*>(_v);
            }
            break;
        case 2:
            if (_t->soC != *reinterpret_cast< int*>(_v)) {
                _t->soC = *reinterpret_cast< int*>(_v);
            }
            break;
        case 3:
            if (_t->AhCnt != *reinterpret_cast< double*>(_v)) {
                _t->AhCnt = *reinterpret_cast< double*>(_v);
            }
            break;
        case 4:
            if (_t->WhCnt != *reinterpret_cast< double*>(_v)) {
                _t->WhCnt = *reinterpret_cast< double*>(_v);
            }
            break;
        case 5:
            if (_t->AhCntChg != *reinterpret_cast< double*>(_v)) {
                _t->AhCntChg = *reinterpret_cast< double*>(_v);
            }
            break;
        case 6:
            if (_t->WhCntChg != *reinterpret_cast< double*>(_v)) {
                _t->WhCntChg = *reinterpret_cast< double*>(_v);
            }
            break;
        case 7:
            if (_t->AhCntDis != *reinterpret_cast< double*>(_v)) {
                _t->AhCntDis = *reinterpret_cast< double*>(_v);
            }
            break;
        case 8:
            if (_t->WhCntDis != *reinterpret_cast< double*>(_v)) {
                _t->WhCntDis = *reinterpret_cast< double*>(_v);
            }
            break;
        case 9:
            if (_t->cVHigh != *reinterpret_cast< double*>(_v)) {
                _t->cVHigh = *reinterpret_cast< double*>(_v);
            }
            break;
        case 10:
            if (_t->cVAverage != *reinterpret_cast< double*>(_v)) {
                _t->cVAverage = *reinterpret_cast< double*>(_v);
            }
            break;
        case 11:
            if (_t->cVLow != *reinterpret_cast< double*>(_v)) {
                _t->cVLow = *reinterpret_cast< double*>(_v);
            }
            break;
        case 12:
            if (_t->cVMisMatch != *reinterpret_cast< double*>(_v)) {
                _t->cVMisMatch = *reinterpret_cast< double*>(_v);
            }
            break;
        case 13:
            if (_t->loadLCVoltage != *reinterpret_cast< double*>(_v)) {
                _t->loadLCVoltage = *reinterpret_cast< double*>(_v);
            }
            break;
        case 14:
            if (_t->loadLCCurrent != *reinterpret_cast< double*>(_v)) {
                _t->loadLCCurrent = *reinterpret_cast< double*>(_v);
            }
            break;
        case 15:
            if (_t->loadHCVoltage != *reinterpret_cast< double*>(_v)) {
                _t->loadHCVoltage = *reinterpret_cast< double*>(_v);
            }
            break;
        case 16:
            if (_t->loadHCCurrent != *reinterpret_cast< double*>(_v)) {
                _t->loadHCCurrent = *reinterpret_cast< double*>(_v);
            }
            break;
        case 17:
            if (_t->chargerVoltage != *reinterpret_cast< double*>(_v)) {
                _t->chargerVoltage = *reinterpret_cast< double*>(_v);
            }
            break;
        case 18:
            if (_t->auxVoltage != *reinterpret_cast< double*>(_v)) {
                _t->auxVoltage = *reinterpret_cast< double*>(_v);
            }
            break;
        case 19:
            if (_t->auxCurrent != *reinterpret_cast< double*>(_v)) {
                _t->auxCurrent = *reinterpret_cast< double*>(_v);
            }
            break;
        case 20:
            if (_t->tempBattHigh != *reinterpret_cast< double*>(_v)) {
                _t->tempBattHigh = *reinterpret_cast< double*>(_v);
            }
            break;
        case 21:
            if (_t->tempBattAverage != *reinterpret_cast< double*>(_v)) {
                _t->tempBattAverage = *reinterpret_cast< double*>(_v);
            }
            break;
        case 22:
            if (_t->tempBattLow != *reinterpret_cast< double*>(_v)) {
                _t->tempBattLow = *reinterpret_cast< double*>(_v);
            }
            break;
        case 23:
            if (_t->tempBMSHigh != *reinterpret_cast< double*>(_v)) {
                _t->tempBMSHigh = *reinterpret_cast< double*>(_v);
            }
            break;
        case 24:
            if (_t->tempBMSAverage != *reinterpret_cast< double*>(_v)) {
                _t->tempBMSAverage = *reinterpret_cast< double*>(_v);
            }
            break;
        case 25:
            if (_t->tempBMSLow != *reinterpret_cast< double*>(_v)) {
                _t->tempBMSLow = *reinterpret_cast< double*>(_v);
            }
            break;
        case 26:
            if (_t->humidity != *reinterpret_cast< double*>(_v)) {
                _t->humidity = *reinterpret_cast< double*>(_v);
            }
            break;
        case 27:
            if (_t->opState != *reinterpret_cast< QString*>(_v)) {
                _t->opState = *reinterpret_cast< QString*>(_v);
            }
            break;
        case 28:
            if (_t->balanceActive != *reinterpret_cast< int*>(_v)) {
                _t->balanceActive = *reinterpret_cast< int*>(_v);
            }
            break;
        case 29:
            if (_t->faultState != *reinterpret_cast< QString*>(_v)) {
                _t->faultState = *reinterpret_cast< QString*>(_v);
            }
            break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject BMS_VALUES::staticMetaObject = { {
    nullptr,
    qt_meta_stringdata_BMS_VALUES.data,
    qt_meta_data_BMS_VALUES,
    qt_static_metacall,
    nullptr,
    nullptr
} };

struct qt_meta_stringdata_MCCONF_TEMP_t {
    QByteArrayData data[10];
    char stringdata0[125];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MCCONF_TEMP_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MCCONF_TEMP_t qt_meta_stringdata_MCCONF_TEMP = {
    {
QT_MOC_LITERAL(0, 0, 11), // "MCCONF_TEMP"
QT_MOC_LITERAL(1, 12, 17), // "current_min_scale"
QT_MOC_LITERAL(2, 30, 17), // "current_max_scale"
QT_MOC_LITERAL(3, 48, 17), // "erpm_or_speed_min"
QT_MOC_LITERAL(4, 66, 17), // "erpm_or_speed_max"
QT_MOC_LITERAL(5, 84, 8), // "duty_min"
QT_MOC_LITERAL(6, 93, 8), // "duty_max"
QT_MOC_LITERAL(7, 102, 8), // "watt_min"
QT_MOC_LITERAL(8, 111, 8), // "watt_max"
QT_MOC_LITERAL(9, 120, 4) // "name"

    },
    "MCCONF_TEMP\0current_min_scale\0"
    "current_max_scale\0erpm_or_speed_min\0"
    "erpm_or_speed_max\0duty_min\0duty_max\0"
    "watt_min\0watt_max\0name"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MCCONF_TEMP[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       9,   14, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       4,       // flags
       0,       // signalCount

 // properties: name, type, flags
       1, QMetaType::Double, 0x00095003,
       2, QMetaType::Double, 0x00095003,
       3, QMetaType::Double, 0x00095003,
       4, QMetaType::Double, 0x00095003,
       5, QMetaType::Double, 0x00095003,
       6, QMetaType::Double, 0x00095003,
       7, QMetaType::Double, 0x00095003,
       8, QMetaType::Double, 0x00095003,
       9, QMetaType::QString, 0x00095003,

       0        // eod
};

void MCCONF_TEMP::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{

#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty) {
        auto *_t = reinterpret_cast<MCCONF_TEMP *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< double*>(_v) = _t->current_min_scale; break;
        case 1: *reinterpret_cast< double*>(_v) = _t->current_max_scale; break;
        case 2: *reinterpret_cast< double*>(_v) = _t->erpm_or_speed_min; break;
        case 3: *reinterpret_cast< double*>(_v) = _t->erpm_or_speed_max; break;
        case 4: *reinterpret_cast< double*>(_v) = _t->duty_min; break;
        case 5: *reinterpret_cast< double*>(_v) = _t->duty_max; break;
        case 6: *reinterpret_cast< double*>(_v) = _t->watt_min; break;
        case 7: *reinterpret_cast< double*>(_v) = _t->watt_max; break;
        case 8: *reinterpret_cast< QString*>(_v) = _t->name; break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = reinterpret_cast<MCCONF_TEMP *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0:
            if (_t->current_min_scale != *reinterpret_cast< double*>(_v)) {
                _t->current_min_scale = *reinterpret_cast< double*>(_v);
            }
            break;
        case 1:
            if (_t->current_max_scale != *reinterpret_cast< double*>(_v)) {
                _t->current_max_scale = *reinterpret_cast< double*>(_v);
            }
            break;
        case 2:
            if (_t->erpm_or_speed_min != *reinterpret_cast< double*>(_v)) {
                _t->erpm_or_speed_min = *reinterpret_cast< double*>(_v);
            }
            break;
        case 3:
            if (_t->erpm_or_speed_max != *reinterpret_cast< double*>(_v)) {
                _t->erpm_or_speed_max = *reinterpret_cast< double*>(_v);
            }
            break;
        case 4:
            if (_t->duty_min != *reinterpret_cast< double*>(_v)) {
                _t->duty_min = *reinterpret_cast< double*>(_v);
            }
            break;
        case 5:
            if (_t->duty_max != *reinterpret_cast< double*>(_v)) {
                _t->duty_max = *reinterpret_cast< double*>(_v);
            }
            break;
        case 6:
            if (_t->watt_min != *reinterpret_cast< double*>(_v)) {
                _t->watt_min = *reinterpret_cast< double*>(_v);
            }
            break;
        case 7:
            if (_t->watt_max != *reinterpret_cast< double*>(_v)) {
                _t->watt_max = *reinterpret_cast< double*>(_v);
            }
            break;
        case 8:
            if (_t->name != *reinterpret_cast< QString*>(_v)) {
                _t->name = *reinterpret_cast< QString*>(_v);
            }
            break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject MCCONF_TEMP::staticMetaObject = { {
    nullptr,
    qt_meta_stringdata_MCCONF_TEMP.data,
    qt_meta_data_MCCONF_TEMP,
    qt_static_metacall,
    nullptr,
    nullptr
} };

struct qt_meta_stringdata_bldc_detect_t {
    QByteArrayData data[6];
    char stringdata0[77];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_bldc_detect_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_bldc_detect_t qt_meta_stringdata_bldc_detect = {
    {
QT_MOC_LITERAL(0, 0, 11), // "bldc_detect"
QT_MOC_LITERAL(1, 12, 15), // "cycle_int_limit"
QT_MOC_LITERAL(2, 28, 15), // "bemf_coupling_k"
QT_MOC_LITERAL(3, 44, 10), // "hall_table"
QT_MOC_LITERAL(4, 55, 12), // "QVector<int>"
QT_MOC_LITERAL(5, 68, 8) // "hall_res"

    },
    "bldc_detect\0cycle_int_limit\0bemf_coupling_k\0"
    "hall_table\0QVector<int>\0hall_res"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_bldc_detect[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       4,   14, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       4,       // flags
       0,       // signalCount

 // properties: name, type, flags
       1, QMetaType::Double, 0x00095003,
       2, QMetaType::Double, 0x00095003,
       3, 0x80000000 | 4, 0x0009500b,
       5, QMetaType::Int, 0x00095003,

       0        // eod
};

void bldc_detect::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::RegisterPropertyMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 2:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<int> >(); break;
        }
    }

#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = reinterpret_cast<bldc_detect *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< double*>(_v) = _t->cycle_int_limit; break;
        case 1: *reinterpret_cast< double*>(_v) = _t->bemf_coupling_k; break;
        case 2: *reinterpret_cast< QVector<int>*>(_v) = _t->hall_table; break;
        case 3: *reinterpret_cast< int*>(_v) = _t->hall_res; break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = reinterpret_cast<bldc_detect *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0:
            if (_t->cycle_int_limit != *reinterpret_cast< double*>(_v)) {
                _t->cycle_int_limit = *reinterpret_cast< double*>(_v);
            }
            break;
        case 1:
            if (_t->bemf_coupling_k != *reinterpret_cast< double*>(_v)) {
                _t->bemf_coupling_k = *reinterpret_cast< double*>(_v);
            }
            break;
        case 2:
            if (_t->hall_table != *reinterpret_cast< QVector<int>*>(_v)) {
                _t->hall_table = *reinterpret_cast< QVector<int>*>(_v);
            }
            break;
        case 3:
            if (_t->hall_res != *reinterpret_cast< int*>(_v)) {
                _t->hall_res = *reinterpret_cast< int*>(_v);
            }
            break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    (void)_o;
}

QT_INIT_METAOBJECT const QMetaObject bldc_detect::staticMetaObject = { {
    nullptr,
    qt_meta_stringdata_bldc_detect.data,
    qt_meta_data_bldc_detect,
    qt_static_metacall,
    nullptr,
    nullptr
} };

QT_WARNING_POP
QT_END_MOC_NAMESPACE
