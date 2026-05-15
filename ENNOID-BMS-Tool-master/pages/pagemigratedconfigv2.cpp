/*
    Original copyright 2018 Benjamin Vedder benjamin@benjaminvedder.com and the VESC Tool project.
*/

#include "pagemigratedconfigv2.h"
#include "bmsinterface.h"

#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

namespace {

QSpinBox *makeSpinBox(int min, int max, const QString &suffix = QString())
{
    QSpinBox *box = new QSpinBox;
    box->setRange(min, max);
    box->setSuffix(suffix);
    return box;
}

}

PageMigratedConfigV2::PageMigratedConfigV2(QWidget *parent) :
    QWidget(parent),
    mDieBieMS(0),
    mHasConfig(false)
{
    memset(&mConfig, 0, sizeof(mConfig));

    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);

    mModeLabel = new QLabel(tr("Mode: waiting for capabilities"));
    mModeLabel->setWordWrap(true);
    root->addWidget(mModeLabel);

    mStatusLabel = new QLabel(tr("Read the active or default migrated Config V2 before validating or applying changes."));
    mStatusLabel->setWordWrap(true);
    root->addWidget(mStatusLabel);

    QHBoxLayout *buttonRow = new QHBoxLayout;
    mReadActiveButton = new QPushButton(tr("Read Active"));
    mReadDefaultButton = new QPushButton(tr("Read Defaults"));
    mValidateButton = new QPushButton(tr("Validate Edited"));
    mApplyButton = new QPushButton(tr("Apply RAM Only"));
    mStoreButton = new QPushButton(tr("Store"));
    buttonRow->addWidget(mReadActiveButton);
    buttonRow->addWidget(mReadDefaultButton);
    buttonRow->addWidget(mValidateButton);
    buttonRow->addWidget(mApplyButton);
    buttonRow->addWidget(mStoreButton);
    buttonRow->addStretch(1);
    root->addLayout(buttonRow);

    QGroupBox *thresholdBox = new QGroupBox(tr("Core Limits"));
    QFormLayout *thresholds = new QFormLayout(thresholdBox);
    mCellOvSoftMv = makeSpinBox(3000, 5000, tr(" mV"));
    mCellOvHardMv = makeSpinBox(3000, 5000, tr(" mV"));
    mCellUvSoftMv = makeSpinBox(1500, 4000, tr(" mV"));
    mCellUvHardMv = makeSpinBox(1500, 4000, tr(" mV"));
    mChargeTempDeciC = makeSpinBox(-400, 900, tr(" dC"));
    mDischargeTempDeciC = makeSpinBox(-400, 1100, tr(" dC"));
    mHardTempDeciC = makeSpinBox(-400, 1200, tr(" dC"));
    mPrechargePermille = makeSpinBox(1, 1000, tr(" /1000"));
    mPrechargeTimeoutMs = makeSpinBox(50, 10000, tr(" ms"));
    mBalanceStartMv = makeSpinBox(3000, 5000, tr(" mV"));
    mBalanceDiffMv = makeSpinBox(0, 1000, tr(" mV"));
    mTempSettleTimeMs = makeSpinBox(0, 10000, tr(" ms"));
    thresholds->addRow(tr("Cell OV soft"), mCellOvSoftMv);
    thresholds->addRow(tr("Cell OV hard"), mCellOvHardMv);
    thresholds->addRow(tr("Cell UV soft"), mCellUvSoftMv);
    thresholds->addRow(tr("Cell UV hard"), mCellUvHardMv);
    thresholds->addRow(tr("Charge temp limit"), mChargeTempDeciC);
    thresholds->addRow(tr("Discharge temp limit"), mDischargeTempDeciC);
    thresholds->addRow(tr("Hard temp limit"), mHardTempDeciC);
    thresholds->addRow(tr("Min precharge"), mPrechargePermille);
    thresholds->addRow(tr("Precharge timeout"), mPrechargeTimeoutMs);
    thresholds->addRow(tr("Balance start"), mBalanceStartMv);
    thresholds->addRow(tr("Balance diff"), mBalanceDiffMv);
    thresholds->addRow(tr("Temp settle"), mTempSettleTimeMs);
    root->addWidget(thresholdBox);

    QGroupBox *calibrationBox = new QGroupBox(tr("Calibration"));
    QFormLayout *calibration = new QFormLayout(calibrationBox);
    mVpackGain = makeSpinBox(-2000000000, 2000000000);
    mVpackOffset = makeSpinBox(-2000000000, 2000000000);
    mIslGain = makeSpinBox(-2000000000, 2000000000);
    mIslOffset = makeSpinBox(-2000000000, 2000000000);
    mCurrentGain = makeSpinBox(-2000000000, 2000000000);
    mCurrentOffset = makeSpinBox(-2000000000, 2000000000);
    mCurrentSign = new QComboBox;
    mCurrentSign->addItem(tr("Normal"), 0);
    mCurrentSign->addItem(tr("Inverted"), 1);
    calibration->addRow(tr("Vpack gain uV/V"), mVpackGain);
    calibration->addRow(tr("Vpack offset uV"), mVpackOffset);
    calibration->addRow(tr("ISL Vbat gain uV/V"), mIslGain);
    calibration->addRow(tr("ISL Vbat offset uV"), mIslOffset);
    calibration->addRow(tr("Current gain uA/A"), mCurrentGain);
    calibration->addRow(tr("Current offset uA"), mCurrentOffset);
    calibration->addRow(tr("Current sign"), mCurrentSign);
    root->addWidget(calibrationBox);

    mMaskLabel = new QLabel;
    mMaskLabel->setWordWrap(true);
    root->addWidget(mMaskLabel);
    root->addStretch(1);

    connect(mReadActiveButton, SIGNAL(clicked(bool)), this, SLOT(requestActiveConfig()));
    connect(mReadDefaultButton, SIGNAL(clicked(bool)), this, SLOT(requestDefaultConfig()));
    connect(mValidateButton, SIGNAL(clicked(bool)), this, SLOT(validateEditedConfig()));
    connect(mApplyButton, SIGNAL(clicked(bool)), this, SLOT(applyEditedConfig()));
    connect(mStoreButton, SIGNAL(clicked(bool)), this, SLOT(storeConfig()));

    updateUiState();
}

void PageMigratedConfigV2::setDieBieMS(BMSInterface *dieBieMS)
{
    mDieBieMS = dieBieMS;

    if (!mDieBieMS) {
        refreshModeState();
        return;
    }

    connect(mDieBieMS, SIGNAL(uiModeChanged(int,QString)), this, SLOT(refreshModeState()));
    connect(mDieBieMS->commands(), SIGNAL(configV2Received(COMM_PACKET_ID,bms_config_v2_t)),
            this, SLOT(configReceived(COMM_PACKET_ID,bms_config_v2_t)));
    connect(mDieBieMS->commands(), SIGNAL(configV2ResultReceived(COMM_PACKET_ID,int)),
            this, SLOT(configResultReceived(COMM_PACKET_ID,int)));

    refreshModeState();
}

QString PageMigratedConfigV2::resultCodeToString(int resultCode) const
{
    switch (resultCode) {
    case BMS_CONFIG_V2_RESULT_OK: return tr("OK");
    case BMS_CONFIG_V2_RESULT_UNSUPPORTED_VERSION: return tr("Unsupported schema version");
    case BMS_CONFIG_V2_RESULT_BAD_MAGIC: return tr("Bad magic");
    case BMS_CONFIG_V2_RESULT_BAD_LENGTH: return tr("Bad payload length or reserved bytes");
    case BMS_CONFIG_V2_RESULT_BAD_CRC: return tr("Bad internal config CRC");
    case BMS_CONFIG_V2_RESULT_WRONG_HARDWARE_PROFILE: return tr("Wrong hardware profile");
    case BMS_CONFIG_V2_RESULT_INVALID_CELL_COUNT: return tr("Invalid cell count");
    case BMS_CONFIG_V2_RESULT_INVALID_TEMP_COUNT: return tr("Invalid temperature count");
    case BMS_CONFIG_V2_RESULT_INVALID_THRESHOLD_ORDER: return tr("Threshold order invalid");
    case BMS_CONFIG_V2_RESULT_INVALID_THRESHOLD_RANGE: return tr("Threshold range invalid");
    case BMS_CONFIG_V2_RESULT_INVALID_MASK: return tr("Mask contains out-of-range bits");
    case BMS_CONFIG_V2_RESULT_INVALID_CALIBRATION: return tr("Calibration fields invalid");
    case BMS_CONFIG_V2_RESULT_STORE_FAILED: return tr("Store failed");
    case BMS_CONFIG_V2_RESULT_READBACK_FAILED: return tr("Stored config readback failed");
    case BMS_CONFIG_V2_RESULT_UNSUPPORTED_IN_CURRENT_MODE: return tr("Unsupported in current mode");
    default: return tr("Unknown result %1").arg(resultCode);
    }
}

QString PageMigratedConfigV2::maskToString(const quint8 *mask, int len) const
{
    QStringList parts;
    for (int i = 0; i < len; ++i) {
        parts << QStringLiteral("%1").arg(mask[i], 2, 16, QLatin1Char('0')).toUpper();
    }
    return parts.join(QStringLiteral(" "));
}

void PageMigratedConfigV2::setConfigToWidgets(const bms_config_v2_t &config, const QString &origin)
{
    mHasConfig = true;
    mConfig = config;

    mCellOvSoftMv->setValue(config.cellOvSoftMv);
    mCellOvHardMv->setValue(config.cellOvHardMv);
    mCellUvSoftMv->setValue(config.cellUvSoftMv);
    mCellUvHardMv->setValue(config.cellUvHardMv);
    mChargeTempDeciC->setValue(config.chargeTempLimitDeciC);
    mDischargeTempDeciC->setValue(config.dischargeTempLimitDeciC);
    mHardTempDeciC->setValue(config.hardTempLimitDeciC);
    mPrechargePermille->setValue(config.minimalPrechargePermille);
    mPrechargeTimeoutMs->setValue(config.lowCurrentPrechargeTimeoutMs);
    mBalanceStartMv->setValue(config.balanceStartMv);
    mBalanceDiffMv->setValue(config.balanceDiffMv);
    mTempSettleTimeMs->setValue(config.tempSettleTimeMs);
    mVpackGain->setValue(config.vpackGainMicroPerVolt);
    mVpackOffset->setValue(config.vpackOffsetMicroVolt);
    mIslGain->setValue(config.islVbatGainMicroPerVolt);
    mIslOffset->setValue(config.islVbatOffsetMicroVolt);
    mCurrentGain->setValue(config.currentGainMicroPerAmp);
    mCurrentOffset->setValue(config.currentOffsetMicroAmp);
    mCurrentSign->setCurrentIndex(qMin<int>(config.currentSign, 1));

    mMaskLabel->setText(
        tr("Read source: %1\nRequired cell mask: %2\nRequired temp mask: %3\nBalance-allowed mask: %4")
        .arg(origin,
             maskToString(config.requiredCellMask, BMS_CONFIG_V2_MASK_BYTES),
             maskToString(config.requiredTempMask, BMS_CONFIG_V2_MASK_BYTES),
             maskToString(config.balanceAllowedMask, BMS_CONFIG_V2_MASK_BYTES)));

    updateUiState(tr("Loaded Config V2 from %1.").arg(origin), false);
}

bool PageMigratedConfigV2::buildConfigFromWidgets(bms_config_v2_t &config, QString &error) const
{
    if (!mHasConfig) {
        error = tr("Read the active or default migrated config first so masks and hardware fields are preserved.");
        return false;
    }

    config = mConfig;
    config.cellOvSoftMv = static_cast<quint16>(mCellOvSoftMv->value());
    config.cellOvHardMv = static_cast<quint16>(mCellOvHardMv->value());
    config.cellUvSoftMv = static_cast<quint16>(mCellUvSoftMv->value());
    config.cellUvHardMv = static_cast<quint16>(mCellUvHardMv->value());
    config.chargeTempLimitDeciC = static_cast<qint16>(mChargeTempDeciC->value());
    config.dischargeTempLimitDeciC = static_cast<qint16>(mDischargeTempDeciC->value());
    config.hardTempLimitDeciC = static_cast<qint16>(mHardTempDeciC->value());
    config.minimalPrechargePermille = static_cast<quint16>(mPrechargePermille->value());
    config.lowCurrentPrechargeTimeoutMs = static_cast<quint16>(mPrechargeTimeoutMs->value());
    config.balanceStartMv = static_cast<quint16>(mBalanceStartMv->value());
    config.balanceDiffMv = static_cast<quint16>(mBalanceDiffMv->value());
    config.tempSettleTimeMs = static_cast<quint16>(mTempSettleTimeMs->value());
    config.vpackGainMicroPerVolt = mVpackGain->value();
    config.vpackOffsetMicroVolt = mVpackOffset->value();
    config.islVbatGainMicroPerVolt = mIslGain->value();
    config.islVbatOffsetMicroVolt = mIslOffset->value();
    config.currentGainMicroPerAmp = mCurrentGain->value();
    config.currentOffsetMicroAmp = mCurrentOffset->value();
    config.currentSign = static_cast<quint8>(mCurrentSign->currentData().toUInt());

    if (config.cellOvHardMv <= config.cellOvSoftMv ||
            config.cellUvHardMv >= config.cellUvSoftMv ||
            config.cellUvSoftMv >= config.cellOvSoftMv) {
        error = tr("The cell threshold ordering is invalid.");
        return false;
    }

    if (config.hardTempLimitDeciC < config.chargeTempLimitDeciC ||
            config.hardTempLimitDeciC < config.dischargeTempLimitDeciC) {
        error = tr("The hard temperature limit must be greater than or equal to the charge and discharge limits.");
        return false;
    }

    if (config.vpackGainMicroPerVolt <= 0 ||
            config.islVbatGainMicroPerVolt <= 0 ||
            config.currentGainMicroPerAmp == 0) {
        error = tr("Gain values must stay non-zero and positive where required.");
        return false;
    }

    error.clear();
    return true;
}

void PageMigratedConfigV2::updateUiState(const QString &status, bool isError)
{
    bool migratedMode = false;
    bool editable = false;
    bool storeSupported = false;

    if (mDieBieMS) {
        migratedMode = mDieBieMS->getUiMode() == BMS_UI_MODE_MIGRATED_MONITORING_ONLY ||
                mDieBieMS->getUiMode() == BMS_UI_MODE_MIGRATED_CONFIG_V2;
        editable = mDieBieMS->configV2Supported();
        storeSupported = (mDieBieMS->capabilitiesValid() &&
                          (mDieBieMS->getUiMode() == BMS_UI_MODE_MIGRATED_CONFIG_V2) &&
                          (mDieBieMS->commands() != 0));
    }

    mReadActiveButton->setEnabled(migratedMode);
    mReadDefaultButton->setEnabled(migratedMode);
    mValidateButton->setEnabled(editable && mHasConfig);
    mApplyButton->setEnabled(editable && mHasConfig);
    mStoreButton->setEnabled(storeSupported && mHasConfig && false);

    const QList<QWidget*> editors = {
        mCellOvSoftMv, mCellOvHardMv, mCellUvSoftMv, mCellUvHardMv,
        mChargeTempDeciC, mDischargeTempDeciC, mHardTempDeciC,
        mPrechargePermille, mPrechargeTimeoutMs, mBalanceStartMv,
        mBalanceDiffMv, mTempSettleTimeMs, mVpackGain, mVpackOffset,
        mIslGain, mIslOffset, mCurrentGain, mCurrentOffset, mCurrentSign
    };

    for (QWidget *editor : editors) {
        editor->setEnabled(editable && mHasConfig);
    }

    if (!status.isEmpty()) {
        mStatusLabel->setText(status);
        mStatusLabel->setStyleSheet(isError ? QStringLiteral("QLabel { color: red; }")
                                            : QStringLiteral("QLabel { color: black; }"));
    }
}

void PageMigratedConfigV2::requestActiveConfig()
{
    if (mDieBieMS) {
        mDieBieMS->commands()->getConfigV2();
    }
}

void PageMigratedConfigV2::requestDefaultConfig()
{
    if (mDieBieMS) {
        mDieBieMS->commands()->getConfigDefaultV2();
    }
}

void PageMigratedConfigV2::validateEditedConfig()
{
    bms_config_v2_t config;
    QString error;
    if (!buildConfigFromWidgets(config, error)) {
        updateUiState(error, true);
        return;
    }

    if (mDieBieMS) {
        mDieBieMS->commands()->validateConfigV2(config);
    }
}

void PageMigratedConfigV2::applyEditedConfig()
{
    bms_config_v2_t config;
    QString error;
    if (!buildConfigFromWidgets(config, error)) {
        updateUiState(error, true);
        return;
    }

    if (mDieBieMS) {
        mDieBieMS->commands()->setConfigV2(config);
    }
}

void PageMigratedConfigV2::storeConfig()
{
    if (mDieBieMS) {
        mDieBieMS->commands()->storeConfigV2();
    }
}

void PageMigratedConfigV2::configReceived(COMM_PACKET_ID packetId, bms_config_v2_t config)
{
    if (packetId == COMM_BMS_GET_CONFIG_V2) {
        setConfigToWidgets(config, tr("active config"));
    } else if (packetId == COMM_BMS_GET_CONFIG_DEFAULT_V2) {
        setConfigToWidgets(config, tr("default config"));
    }
}

void PageMigratedConfigV2::configResultReceived(COMM_PACKET_ID packetId, int resultCode)
{
    const QString resultText = resultCodeToString(resultCode);
    const bool ok = resultCode == BMS_CONFIG_V2_RESULT_OK;

    if (packetId == COMM_BMS_SET_CONFIG_V2 && ok) {
        QString error;
        bms_config_v2_t updated;
        if (buildConfigFromWidgets(updated, error)) {
            mConfig = updated;
        }
    }

    QString op;
    switch (packetId) {
    case COMM_BMS_VALIDATE_CONFIG_V2: op = tr("Validate"); break;
    case COMM_BMS_SET_CONFIG_V2: op = tr("Apply RAM-only"); break;
    case COMM_BMS_STORE_CONFIG_V2: op = tr("Store"); break;
    default: op = tr("Config V2"); break;
    }

    updateUiState(tr("%1 result: %2").arg(op, resultText), !ok);
}

void PageMigratedConfigV2::refreshModeState()
{
    if (!mDieBieMS) {
        mModeLabel->setText(tr("Mode: no BMS interface"));
        updateUiState();
        return;
    }

    QString modeLine = tr("Mode: %1").arg(mDieBieMS->getUiModeName());
    if (mDieBieMS->capabilitiesValid()) {
        modeLine += tr("\n%1").arg(mDieBieMS->capabilitySummary());
    }
    mModeLabel->setText(modeLine);

    if (mDieBieMS->getUiMode() == BMS_UI_MODE_MIGRATED_MONITORING_ONLY) {
        updateUiState(tr("Migrated hardware detected. Config V2 can be read for inspection, but firmware did not advertise an editable config mode."), false);
    } else if (mDieBieMS->getUiMode() == BMS_UI_MODE_MIGRATED_CONFIG_V2) {
        updateUiState(tr("Migrated Config V2 mode detected. Apply performs RAM-only validation/apply. Store remains disabled until persistent storage is validated on hardware."), false);
    } else {
        updateUiState(tr("Migrated Config V2 is only available for migrated application firmware."), true);
    }
}
