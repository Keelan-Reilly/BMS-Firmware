/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.18)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.18. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[44];
    char stringdata0[1109];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 9), // "timerSlot"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 14), // "showStatusInfo"
QT_MOC_LITERAL(4, 37, 4), // "info"
QT_MOC_LITERAL(5, 42, 6), // "isGood"
QT_MOC_LITERAL(6, 49, 17), // "showMessageDialog"
QT_MOC_LITERAL(7, 67, 5), // "title"
QT_MOC_LITERAL(8, 73, 3), // "msg"
QT_MOC_LITERAL(9, 77, 8), // "richText"
QT_MOC_LITERAL(10, 86, 21), // "serialPortNotWritable"
QT_MOC_LITERAL(11, 108, 4), // "port"
QT_MOC_LITERAL(12, 113, 14), // "bmsconfUpdated"
QT_MOC_LITERAL(13, 128, 20), // "bmsConfigCheckResult"
QT_MOC_LITERAL(14, 149, 12), // "paramsNotSet"
QT_MOC_LITERAL(15, 162, 28), // "on_actionReconnect_triggered"
QT_MOC_LITERAL(16, 191, 29), // "on_actionDisconnect_triggered"
QT_MOC_LITERAL(17, 221, 25), // "on_actionReboot_triggered"
QT_MOC_LITERAL(18, 247, 31), // "on_actionReadBMScconf_triggered"
QT_MOC_LITERAL(19, 279, 38), // "on_actionReadBMScconfDefault_..."
QT_MOC_LITERAL(20, 318, 32), // "on_actionWriteBMScconf_triggered"
QT_MOC_LITERAL(21, 351, 33), // "on_actionSaveBMSConfXml_trigg..."
QT_MOC_LITERAL(22, 385, 33), // "on_actionLoadBMSConfXml_trigg..."
QT_MOC_LITERAL(23, 419, 23), // "on_actionExit_triggered"
QT_MOC_LITERAL(24, 443, 24), // "on_actionAbout_triggered"
QT_MOC_LITERAL(25, 468, 32), // "on_actionLibrariesUsed_triggered"
QT_MOC_LITERAL(26, 501, 29), // "on_pageList_currentRowChanged"
QT_MOC_LITERAL(27, 531, 10), // "currentRow"
QT_MOC_LITERAL(28, 542, 42), // "on_actionParameterEditorBMScc..."
QT_MOC_LITERAL(29, 585, 38), // "on_actionParameterEditorInfo_..."
QT_MOC_LITERAL(30, 624, 45), // "on_actionSaveBMSConfiguration..."
QT_MOC_LITERAL(31, 670, 49), // "on_actionSaveBMSConfiguration..."
QT_MOC_LITERAL(32, 720, 38), // "on_actionTerminalPrintFaults_..."
QT_MOC_LITERAL(33, 759, 35), // "on_actionTerminalShowHelp_tri..."
QT_MOC_LITERAL(34, 795, 32), // "on_actionTerminalClear_triggered"
QT_MOC_LITERAL(35, 828, 39), // "on_actionTerminalPrintThreads..."
QT_MOC_LITERAL(36, 868, 23), // "on_actionCanFwd_toggled"
QT_MOC_LITERAL(37, 892, 4), // "arg1"
QT_MOC_LITERAL(38, 897, 36), // "on_actionSafetyInformation_tr..."
QT_MOC_LITERAL(39, 934, 36), // "on_actionWarrantyStatement_tr..."
QT_MOC_LITERAL(40, 971, 40), // "on_actionDieBieMSToolChangelo..."
QT_MOC_LITERAL(41, 1012, 36), // "on_actionFirmwareChangelog_tr..."
QT_MOC_LITERAL(42, 1049, 26), // "on_actionLicense_triggered"
QT_MOC_LITERAL(43, 1076, 32) // "on_actionStoreBMScconf_triggered"

    },
    "MainWindow\0timerSlot\0\0showStatusInfo\0"
    "info\0isGood\0showMessageDialog\0title\0"
    "msg\0richText\0serialPortNotWritable\0"
    "port\0bmsconfUpdated\0bmsConfigCheckResult\0"
    "paramsNotSet\0on_actionReconnect_triggered\0"
    "on_actionDisconnect_triggered\0"
    "on_actionReboot_triggered\0"
    "on_actionReadBMScconf_triggered\0"
    "on_actionReadBMScconfDefault_triggered\0"
    "on_actionWriteBMScconf_triggered\0"
    "on_actionSaveBMSConfXml_triggered\0"
    "on_actionLoadBMSConfXml_triggered\0"
    "on_actionExit_triggered\0"
    "on_actionAbout_triggered\0"
    "on_actionLibrariesUsed_triggered\0"
    "on_pageList_currentRowChanged\0currentRow\0"
    "on_actionParameterEditorBMScconf_triggered\0"
    "on_actionParameterEditorInfo_triggered\0"
    "on_actionSaveBMSConfigurationHeader_triggered\0"
    "on_actionSaveBMSConfigurationHeaderWrap_triggered\0"
    "on_actionTerminalPrintFaults_triggered\0"
    "on_actionTerminalShowHelp_triggered\0"
    "on_actionTerminalClear_triggered\0"
    "on_actionTerminalPrintThreads_triggered\0"
    "on_actionCanFwd_toggled\0arg1\0"
    "on_actionSafetyInformation_triggered\0"
    "on_actionWarrantyStatement_triggered\0"
    "on_actionDieBieMSToolChangelog_triggered\0"
    "on_actionFirmwareChangelog_triggered\0"
    "on_actionLicense_triggered\0"
    "on_actionStoreBMScconf_triggered"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      33,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  179,    2, 0x08 /* Private */,
       3,    2,  180,    2, 0x08 /* Private */,
       6,    4,  185,    2, 0x08 /* Private */,
      10,    1,  194,    2, 0x08 /* Private */,
      12,    0,  197,    2, 0x08 /* Private */,
      13,    1,  198,    2, 0x08 /* Private */,
      15,    0,  201,    2, 0x08 /* Private */,
      16,    0,  202,    2, 0x08 /* Private */,
      17,    0,  203,    2, 0x08 /* Private */,
      18,    0,  204,    2, 0x08 /* Private */,
      19,    0,  205,    2, 0x08 /* Private */,
      20,    0,  206,    2, 0x08 /* Private */,
      21,    0,  207,    2, 0x08 /* Private */,
      22,    0,  208,    2, 0x08 /* Private */,
      23,    0,  209,    2, 0x08 /* Private */,
      24,    0,  210,    2, 0x08 /* Private */,
      25,    0,  211,    2, 0x08 /* Private */,
      26,    1,  212,    2, 0x08 /* Private */,
      28,    0,  215,    2, 0x08 /* Private */,
      29,    0,  216,    2, 0x08 /* Private */,
      30,    0,  217,    2, 0x08 /* Private */,
      31,    0,  218,    2, 0x08 /* Private */,
      32,    0,  219,    2, 0x08 /* Private */,
      33,    0,  220,    2, 0x08 /* Private */,
      34,    0,  221,    2, 0x08 /* Private */,
      35,    0,  222,    2, 0x08 /* Private */,
      36,    1,  223,    2, 0x08 /* Private */,
      38,    0,  226,    2, 0x08 /* Private */,
      39,    0,  227,    2, 0x08 /* Private */,
      40,    0,  228,    2, 0x08 /* Private */,
      41,    0,  229,    2, 0x08 /* Private */,
      42,    0,  230,    2, 0x08 /* Private */,
      43,    0,  231,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    4,    5,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::Bool, QMetaType::Bool,    7,    8,    5,    9,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QStringList,   14,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   27,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   37,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->timerSlot(); break;
        case 1: _t->showStatusInfo((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 2: _t->showMessageDialog((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4]))); break;
        case 3: _t->serialPortNotWritable((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->bmsconfUpdated(); break;
        case 5: _t->bmsConfigCheckResult((*reinterpret_cast< QStringList(*)>(_a[1]))); break;
        case 6: _t->on_actionReconnect_triggered(); break;
        case 7: _t->on_actionDisconnect_triggered(); break;
        case 8: _t->on_actionReboot_triggered(); break;
        case 9: _t->on_actionReadBMScconf_triggered(); break;
        case 10: _t->on_actionReadBMScconfDefault_triggered(); break;
        case 11: _t->on_actionWriteBMScconf_triggered(); break;
        case 12: _t->on_actionSaveBMSConfXml_triggered(); break;
        case 13: _t->on_actionLoadBMSConfXml_triggered(); break;
        case 14: _t->on_actionExit_triggered(); break;
        case 15: _t->on_actionAbout_triggered(); break;
        case 16: _t->on_actionLibrariesUsed_triggered(); break;
        case 17: _t->on_pageList_currentRowChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 18: _t->on_actionParameterEditorBMScconf_triggered(); break;
        case 19: _t->on_actionParameterEditorInfo_triggered(); break;
        case 20: _t->on_actionSaveBMSConfigurationHeader_triggered(); break;
        case 21: _t->on_actionSaveBMSConfigurationHeaderWrap_triggered(); break;
        case 22: _t->on_actionTerminalPrintFaults_triggered(); break;
        case 23: _t->on_actionTerminalShowHelp_triggered(); break;
        case 24: _t->on_actionTerminalClear_triggered(); break;
        case 25: _t->on_actionTerminalPrintThreads_triggered(); break;
        case 26: _t->on_actionCanFwd_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 27: _t->on_actionSafetyInformation_triggered(); break;
        case 28: _t->on_actionWarrantyStatement_triggered(); break;
        case 29: _t->on_actionDieBieMSToolChangelog_triggered(); break;
        case 30: _t->on_actionFirmwareChangelog_triggered(); break;
        case 31: _t->on_actionLicense_triggered(); break;
        case 32: _t->on_actionStoreBMScconf_triggered(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 33)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 33;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 33)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 33;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
