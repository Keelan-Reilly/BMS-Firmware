/****************************************************************************
** Meta object code from reading C++ file 'bmsinterface.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.18)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "bmsinterface.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'bmsinterface.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.18. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_BMSInterface_t {
    QByteArrayData data[80];
    char stringdata0[1034];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_BMSInterface_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_BMSInterface_t qt_meta_stringdata_BMSInterface = {
    {
QT_MOC_LITERAL(0, 0, 12), // "BMSInterface"
QT_MOC_LITERAL(1, 13, 13), // "statusMessage"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 3), // "msg"
QT_MOC_LITERAL(4, 32, 6), // "isGood"
QT_MOC_LITERAL(5, 39, 13), // "messageDialog"
QT_MOC_LITERAL(6, 53, 5), // "title"
QT_MOC_LITERAL(7, 59, 8), // "richText"
QT_MOC_LITERAL(8, 68, 14), // "fwUploadStatus"
QT_MOC_LITERAL(9, 83, 6), // "status"
QT_MOC_LITERAL(10, 90, 8), // "progress"
QT_MOC_LITERAL(11, 99, 9), // "isOngoing"
QT_MOC_LITERAL(12, 109, 21), // "serialPortNotWritable"
QT_MOC_LITERAL(13, 131, 4), // "port"
QT_MOC_LITERAL(14, 136, 11), // "fwRxChanged"
QT_MOC_LITERAL(15, 148, 2), // "rx"
QT_MOC_LITERAL(16, 151, 7), // "limited"
QT_MOC_LITERAL(17, 159, 20), // "portConnectedChanged"
QT_MOC_LITERAL(18, 180, 26), // "autoConnectProgressUpdated"
QT_MOC_LITERAL(19, 207, 19), // "autoConnectFinished"
QT_MOC_LITERAL(20, 227, 18), // "pairingListUpdated"
QT_MOC_LITERAL(21, 246, 19), // "serialDataAvailable"
QT_MOC_LITERAL(22, 266, 15), // "serialPortError"
QT_MOC_LITERAL(23, 282, 28), // "QSerialPort::SerialPortError"
QT_MOC_LITERAL(24, 311, 5), // "error"
QT_MOC_LITERAL(25, 317, 17), // "tcpInputConnected"
QT_MOC_LITERAL(26, 335, 20), // "tcpInputDisconnected"
QT_MOC_LITERAL(27, 356, 21), // "tcpInputDataAvailable"
QT_MOC_LITERAL(28, 378, 13), // "tcpInputError"
QT_MOC_LITERAL(29, 392, 28), // "QAbstractSocket::SocketError"
QT_MOC_LITERAL(30, 421, 11), // "socketError"
QT_MOC_LITERAL(31, 433, 9), // "bleDataRx"
QT_MOC_LITERAL(32, 443, 4), // "data"
QT_MOC_LITERAL(33, 448, 9), // "timerSlot"
QT_MOC_LITERAL(34, 458, 16), // "packetDataToSend"
QT_MOC_LITERAL(35, 475, 11), // "QByteArray&"
QT_MOC_LITERAL(36, 487, 14), // "packetReceived"
QT_MOC_LITERAL(37, 502, 13), // "cmdDataToSend"
QT_MOC_LITERAL(38, 516, 17), // "fwVersionReceived"
QT_MOC_LITERAL(39, 534, 5), // "major"
QT_MOC_LITERAL(40, 540, 5), // "minor"
QT_MOC_LITERAL(41, 546, 2), // "hw"
QT_MOC_LITERAL(42, 549, 4), // "uuid"
QT_MOC_LITERAL(43, 554, 14), // "bmsconfUpdated"
QT_MOC_LITERAL(44, 569, 13), // "bmsconfStored"
QT_MOC_LITERAL(45, 583, 11), // "ackReceived"
QT_MOC_LITERAL(46, 595, 7), // "ackType"
QT_MOC_LITERAL(47, 603, 8), // "commands"
QT_MOC_LITERAL(48, 612, 9), // "Commands*"
QT_MOC_LITERAL(49, 622, 9), // "bmsConfig"
QT_MOC_LITERAL(50, 632, 13), // "ConfigParams*"
QT_MOC_LITERAL(51, 646, 10), // "infoConfig"
QT_MOC_LITERAL(52, 657, 21), // "getSupportedFirmwares"
QT_MOC_LITERAL(53, 679, 14), // "getFirmwareNow"
QT_MOC_LITERAL(54, 694, 17), // "emitStatusMessage"
QT_MOC_LITERAL(55, 712, 17), // "emitMessageDialog"
QT_MOC_LITERAL(56, 730, 4), // "fwRx"
QT_MOC_LITERAL(57, 735, 13), // "storeSettings"
QT_MOC_LITERAL(58, 749, 9), // "bleDevice"
QT_MOC_LITERAL(59, 759, 8), // "BleUart*"
QT_MOC_LITERAL(60, 768, 12), // "storeBleName"
QT_MOC_LITERAL(61, 781, 7), // "address"
QT_MOC_LITERAL(62, 789, 4), // "name"
QT_MOC_LITERAL(63, 794, 10), // "getBleName"
QT_MOC_LITERAL(64, 805, 14), // "getLastBleAddr"
QT_MOC_LITERAL(65, 820, 15), // "isPortConnected"
QT_MOC_LITERAL(66, 836, 14), // "disconnectPort"
QT_MOC_LITERAL(67, 851, 17), // "reconnectLastPort"
QT_MOC_LITERAL(68, 869, 11), // "autoconnect"
QT_MOC_LITERAL(69, 881, 20), // "getConnectedPortName"
QT_MOC_LITERAL(70, 902, 10), // "connectTcp"
QT_MOC_LITERAL(71, 913, 6), // "server"
QT_MOC_LITERAL(72, 920, 10), // "connectBle"
QT_MOC_LITERAL(73, 931, 20), // "isAutoconnectOngoing"
QT_MOC_LITERAL(74, 952, 22), // "getAutoconnectProgress"
QT_MOC_LITERAL(75, 975, 7), // "scanCan"
QT_MOC_LITERAL(76, 983, 12), // "QVector<int>"
QT_MOC_LITERAL(77, 996, 14), // "getCanDevsLast"
QT_MOC_LITERAL(78, 1011, 15), // "ignoreCanChange"
QT_MOC_LITERAL(79, 1027, 6) // "ignore"

    },
    "BMSInterface\0statusMessage\0\0msg\0isGood\0"
    "messageDialog\0title\0richText\0"
    "fwUploadStatus\0status\0progress\0isOngoing\0"
    "serialPortNotWritable\0port\0fwRxChanged\0"
    "rx\0limited\0portConnectedChanged\0"
    "autoConnectProgressUpdated\0"
    "autoConnectFinished\0pairingListUpdated\0"
    "serialDataAvailable\0serialPortError\0"
    "QSerialPort::SerialPortError\0error\0"
    "tcpInputConnected\0tcpInputDisconnected\0"
    "tcpInputDataAvailable\0tcpInputError\0"
    "QAbstractSocket::SocketError\0socketError\0"
    "bleDataRx\0data\0timerSlot\0packetDataToSend\0"
    "QByteArray&\0packetReceived\0cmdDataToSend\0"
    "fwVersionReceived\0major\0minor\0hw\0uuid\0"
    "bmsconfUpdated\0bmsconfStored\0ackReceived\0"
    "ackType\0commands\0Commands*\0bmsConfig\0"
    "ConfigParams*\0infoConfig\0getSupportedFirmwares\0"
    "getFirmwareNow\0emitStatusMessage\0"
    "emitMessageDialog\0fwRx\0storeSettings\0"
    "bleDevice\0BleUart*\0storeBleName\0address\0"
    "name\0getBleName\0getLastBleAddr\0"
    "isPortConnected\0disconnectPort\0"
    "reconnectLastPort\0autoconnect\0"
    "getConnectedPortName\0connectTcp\0server\0"
    "connectBle\0isAutoconnectOngoing\0"
    "getAutoconnectProgress\0scanCan\0"
    "QVector<int>\0getCanDevsLast\0ignoreCanChange\0"
    "ignore"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_BMSInterface[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      50,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       9,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,  264,    2, 0x06 /* Public */,
       5,    4,  269,    2, 0x06 /* Public */,
       8,    3,  278,    2, 0x06 /* Public */,
      12,    1,  285,    2, 0x06 /* Public */,
      14,    2,  288,    2, 0x06 /* Public */,
      17,    0,  293,    2, 0x06 /* Public */,
      18,    2,  294,    2, 0x06 /* Public */,
      19,    0,  299,    2, 0x06 /* Public */,
      20,    0,  300,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      21,    0,  301,    2, 0x08 /* Private */,
      22,    1,  302,    2, 0x08 /* Private */,
      25,    0,  305,    2, 0x08 /* Private */,
      26,    0,  306,    2, 0x08 /* Private */,
      27,    0,  307,    2, 0x08 /* Private */,
      28,    1,  308,    2, 0x08 /* Private */,
      31,    1,  311,    2, 0x08 /* Private */,
      33,    0,  314,    2, 0x08 /* Private */,
      34,    1,  315,    2, 0x08 /* Private */,
      36,    1,  318,    2, 0x08 /* Private */,
      37,    1,  321,    2, 0x08 /* Private */,
      38,    4,  324,    2, 0x08 /* Private */,
      43,    0,  333,    2, 0x08 /* Private */,
      44,    0,  334,    2, 0x08 /* Private */,
      45,    1,  335,    2, 0x08 /* Private */,

 // methods: name, argc, parameters, tag, flags
      47,    0,  338,    2, 0x02 /* Public */,
      49,    0,  339,    2, 0x02 /* Public */,
      51,    0,  340,    2, 0x02 /* Public */,
      52,    0,  341,    2, 0x02 /* Public */,
      53,    0,  342,    2, 0x02 /* Public */,
      54,    2,  343,    2, 0x02 /* Public */,
      55,    4,  348,    2, 0x02 /* Public */,
      55,    3,  357,    2, 0x22 /* Public | MethodCloned */,
      56,    0,  364,    2, 0x02 /* Public */,
      57,    0,  365,    2, 0x02 /* Public */,
      58,    0,  366,    2, 0x02 /* Public */,
      60,    2,  367,    2, 0x02 /* Public */,
      63,    1,  372,    2, 0x02 /* Public */,
      64,    0,  375,    2, 0x02 /* Public */,
      65,    0,  376,    2, 0x02 /* Public */,
      66,    0,  377,    2, 0x02 /* Public */,
      67,    0,  378,    2, 0x02 /* Public */,
      68,    0,  379,    2, 0x02 /* Public */,
      69,    0,  380,    2, 0x02 /* Public */,
      70,    2,  381,    2, 0x02 /* Public */,
      72,    1,  386,    2, 0x02 /* Public */,
      73,    0,  389,    2, 0x02 /* Public */,
      74,    0,  390,    2, 0x02 /* Public */,
      75,    0,  391,    2, 0x02 /* Public */,
      77,    0,  392,    2, 0x02 /* Public */,
      78,    1,  393,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    3,    4,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::Bool, QMetaType::Bool,    6,    3,    4,    7,
    QMetaType::Void, QMetaType::QString, QMetaType::Double, QMetaType::Bool,    9,   10,   11,
    QMetaType::Void, QMetaType::QString,   13,
    QMetaType::Void, QMetaType::Bool, QMetaType::Bool,   15,   16,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double, QMetaType::Bool,   10,   11,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 23,   24,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 29,   30,
    QMetaType::Void, QMetaType::QByteArray,   32,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 35,   32,
    QMetaType::Void, 0x80000000 | 35,   32,
    QMetaType::Void, 0x80000000 | 35,   32,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::QString, QMetaType::QByteArray,   39,   40,   41,   42,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   46,

 // methods: parameters
    0x80000000 | 48,
    0x80000000 | 50,
    0x80000000 | 50,
    QMetaType::QStringList,
    QMetaType::QString,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    3,    4,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::Bool, QMetaType::Bool,    6,    3,    4,    7,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::Bool,    6,    3,    4,
    QMetaType::Bool,
    QMetaType::Void,
    0x80000000 | 59,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   61,   62,
    QMetaType::QString, QMetaType::QString,   61,
    QMetaType::QString,
    QMetaType::Bool,
    QMetaType::Void,
    QMetaType::Bool,
    QMetaType::Bool,
    QMetaType::QString,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,   71,   13,
    QMetaType::Void, QMetaType::QString,   61,
    QMetaType::Bool,
    QMetaType::Double,
    0x80000000 | 76,
    0x80000000 | 76,
    QMetaType::Void, QMetaType::Bool,   79,

       0        // eod
};

void BMSInterface::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<BMSInterface *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->statusMessage((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 1: _t->messageDialog((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4]))); break;
        case 2: _t->fwUploadStatus((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 3: _t->serialPortNotWritable((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->fwRxChanged((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 5: _t->portConnectedChanged(); break;
        case 6: _t->autoConnectProgressUpdated((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 7: _t->autoConnectFinished(); break;
        case 8: _t->pairingListUpdated(); break;
        case 9: _t->serialDataAvailable(); break;
        case 10: _t->serialPortError((*reinterpret_cast< QSerialPort::SerialPortError(*)>(_a[1]))); break;
        case 11: _t->tcpInputConnected(); break;
        case 12: _t->tcpInputDisconnected(); break;
        case 13: _t->tcpInputDataAvailable(); break;
        case 14: _t->tcpInputError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 15: _t->bleDataRx((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 16: _t->timerSlot(); break;
        case 17: _t->packetDataToSend((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 18: _t->packetReceived((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 19: _t->cmdDataToSend((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 20: _t->fwVersionReceived((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< QByteArray(*)>(_a[4]))); break;
        case 21: _t->bmsconfUpdated(); break;
        case 22: _t->bmsconfStored(); break;
        case 23: _t->ackReceived((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 24: { Commands* _r = _t->commands();
            if (_a[0]) *reinterpret_cast< Commands**>(_a[0]) = std::move(_r); }  break;
        case 25: { ConfigParams* _r = _t->bmsConfig();
            if (_a[0]) *reinterpret_cast< ConfigParams**>(_a[0]) = std::move(_r); }  break;
        case 26: { ConfigParams* _r = _t->infoConfig();
            if (_a[0]) *reinterpret_cast< ConfigParams**>(_a[0]) = std::move(_r); }  break;
        case 27: { QStringList _r = _t->getSupportedFirmwares();
            if (_a[0]) *reinterpret_cast< QStringList*>(_a[0]) = std::move(_r); }  break;
        case 28: { QString _r = _t->getFirmwareNow();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 29: _t->emitStatusMessage((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 30: _t->emitMessageDialog((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4]))); break;
        case 31: _t->emitMessageDialog((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 32: { bool _r = _t->fwRx();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 33: _t->storeSettings(); break;
        case 34: { BleUart* _r = _t->bleDevice();
            if (_a[0]) *reinterpret_cast< BleUart**>(_a[0]) = std::move(_r); }  break;
        case 35: _t->storeBleName((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 36: { QString _r = _t->getBleName((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 37: { QString _r = _t->getLastBleAddr();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 38: { bool _r = _t->isPortConnected();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 39: _t->disconnectPort(); break;
        case 40: { bool _r = _t->reconnectLastPort();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 41: { bool _r = _t->autoconnect();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 42: { QString _r = _t->getConnectedPortName();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 43: _t->connectTcp((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 44: _t->connectBle((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 45: { bool _r = _t->isAutoconnectOngoing();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 46: { double _r = _t->getAutoconnectProgress();
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = std::move(_r); }  break;
        case 47: { QVector<int> _r = _t->scanCan();
            if (_a[0]) *reinterpret_cast< QVector<int>*>(_a[0]) = std::move(_r); }  break;
        case 48: { QVector<int> _r = _t->getCanDevsLast();
            if (_a[0]) *reinterpret_cast< QVector<int>*>(_a[0]) = std::move(_r); }  break;
        case 49: _t->ignoreCanChange((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 14:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractSocket::SocketError >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (BMSInterface::*)(const QString & , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BMSInterface::statusMessage)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (BMSInterface::*)(const QString & , const QString & , bool , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BMSInterface::messageDialog)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (BMSInterface::*)(const QString & , double , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BMSInterface::fwUploadStatus)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (BMSInterface::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BMSInterface::serialPortNotWritable)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (BMSInterface::*)(bool , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BMSInterface::fwRxChanged)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (BMSInterface::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BMSInterface::portConnectedChanged)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (BMSInterface::*)(double , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BMSInterface::autoConnectProgressUpdated)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (BMSInterface::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BMSInterface::autoConnectFinished)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (BMSInterface::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BMSInterface::pairingListUpdated)) {
                *result = 8;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject BMSInterface::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_BMSInterface.data,
    qt_meta_data_BMSInterface,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *BMSInterface::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BMSInterface::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_BMSInterface.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int BMSInterface::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 50)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 50;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 50)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 50;
    }
    return _id;
}

// SIGNAL 0
void BMSInterface::statusMessage(const QString & _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void BMSInterface::messageDialog(const QString & _t1, const QString & _t2, bool _t3, bool _t4)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void BMSInterface::fwUploadStatus(const QString & _t1, double _t2, bool _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void BMSInterface::serialPortNotWritable(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void BMSInterface::fwRxChanged(bool _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void BMSInterface::portConnectedChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void BMSInterface::autoConnectProgressUpdated(double _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void BMSInterface::autoConnectFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void BMSInterface::pairingListUpdated()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
