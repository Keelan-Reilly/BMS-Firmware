/****************************************************************************
** Meta object code from reading C++ file 'commands.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.18)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "commands.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'commands.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.18. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Commands_t {
    QByteArrayData data[76];
    char stringdata0[887];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Commands_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Commands_t qt_meta_stringdata_Commands = {
    {
QT_MOC_LITERAL(0, 0, 8), // "Commands"
QT_MOC_LITERAL(1, 9, 10), // "dataToSend"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 11), // "QByteArray&"
QT_MOC_LITERAL(4, 33, 4), // "data"
QT_MOC_LITERAL(5, 38, 17), // "fwVersionReceived"
QT_MOC_LITERAL(6, 56, 5), // "major"
QT_MOC_LITERAL(7, 62, 5), // "minor"
QT_MOC_LITERAL(8, 68, 2), // "hw"
QT_MOC_LITERAL(9, 71, 4), // "uuid"
QT_MOC_LITERAL(10, 76, 11), // "ackReceived"
QT_MOC_LITERAL(11, 88, 7), // "ackType"
QT_MOC_LITERAL(12, 96, 14), // "valuesReceived"
QT_MOC_LITERAL(13, 111, 10), // "BMS_VALUES"
QT_MOC_LITERAL(14, 122, 6), // "values"
QT_MOC_LITERAL(15, 129, 13), // "cellsReceived"
QT_MOC_LITERAL(16, 143, 9), // "cellCount"
QT_MOC_LITERAL(17, 153, 15), // "QVector<double>"
QT_MOC_LITERAL(18, 169, 16), // "cellVoltageArray"
QT_MOC_LITERAL(19, 186, 11), // "auxReceived"
QT_MOC_LITERAL(20, 198, 8), // "auxCount"
QT_MOC_LITERAL(21, 207, 15), // "auxVoltageArray"
QT_MOC_LITERAL(22, 223, 15), // "expTempReceived"
QT_MOC_LITERAL(23, 239, 12), // "expTempCount"
QT_MOC_LITERAL(24, 252, 19), // "expTempVoltageArray"
QT_MOC_LITERAL(25, 272, 13), // "printReceived"
QT_MOC_LITERAL(26, 286, 3), // "str"
QT_MOC_LITERAL(27, 290, 16), // "rotorPosReceived"
QT_MOC_LITERAL(28, 307, 3), // "pos"
QT_MOC_LITERAL(29, 311, 20), // "bmsConfigCheckResult"
QT_MOC_LITERAL(30, 332, 12), // "paramsNotSet"
QT_MOC_LITERAL(31, 345, 19), // "valuesSetupReceived"
QT_MOC_LITERAL(32, 365, 9), // "pingCanRx"
QT_MOC_LITERAL(33, 375, 12), // "QVector<int>"
QT_MOC_LITERAL(34, 388, 4), // "devs"
QT_MOC_LITERAL(35, 393, 9), // "isTimeout"
QT_MOC_LITERAL(36, 403, 13), // "processPacket"
QT_MOC_LITERAL(37, 417, 12), // "getFwVersion"
QT_MOC_LITERAL(38, 430, 9), // "getValues"
QT_MOC_LITERAL(39, 440, 8), // "getCells"
QT_MOC_LITERAL(40, 449, 6), // "getAux"
QT_MOC_LITERAL(41, 456, 16), // "getExpansionTemp"
QT_MOC_LITERAL(42, 473, 15), // "sendTerminalCmd"
QT_MOC_LITERAL(43, 489, 3), // "cmd"
QT_MOC_LITERAL(44, 493, 9), // "setDetect"
QT_MOC_LITERAL(45, 503, 13), // "disp_pos_mode"
QT_MOC_LITERAL(46, 517, 4), // "mode"
QT_MOC_LITERAL(47, 522, 11), // "samplePrint"
QT_MOC_LITERAL(48, 534, 19), // "debug_sampling_mode"
QT_MOC_LITERAL(49, 554, 10), // "sample_len"
QT_MOC_LITERAL(50, 565, 3), // "dec"
QT_MOC_LITERAL(51, 569, 10), // "getBMSconf"
QT_MOC_LITERAL(52, 580, 17), // "getBMSconfDefault"
QT_MOC_LITERAL(53, 598, 10), // "setBMSconf"
QT_MOC_LITERAL(54, 609, 5), // "check"
QT_MOC_LITERAL(55, 615, 6), // "reboot"
QT_MOC_LITERAL(56, 622, 9), // "sendAlive"
QT_MOC_LITERAL(57, 632, 7), // "pairNrf"
QT_MOC_LITERAL(58, 640, 2), // "ms"
QT_MOC_LITERAL(59, 643, 7), // "pingCan"
QT_MOC_LITERAL(60, 651, 9), // "timerSlot"
QT_MOC_LITERAL(61, 661, 13), // "isLimitedMode"
QT_MOC_LITERAL(62, 675, 10), // "setSendCan"
QT_MOC_LITERAL(63, 686, 7), // "sendCan"
QT_MOC_LITERAL(64, 694, 2), // "id"
QT_MOC_LITERAL(65, 697, 10), // "getSendCan"
QT_MOC_LITERAL(66, 708, 12), // "setCanSendId"
QT_MOC_LITERAL(67, 721, 12), // "getCanSendId"
QT_MOC_LITERAL(68, 734, 19), // "startFirmwareUpload"
QT_MOC_LITERAL(69, 754, 11), // "newFirmware"
QT_MOC_LITERAL(70, 766, 12), // "isBootloader"
QT_MOC_LITERAL(71, 779, 25), // "getFirmwareUploadProgress"
QT_MOC_LITERAL(72, 805, 23), // "getFirmwareUploadStatus"
QT_MOC_LITERAL(73, 829, 20), // "cancelFirmwareUpload"
QT_MOC_LITERAL(74, 850, 15), // "emitEmptyValues"
QT_MOC_LITERAL(75, 866, 20) // "emitEmptySetupValues"

    },
    "Commands\0dataToSend\0\0QByteArray&\0data\0"
    "fwVersionReceived\0major\0minor\0hw\0uuid\0"
    "ackReceived\0ackType\0valuesReceived\0"
    "BMS_VALUES\0values\0cellsReceived\0"
    "cellCount\0QVector<double>\0cellVoltageArray\0"
    "auxReceived\0auxCount\0auxVoltageArray\0"
    "expTempReceived\0expTempCount\0"
    "expTempVoltageArray\0printReceived\0str\0"
    "rotorPosReceived\0pos\0bmsConfigCheckResult\0"
    "paramsNotSet\0valuesSetupReceived\0"
    "pingCanRx\0QVector<int>\0devs\0isTimeout\0"
    "processPacket\0getFwVersion\0getValues\0"
    "getCells\0getAux\0getExpansionTemp\0"
    "sendTerminalCmd\0cmd\0setDetect\0"
    "disp_pos_mode\0mode\0samplePrint\0"
    "debug_sampling_mode\0sample_len\0dec\0"
    "getBMSconf\0getBMSconfDefault\0setBMSconf\0"
    "check\0reboot\0sendAlive\0pairNrf\0ms\0"
    "pingCan\0timerSlot\0isLimitedMode\0"
    "setSendCan\0sendCan\0id\0getSendCan\0"
    "setCanSendId\0getCanSendId\0startFirmwareUpload\0"
    "newFirmware\0isBootloader\0"
    "getFirmwareUploadProgress\0"
    "getFirmwareUploadStatus\0cancelFirmwareUpload\0"
    "emitEmptyValues\0emitEmptySetupValues"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Commands[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      43,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      12,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  229,    2, 0x06 /* Public */,
       5,    4,  232,    2, 0x06 /* Public */,
      10,    1,  241,    2, 0x06 /* Public */,
      12,    1,  244,    2, 0x06 /* Public */,
      15,    2,  247,    2, 0x06 /* Public */,
      19,    2,  252,    2, 0x06 /* Public */,
      22,    2,  257,    2, 0x06 /* Public */,
      25,    1,  262,    2, 0x06 /* Public */,
      27,    1,  265,    2, 0x06 /* Public */,
      29,    1,  268,    2, 0x06 /* Public */,
      31,    1,  271,    2, 0x06 /* Public */,
      32,    2,  274,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      36,    1,  279,    2, 0x0a /* Public */,
      37,    0,  282,    2, 0x0a /* Public */,
      38,    0,  283,    2, 0x0a /* Public */,
      39,    0,  284,    2, 0x0a /* Public */,
      40,    0,  285,    2, 0x0a /* Public */,
      41,    0,  286,    2, 0x0a /* Public */,
      42,    1,  287,    2, 0x0a /* Public */,
      44,    1,  290,    2, 0x0a /* Public */,
      47,    3,  293,    2, 0x0a /* Public */,
      51,    0,  300,    2, 0x0a /* Public */,
      52,    0,  301,    2, 0x0a /* Public */,
      53,    1,  302,    2, 0x0a /* Public */,
      53,    0,  305,    2, 0x2a /* Public | MethodCloned */,
      55,    0,  306,    2, 0x0a /* Public */,
      56,    0,  307,    2, 0x0a /* Public */,
      57,    1,  308,    2, 0x0a /* Public */,
      59,    0,  311,    2, 0x0a /* Public */,
      60,    0,  312,    2, 0x08 /* Private */,

 // methods: name, argc, parameters, tag, flags
      61,    0,  313,    2, 0x02 /* Public */,
      62,    2,  314,    2, 0x02 /* Public */,
      62,    1,  319,    2, 0x22 /* Public | MethodCloned */,
      65,    0,  322,    2, 0x02 /* Public */,
      66,    1,  323,    2, 0x02 /* Public */,
      67,    0,  326,    2, 0x02 /* Public */,
      68,    2,  327,    2, 0x02 /* Public */,
      68,    1,  332,    2, 0x22 /* Public | MethodCloned */,
      71,    0,  335,    2, 0x02 /* Public */,
      72,    0,  336,    2, 0x02 /* Public */,
      73,    0,  337,    2, 0x02 /* Public */,
      74,    0,  338,    2, 0x02 /* Public */,
      75,    0,  339,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::QString, QMetaType::QByteArray,    6,    7,    8,    9,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void, 0x80000000 | 13,   14,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 17,   16,   18,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 17,   20,   21,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 17,   23,   24,
    QMetaType::Void, QMetaType::QString,   26,
    QMetaType::Void, QMetaType::Double,   28,
    QMetaType::Void, QMetaType::QStringList,   30,
    QMetaType::Void, 0x80000000 | 13,   14,
    QMetaType::Void, 0x80000000 | 33, QMetaType::Bool,   34,   35,

 // slots: parameters
    QMetaType::Void, QMetaType::QByteArray,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   43,
    QMetaType::Void, 0x80000000 | 45,   46,
    QMetaType::Void, 0x80000000 | 48, QMetaType::Int, QMetaType::Int,   46,   49,   50,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   54,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   58,
    QMetaType::Void,
    QMetaType::Void,

 // methods: parameters
    QMetaType::Bool,
    QMetaType::Void, QMetaType::Bool, QMetaType::Int,   63,   64,
    QMetaType::Void, QMetaType::Bool,   63,
    QMetaType::Bool,
    QMetaType::Void, QMetaType::UInt,   64,
    QMetaType::Int,
    QMetaType::Void, 0x80000000 | 3, QMetaType::Bool,   69,   70,
    QMetaType::Void, 0x80000000 | 3,   69,
    QMetaType::Double,
    QMetaType::QString,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Commands::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Commands *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->dataToSend((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 1: _t->fwVersionReceived((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< QByteArray(*)>(_a[4]))); break;
        case 2: _t->ackReceived((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->valuesReceived((*reinterpret_cast< BMS_VALUES(*)>(_a[1]))); break;
        case 4: _t->cellsReceived((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QVector<double>(*)>(_a[2]))); break;
        case 5: _t->auxReceived((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QVector<double>(*)>(_a[2]))); break;
        case 6: _t->expTempReceived((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QVector<double>(*)>(_a[2]))); break;
        case 7: _t->printReceived((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 8: _t->rotorPosReceived((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 9: _t->bmsConfigCheckResult((*reinterpret_cast< QStringList(*)>(_a[1]))); break;
        case 10: _t->valuesSetupReceived((*reinterpret_cast< BMS_VALUES(*)>(_a[1]))); break;
        case 11: _t->pingCanRx((*reinterpret_cast< QVector<int>(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 12: _t->processPacket((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 13: _t->getFwVersion(); break;
        case 14: _t->getValues(); break;
        case 15: _t->getCells(); break;
        case 16: _t->getAux(); break;
        case 17: _t->getExpansionTemp(); break;
        case 18: _t->sendTerminalCmd((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 19: _t->setDetect((*reinterpret_cast< disp_pos_mode(*)>(_a[1]))); break;
        case 20: _t->samplePrint((*reinterpret_cast< debug_sampling_mode(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 21: _t->getBMSconf(); break;
        case 22: _t->getBMSconfDefault(); break;
        case 23: _t->setBMSconf((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 24: _t->setBMSconf(); break;
        case 25: _t->reboot(); break;
        case 26: _t->sendAlive(); break;
        case 27: _t->pairNrf((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 28: _t->pingCan(); break;
        case 29: _t->timerSlot(); break;
        case 30: { bool _r = _t->isLimitedMode();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 31: _t->setSendCan((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 32: _t->setSendCan((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 33: { bool _r = _t->getSendCan();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 34: _t->setCanSendId((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 35: { int _r = _t->getCanSendId();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 36: _t->startFirmwareUpload((*reinterpret_cast< QByteArray(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 37: _t->startFirmwareUpload((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 38: { double _r = _t->getFirmwareUploadProgress();
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = std::move(_r); }  break;
        case 39: { QString _r = _t->getFirmwareUploadStatus();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 40: _t->cancelFirmwareUpload(); break;
        case 41: _t->emitEmptyValues(); break;
        case 42: _t->emitEmptySetupValues(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< BMS_VALUES >(); break;
            }
            break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<double> >(); break;
            }
            break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<double> >(); break;
            }
            break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<double> >(); break;
            }
            break;
        case 10:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< BMS_VALUES >(); break;
            }
            break;
        case 11:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<int> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Commands::*)(QByteArray & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Commands::dataToSend)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Commands::*)(int , int , QString , QByteArray );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Commands::fwVersionReceived)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Commands::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Commands::ackReceived)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (Commands::*)(BMS_VALUES );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Commands::valuesReceived)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (Commands::*)(int , QVector<double> );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Commands::cellsReceived)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (Commands::*)(int , QVector<double> );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Commands::auxReceived)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (Commands::*)(int , QVector<double> );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Commands::expTempReceived)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (Commands::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Commands::printReceived)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (Commands::*)(double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Commands::rotorPosReceived)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (Commands::*)(QStringList );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Commands::bmsConfigCheckResult)) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (Commands::*)(BMS_VALUES );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Commands::valuesSetupReceived)) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (Commands::*)(QVector<int> , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Commands::pingCanRx)) {
                *result = 11;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Commands::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_Commands.data,
    qt_meta_data_Commands,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Commands::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Commands::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Commands.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Commands::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 43)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 43;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 43)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 43;
    }
    return _id;
}

// SIGNAL 0
void Commands::dataToSend(QByteArray & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Commands::fwVersionReceived(int _t1, int _t2, QString _t3, QByteArray _t4)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Commands::ackReceived(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Commands::valuesReceived(BMS_VALUES _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Commands::cellsReceived(int _t1, QVector<double> _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Commands::auxReceived(int _t1, QVector<double> _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void Commands::expTempReceived(int _t1, QVector<double> _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void Commands::printReceived(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void Commands::rotorPosReceived(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void Commands::bmsConfigCheckResult(QStringList _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void Commands::valuesSetupReceived(BMS_VALUES _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void Commands::pingCanRx(QVector<int> _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
