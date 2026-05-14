/*
    Original copyright 2018 Benjamin Vedder benjamin@vedder.se and the VESC Tool project ( https://github.com/vedderb/vesc_tool )

    Forked to:
    Copyright 2018 Danny Bokma github@diebie.nl (https://github.com/DieBieEngineering/DieBieMS-Tool)

    Now forked to:
    Copyright 2019 - 2020 Kevin Dionne kevin.dionne@ennoid.me (https://github.com/EnnoidMe/ENNOID-BMS-Tool)

    This file is part of ENNOID-BMS Tool.

    ENNOID-BMS Tool is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ENNOID-BMS Tool is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "pagemastergeneral.h"
#include "ui_pagemastergeneral.h"

PageMasterGeneral::PageMasterGeneral(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PageMasterGeneral)
{
    ui->setupUi(this);
    mDieBieMS = 0;
}

PageMasterGeneral::~PageMasterGeneral()
{
    delete ui;
}

BMSInterface *PageMasterGeneral::bms() const
{
    return mDieBieMS;
}

void PageMasterGeneral::setDieBieMS(BMSInterface *dieBieMS)
{
    mDieBieMS = dieBieMS;

    if (mDieBieMS) {
        ui->powerStateTab->addRowSeparator(tr("Onstate"));
        ui->powerStateTab->addParamRow(mDieBieMS->bmsConfig(), "pulseToggleButton");
        ui->powerStateTab->addParamRow(mDieBieMS->bmsConfig(), "notUsedCurrentThreshold");
        ui->powerStateTab->addParamRow(mDieBieMS->bmsConfig(), "notUsedTimeout");
        ui->powerStateTab->addParamRow(mDieBieMS->bmsConfig(), "powerDownDelay");
        ui->powerStateTab->addParamRow(mDieBieMS->bmsConfig(), "allowForceOn");
        ui->powerStateTab->addRowSeparator(tr("Jump to"));
        ui->powerStateTab->addParamRow(mDieBieMS->bmsConfig(), "extEnableState");
        ui->powerStateTab->addParamRow(mDieBieMS->bmsConfig(), "chargeEnableState");
        ui->masterLimitsTab->addRowSeparator(tr("Current"));
        ui->masterLimitsTab->addParamRow(mDieBieMS->bmsConfig(), "maxAllowedCurrent");
        ui->masterLimitsTab->addParamRow(mDieBieMS->bmsConfig(), "maxAllowedChargeCurrent");
        ui->masterLimitsTab->addRowSeparator(tr("Temperature discharging"));
        ui->masterLimitsTab->addParamRow(mDieBieMS->bmsConfig(), "allowedTempBattDischargingMax");
        ui->masterLimitsTab->addParamRow(mDieBieMS->bmsConfig(), "allowedTempBattDischargingMin");
        ui->masterLimitsTab->addRowSeparator(tr("Temperature charging"));
        ui->masterLimitsTab->addParamRow(mDieBieMS->bmsConfig(), "allowedTempBattChargingMax");
        ui->masterLimitsTab->addParamRow(mDieBieMS->bmsConfig(), "allowedTempBattChargingMin");
        ui->masterLimitsTab->addRowSeparator(tr("Temperature cooling/heating"));
        ui->masterLimitsTab->addParamRow(mDieBieMS->bmsConfig(), "allowedTempBattCoolingMax");
        ui->masterLimitsTab->addParamRow(mDieBieMS->bmsConfig(), "allowedTempBattCoolingMin");
        ui->masterLimitsTab->addRowSeparator(tr("Temperature Master board"));
        ui->masterLimitsTab->addParamRow(mDieBieMS->bmsConfig(), "allowedTempBMSMax");
        ui->masterLimitsTab->addParamRow(mDieBieMS->bmsConfig(), "allowedTempBMSMin");

        ui->canTab->addRowSeparator(tr("CAN Configuration"));
        ui->canTab->addParamRow(mDieBieMS->bmsConfig(), "CANID");
        ui->canTab->addParamRow(mDieBieMS->bmsConfig(), "CANIDStyle");
        ui->canTab->addParamRow(mDieBieMS->bmsConfig(), "CANBaudRate");
        ui->canTab->addParamRow(mDieBieMS->bmsConfig(), "loadCANProtocol");
        ui->canTab->addParamRow(mDieBieMS->bmsConfig(), "chargerCANProtocol");
        ui->canTab->addRowSeparator(tr("CAN advanced features"));
        ui->canTab->addParamRow(mDieBieMS->bmsConfig(), "useCANSafetyInput");
        ui->canTab->addParamRow(mDieBieMS->bmsConfig(), "useCANDelayedPowerDown");

        ui->masterSensorsTab->addInfoRow(tr("Config fields on this page are legacy ENNOID NTC settings. "
                                            "Migrated 75-cell firmware uses Enepaq temperature channels over "
                                            "COMM_EBMS_GET_EXP_TEMP. Config read/write is disabled; use realtime "
                                            "monitoring only."));
        ui->masterSensorsTab->addRowSeparator(tr("Legacy NTC battery settings - not used by migrated firmware"));
        ui->masterSensorsTab->addParamRow(mDieBieMS->bmsConfig(), "tempEnableMaskBattery", false);
        ui->masterSensorsTab->addParamRow(mDieBieMS->bmsConfig(), "noOfTempSensorPerModule", false);
        ui->masterSensorsTab->addParamRow(mDieBieMS->bmsConfig(), "NTCLTC25Deg", false);
        ui->masterSensorsTab->addParamRow(mDieBieMS->bmsConfig(), "NTCLTCBeta", false);
        ui->masterSensorsTab->addRowSeparator(tr("Legacy NTC expansion settings - not used by migrated firmware"));
        ui->masterSensorsTab->addParamRow(mDieBieMS->bmsConfig(), "tempEnableMaskExpansion", false);
        ui->masterSensorsTab->addParamRow(mDieBieMS->bmsConfig(), "noOfExpansionBoard", false);
        ui->masterSensorsTab->addParamRow(mDieBieMS->bmsConfig(), "noOfTempSensorPerExpansionBoard", false);
        ui->masterSensorsTab->addParamRow(mDieBieMS->bmsConfig(), "NTCEXP25Deg", false);
        ui->masterSensorsTab->addParamRow(mDieBieMS->bmsConfig(), "NTCEXPBeta", false);
        ui->masterSensorsTab->addRowSeparator(tr("Legacy temperature config - monitoring only"));
        ui->masterSensorsTab->addParamRow(mDieBieMS->bmsConfig(), "maxUnderAndOverTemperatureErrorCount", false);
        ui->masterSensorsTab->addParamRow(mDieBieMS->bmsConfig(), "humidityICType", false);


  //      ui->masterSensorsTab->addRowSeparator(tr("Water detect enable mask"));
   //     ui->masterSensorsTab->addParamRow(mDieBieMS->bmsConfig(), "waterSensorEnableMask");
   //     ui->masterSensorsTab->addParamRow(mDieBieMS->bmsConfig(), "waterSensorThreshold");
    }
}
