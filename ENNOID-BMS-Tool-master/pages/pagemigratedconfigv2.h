/*
    Original copyright 2018 Benjamin Vedder benjamin@vedder.se and the VESC Tool project ( https://github.com/vedderb/vesc_tool )

    Forked to:
    Copyright 2018 Danny Bokma github@diebie.nl (https://github.com/DieBieEngineering/DieBieMS-Tool)

    Now forked to:
    Copyright 2019 - 2020 Kevin Dionne kevin.dionne@ennoid.me (https://github.com/EnnoidMe/ENNOID-BMS-Tool)

    This file is part of ENNOID-BMS Tool.
*/

#ifndef PAGEMIGRATEDCONFIGV2_H
#define PAGEMIGRATEDCONFIGV2_H

#include <QWidget>
#include "datatypes.h"

class QLabel;
class QPushButton;
class QSpinBox;
class QComboBox;
class BMSInterface;

class PageMigratedConfigV2 : public QWidget
{
    Q_OBJECT

public:
    explicit PageMigratedConfigV2(QWidget *parent = 0);
    void setDieBieMS(BMSInterface *dieBieMS);

private slots:
    void requestActiveConfig();
    void requestDefaultConfig();
    void validateEditedConfig();
    void applyEditedConfig();
    void storeConfig();
    void configReceived(COMM_PACKET_ID packetId, bms_config_v2_t config);
    void configResultReceived(COMM_PACKET_ID packetId, int resultCode);
    void refreshModeState();

private:
    QString resultCodeToString(int resultCode) const;
    QString maskToString(const quint8 *mask, int len) const;
    void setConfigToWidgets(const bms_config_v2_t &config, const QString &origin);
    bool buildConfigFromWidgets(bms_config_v2_t &config, QString &error) const;
    void updateUiState(const QString &status = QString(), bool isError = false);

    BMSInterface *mDieBieMS;
    bool mHasConfig;
    bms_config_v2_t mConfig;

    QLabel *mModeLabel;
    QLabel *mStatusLabel;
    QLabel *mMaskLabel;
    QPushButton *mReadActiveButton;
    QPushButton *mReadDefaultButton;
    QPushButton *mValidateButton;
    QPushButton *mApplyButton;
    QPushButton *mStoreButton;
    QSpinBox *mCellOvSoftMv;
    QSpinBox *mCellOvHardMv;
    QSpinBox *mCellUvSoftMv;
    QSpinBox *mCellUvHardMv;
    QSpinBox *mChargeTempDeciC;
    QSpinBox *mDischargeTempDeciC;
    QSpinBox *mHardTempDeciC;
    QSpinBox *mPrechargePermille;
    QSpinBox *mPrechargeTimeoutMs;
    QSpinBox *mBalanceStartMv;
    QSpinBox *mBalanceDiffMv;
    QSpinBox *mTempSettleTimeMs;
    QSpinBox *mVpackGain;
    QSpinBox *mVpackOffset;
    QSpinBox *mIslGain;
    QSpinBox *mIslOffset;
    QSpinBox *mCurrentGain;
    QSpinBox *mCurrentOffset;
    QComboBox *mCurrentSign;
};

#endif // PAGEMIGRATEDCONFIGV2_H
