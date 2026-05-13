/*
	Copyright 2016 - 2017 Benjamin Vedder	benjamin@vedder.se

	This file is part of the VESC firmware.

	The VESC firmware is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    The VESC firmware is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "modTerminal.h"
#include "mxconstants.h"

static const uint8_t terminalDiagnosticSampleCount = 4u;

static const char *terminalBoolToString(bool value) {
	return value ? "true" : "false";
}

static const char *terminalOperationalStateToString(OperationalStateTypedef state) {
	switch(state) {
		case OP_STATE_CHARGING:
			return "Charging";
		case OP_STATE_LOAD_ENABLED:
			return "Load enabled";
		case OP_STATE_CHARGED:
			return "Charged";
		case OP_STATE_BALANCING:
			return "Balancing";
		case OP_STATE_ERROR_PRECHARGE:
			return "Precharge error";
		case OP_STATE_ERROR:
			return "Error";
		case OP_STATE_FORCEON:
			return "Forced on";
		case OP_STATE_POWER_DOWN:
			return "Power down";
		case OP_STATE_EXTERNAL:
			return "External (USB/CAN)";
		default:
			return "Unknown";
	}
}

static void terminalPrintCellSamples(const char *label, uint8_t startIndex, uint8_t count) {
	for(uint8_t sampleIndex = 0u; sampleIndex < count; sampleIndex++) {
		uint8_t cellIndex = (uint8_t)(startIndex + sampleIndex);
		modCommandsPrintf("%s cell[%02u] : %.4fV raw=%u dev=%u ch=%u",
			label,
			cellIndex,
			packState.cellVoltagesLTC6812[cellIndex].cellVoltage,
			packState.cellVoltagesLTC6812[cellIndex].rawCode,
			packState.cellVoltagesLTC6812[cellIndex].deviceIndex,
			packState.cellVoltagesLTC6812[cellIndex].cellIndexOnDevice);
	}
}

static void terminalPrintTempSamples(const char *label, uint8_t startIndex, uint8_t count) {
	for(uint8_t sampleIndex = 0u; sampleIndex < count; sampleIndex++) {
		uint8_t tempIndex = (uint8_t)(startIndex + sampleIndex);
		modCommandsPrintf("%s temp[%02u] : raw=%umV code=%u conv=%.1fC valid=%s dev=%u ch=%u",
			label,
			tempIndex,
			packState.tempSensorVoltagesLTC6812[tempIndex].milliVolts,
			packState.tempSensorVoltagesLTC6812[tempIndex].rawCode,
			packState.temperaturesLTC6812[tempIndex],
			terminalBoolToString(packState.temperaturesLTC6812Valid[tempIndex]),
			packState.tempSensorVoltagesLTC6812[tempIndex].deviceIndex,
			packState.tempSensorVoltagesLTC6812[tempIndex].channelIndexOnDevice);
	}
}

static void terminalPrintMeasurementStatusSummary(void) {
	modCommandsPrintf("Measurement status:");
	modCommandsPrintf("  cell valid=%s err=%u count=%u",
		terminalBoolToString(packState.cellVoltageReadoutValid),
		packState.cellVoltageReadoutErrorCount,
		packState.cellVoltageReadoutCount);
	modCommandsPrintf("  temp valid=%s err=%u count=%u",
		terminalBoolToString(packState.temperatureReadoutValid),
		packState.temperatureReadoutErrorCount,
		packState.temperatureReadoutCount);
	modCommandsPrintf("  vbat valid=%s current valid=%s vpack valid=%s",
		terminalBoolToString(packState.vBatReadoutValid),
		terminalBoolToString(packState.currentReadoutValid),
		terminalBoolToString(packState.vPackReadoutValid));
	modCommandsPrintf("  power monitor valid=%s err=%u",
		terminalBoolToString(packState.powerMonitorReadoutValid),
		packState.powerMonitorReadoutErrorCount);
}

static void terminalPrintCellDiagnostics(void) {
	driverLTC6812StatusTypedef cellChainStatus = driverSWLTC6812GetCellChainStatus();
	uint8_t totalCells = BMS_TOTAL_CELLS;

	modCommandsPrintf("Cell-chain diagnostics:");
	modCommandsPrintf("  total=%u readoutValid=%s readoutErr=%u count=%u",
		totalCells,
		terminalBoolToString(packState.cellVoltageReadoutValid),
		packState.cellVoltageReadoutErrorCount,
		packState.cellVoltageReadoutCount);
	modCommandsPrintf("  chain lastReadValid=%s lastReadPECErrors=%u",
		terminalBoolToString(cellChainStatus.lastReadValid),
		cellChainStatus.lastReadPECErrors);
	modCommandsPrintf("  min=%.4fV avg=%.4fV max=%.4fV mismatch=%.4fV",
		packState.cellVoltageLow,
		packState.cellVoltageAverage,
		packState.cellVoltageHigh,
		packState.cellVoltageMisMatch);
	terminalPrintCellSamples("  first", 0u, terminalDiagnosticSampleCount);
	terminalPrintCellSamples("  last ", (uint8_t)(totalCells - terminalDiagnosticSampleCount), terminalDiagnosticSampleCount);
}

static void terminalPrintTempDiagnostics(void) {
	driverLTC6812StatusTypedef tempChainStatus = driverSWLTC6812GetTemperatureChainStatus();
	uint8_t totalTemps = BMS_TOTAL_TEMPS;

	modCommandsPrintf("TEMP-chain diagnostics:");
	modCommandsPrintf("  total=%u readoutValid=%s readoutErr=%u count=%u",
		totalTemps,
		terminalBoolToString(packState.temperatureReadoutValid),
		packState.temperatureReadoutErrorCount,
		packState.temperatureReadoutCount);
	modCommandsPrintf("  chain lastReadValid=%s lastReadPECErrors=%u",
		terminalBoolToString(tempChainStatus.lastReadValid),
		tempChainStatus.lastReadPECErrors);
	modCommandsPrintf("  sensor-enable status is shared with TEMP-chain config readback in the current driver");
	modCommandsPrintf("  battery temp high=%.1fC avg=%.1fC low=%.1fC",
		packState.tempBatteryHigh,
		packState.tempBatteryAverage,
		packState.tempBatteryLow);
	modCommandsPrintf("  bms temp high=%.1fC avg=%.1fC low=%.1fC",
		packState.tempBMSHigh,
		packState.tempBMSAverage,
		packState.tempBMSLow);
	terminalPrintTempSamples("  first", 0u, terminalDiagnosticSampleCount);
	terminalPrintTempSamples("  last ", (uint8_t)(totalTemps - terminalDiagnosticSampleCount), terminalDiagnosticSampleCount);
}

static void terminalPrintPowerDiagnostics(void) {
	modCommandsPrintf("Power-monitor diagnostics:");
	modCommandsPrintf("  Vbat(ISL28022)=%.3fV valid=%s",
		packState.vBatVoltage,
		terminalBoolToString(packState.vBatReadoutValid));
	modCommandsPrintf("  Current(ISL28022)=%.3fA valid=%s",
		packState.loCurrentLoadCurrent,
		terminalBoolToString(packState.currentReadoutValid));
	modCommandsPrintf("  Vpack(PA1 ADC)=%.3fV valid=%s",
		packState.vPackVoltage,
		terminalBoolToString(packState.vPackReadoutValid));
	modCommandsPrintf("  powerMonitorValid=%s err=%u",
		terminalBoolToString(packState.powerMonitorReadoutValid),
		packState.powerMonitorReadoutErrorCount);
	modCommandsPrintf("  ChargeDetect GPIO=%s threshold=%.2fA chargeCurrentDetected=%s",
		terminalBoolToString(modPowerStateChargerDetected()),
		generalConfig->chargerEnabledThreshold,
		terminalBoolToString(packState.chargeCurrentDetected));
	modCommandsPrintf("  PowerButton GPIO=%s debounced=%s",
		terminalBoolToString(modPowerStateGetButtonPressedState()),
		terminalBoolToString(packState.powerButtonActuated));
}

static void terminalPrintOutputDiagnostics(void) {
	bool chargePermissionEffective = driverHWSwitchesGetSwitchState(SWITCH_CHARGE_ENABLE);
	bool chargerSafetyEffective = driverHWSwitchesGetSwitchState(SWITCH_CHARGER_SAFETY);
	bool masterOkEffective = driverHWSwitchesGetSwitchState(SWITCH_MULTIPURPOSE_ENABLE);
	bool dischargePermissionEffective = driverHWSwitchesGetSwitchState(SWITCH_DISCHARGE_ENABLE);

	modCommandsPrintf("Output-permission diagnostics:");
	modCommandsPrintf("  MasterOk desired=%s effectiveGPIO=%s downstreamNote=active-low after MOSFET stage",
		terminalBoolToString(packState.masterOkDesired),
		terminalBoolToString(masterOkEffective));
	modCommandsPrintf("  DischargePermission desired=%s allowed=%s effectiveGPIO=%s",
		terminalBoolToString(packState.disChargeDesired),
		terminalBoolToString(packState.disChargeLCAllowed),
		terminalBoolToString(dischargePermissionEffective));
	modCommandsPrintf("  ChargePermission desired=%s allowed=%s effectiveGPIO=%s",
		terminalBoolToString(packState.chargeDesired),
		terminalBoolToString(packState.chargeAllowed),
		terminalBoolToString(chargePermissionEffective));
	modCommandsPrintf("  ChargerSafety desired=%s effectiveGPIO=%s",
		terminalBoolToString(packState.chargerSafetyDesired),
		terminalBoolToString(chargerSafetyEffective));
}

static void terminalPrintIsoSpiDiagnostics(void) {
	bool cellSelected = driverHWIsoSpiIsSelected(BMS_ISOSPI_CHAIN_CELL);
	bool tempSelected = driverHWIsoSpiIsSelected(BMS_ISOSPI_CHAIN_TEMP);
	bool cellCsHigh = (HAL_GPIO_ReadPin(CS_CELL_GPIO_Port, CS_CELL_Pin) == GPIO_PIN_SET);
	bool tempCsHigh = (HAL_GPIO_ReadPin(CS_TEMP_GPIO_Port, CS_TEMP_Pin) == GPIO_PIN_SET);

	modCommandsPrintf("isoSPI diagnostics:");
	if(cellSelected) {
		modCommandsPrintf("  selected chain: CELL");
	} else if(tempSelected) {
		modCommandsPrintf("  selected chain: TEMP");
	} else {
		modCommandsPrintf("  selected chain: NONE");
	}
	modCommandsPrintf("  CS_CELL idleHigh=%s raw=%s",
		terminalBoolToString(cellCsHigh),
		cellCsHigh ? "HIGH" : "LOW");
	modCommandsPrintf("  CS_TEMP idleHigh=%s raw=%s",
		terminalBoolToString(tempCsHigh),
		tempCsHigh ? "HIGH" : "LOW");
	modCommandsPrintf("  both chip-selects idle high=%s",
		terminalBoolToString(cellCsHigh && tempCsHigh));
	modCommandsPrintf("  TEMP chain note: S outputs are temporary sensor-bias enables, not balancing");
}

// Private types
typedef struct _terminal_callback_struct {
	const char *command;
	const char *help;
	const char *arg_names;
	void(*cbf)(int argc, const char **argv);
} terminal_callback_struct;

// Private variables
static terminal_callback_struct callbacks[CALLBACK_LEN];
static int callback_write = 0;

extern modConfigGeneralConfigStructTypedef *generalConfig;
extern modStateOfChargeStructTypeDef *generalStateOfCharge;
extern modPowerElectricsPackStateTypedef packState;
extern OperationalStateTypedef modOperationalStateCurrentState;

void terminal_process_string(char *str) {
	enum { kMaxArgs = 64 };
	int argc = 0;
	char *argv[kMaxArgs];

	char *p2 = strtok(str, " ");
	while (p2 && argc < kMaxArgs) {
		argv[argc++] = p2;
		p2 = strtok(0, " ");
	}

	if (argc == 0) {
		modCommandsPrintf("No command received\n");
		return;
	}

		if (strcmp(argv[0], "ping") == 0) {
			modCommandsPrintf("pong\n");
		} else if (strcmp(argv[0], "diag") == 0) {
			modCommandsPrintf("----- Bring-up diagnostics summary -----");
			modCommandsPrintf("Operational state       : %s", terminalOperationalStateToString(modOperationalStateCurrentState));
			terminalPrintMeasurementStatusSummary();
			modCommandsPrintf("Pack summary: Vbat=%.3fV I=%.3fA Vpack=%.3fV cell[min/avg/max]=%.4f/%.4f/%.4fV tempBatt[min/avg/max]=%.1f/%.1f/%.1fC",
				packState.vBatVoltage,
				packState.packCurrent,
				packState.vPackVoltage,
				packState.cellVoltageLow,
				packState.cellVoltageAverage,
				packState.cellVoltageHigh,
				packState.tempBatteryLow,
				packState.tempBatteryAverage,
				packState.tempBatteryHigh);
			modCommandsPrintf("Use diag_cells, diag_temp, diag_power, diag_outputs, diag_isospi for detailed views.");
			modCommandsPrintf("----- End bring-up diagnostics -----");
			modCommandsPrintf(" ");
		} else if (strcmp(argv[0], "diag_cells") == 0) {
			modCommandsPrintf("----- Cell-chain diagnostics -----");
			terminalPrintCellDiagnostics();
			modCommandsPrintf("----- End cell-chain diagnostics -----");
			modCommandsPrintf(" ");
		} else if (strcmp(argv[0], "diag_temp") == 0) {
			modCommandsPrintf("----- TEMP-chain diagnostics -----");
			terminalPrintTempDiagnostics();
			modCommandsPrintf("----- End TEMP-chain diagnostics -----");
			modCommandsPrintf(" ");
		} else if (strcmp(argv[0], "diag_power") == 0) {
			modCommandsPrintf("----- Power diagnostics -----");
			terminalPrintPowerDiagnostics();
			modCommandsPrintf("----- End power diagnostics -----");
			modCommandsPrintf(" ");
		} else if (strcmp(argv[0], "diag_outputs") == 0) {
			modCommandsPrintf("----- Output diagnostics -----");
			terminalPrintOutputDiagnostics();
			modCommandsPrintf("----- End output diagnostics -----");
			modCommandsPrintf(" ");
		} else if (strcmp(argv[0], "diag_isospi") == 0) {
			modCommandsPrintf("----- isoSPI diagnostics -----");
			terminalPrintIsoSpiDiagnostics();
			modCommandsPrintf("----- End isoSPI diagnostics -----");
			modCommandsPrintf(" ");
		} else if (strcmp(argv[0], "status") == 0) {
			bool disChargeEnabled = packState.disChargeDesired && packState.disChargeLCAllowed;
			bool chargeEnabled = packState.chargeDesired && packState.chargeAllowed;
		 
		modCommandsPrintf("-----Battery Pack Status-----");		
		modCommandsPrintf("Pack voltage          : %.2fV",packState.packVoltage);
		modCommandsPrintf("Pack current          : %.2fA",packState.packCurrent);
		modCommandsPrintf("Low  current          : %.2fA",packState.loCurrentLoadCurrent);
		modCommandsPrintf("High current          : %.2fA",packState.hiCurrentLoadCurrent);		
		modCommandsPrintf("State of charge       : %.1f%%",generalStateOfCharge->generalStateOfCharge);
		modCommandsPrintf("Remaining capacity    : %.2fAh",generalStateOfCharge->remainingCapacityAh);
		
			modCommandsPrintf("Operational state     : %s",terminalOperationalStateToString(modOperationalStateCurrentState));
		modCommandsPrintf("Load voltage          : %.2fV",packState.loCurrentLoadVoltage);
		modCommandsPrintf("Cell voltage high     : %.3fV",packState.cellVoltageHigh);
		modCommandsPrintf("Cell voltage low      : %.3fV",packState.cellVoltageLow);
		modCommandsPrintf("Cell voltage average  : %.3fV",packState.cellVoltageAverage);
		modCommandsPrintf("Cell voltage mismatch : %.3fV",packState.cellVoltageMisMatch);
		modCommandsPrintf("Discharge enabled     : %s",disChargeEnabled ? "True" : "False");
		modCommandsPrintf("Charge enabled        : %s",chargeEnabled ? "True" : "False");	
    modCommandsPrintf("Power button pressed  : %s",packState.powerButtonActuated ? "True" : "False");		
		modCommandsPrintf("---End Battery Pack Status---");
		modCommandsPrintf(" ");
		
	} else if (strcmp(argv[0], "sens") == 0) {		
		modCommandsPrintf("-----       Sensors         -----");
		
		// print temperatures
		modCommandsPrintf("Sensor[0]  : %.1f C - E - 'LTC NTC0'",packState.temperatures[0]);
		modCommandsPrintf("Sensor[1]  : %.1f C - E - 'LTC NTC1'",packState.temperatures[1]);
		modCommandsPrintf("Sensor[2]  : %.1f C - I - 'LTC Internal'",packState.temperatures[2]);
		modCommandsPrintf("Sensor[3]  : %.1f C - I - 'STM NTC'",packState.temperatures[3]);
		modCommandsPrintf("Sensor[4]  : %.1f C - E - 'ADC NTC0'",packState.temperatures[4]);
		modCommandsPrintf("Sensor[5]  : %.1f C - E - 'ADC NTC1'",packState.temperatures[5]);
		modCommandsPrintf("Sensor[6]  : %.1f C - E - 'ADC NTC2'",packState.temperatures[6]);
		modCommandsPrintf("Sensor[7]  : %.1f C - E - 'ADC NTC3'",packState.temperatures[7]);
		modCommandsPrintf("Sensor[8]  : %.1f C - E - 'ADC NTC4'",packState.temperatures[8]);
		modCommandsPrintf("Sensor[9]  : %.1f C - E - 'ADC NTC5'",packState.temperatures[9]);
		modCommandsPrintf("Sensor[10] : %.1f C - I - 'ADC NTC6'",packState.temperatures[10]);
		modCommandsPrintf("Sensor[11] : %.1f C - I - 'ADC NTC7'",packState.temperatures[11]);
		modCommandsPrintf("Sensor[12] : %.1f C - I - 'SHT'",packState.temperatures[12]);
		modCommandsPrintf("Sensor[13] : %.1f %% - I - 'Humidity'",packState.humidity);		
		modCommandsPrintf("----- E=External I=Internal -----");
		modCommandsPrintf("-----     End sensors       -----");
		modCommandsPrintf(" ");
		
	} else if (strcmp(argv[0], "cells") == 0) {
		uint8_t cellPointer = 0;
		
		modCommandsPrintf("-----   Cell voltages   -----");				
		for(cellPointer = 0 ; cellPointer < generalConfig->noOfCells ; cellPointer++) {
			modCommandsPrintf("Cell voltage%2d             : %.3fV",cellPointer,packState.cellVoltagesIndividual[cellPointer].cellVoltage);
		}
		modCommandsPrintf("Cell voltage high          : %.3fV",packState.cellVoltageHigh);
		modCommandsPrintf("Cell voltage low           : %.3fV",packState.cellVoltageLow);
		modCommandsPrintf("Cell voltage average       : %.3fV",packState.cellVoltageAverage);
		modCommandsPrintf("Cell voltage mismatch      : %.3fV",packState.cellVoltageMisMatch);
		modCommandsPrintf("----- End Cell voltages -----");	
		modCommandsPrintf(" ");
		
	} else if (strcmp(argv[0], "config") == 0) {
		modCommandsPrintf("---   BMS Configuration   ---");
		modCommandsPrintf("NoOfCells                  : %u",generalConfig->noOfCells);
		modCommandsPrintf("batteryCapacity            : %.2fAh",generalConfig->batteryCapacity);
		modCommandsPrintf("cellHardUnderVoltage       : %.3fV",generalConfig->cellHardUnderVoltage);
		modCommandsPrintf("cellHardOverVoltage        : %.3fV",generalConfig->cellHardOverVoltage);
		modCommandsPrintf("cellLCSoftUnderVoltage     : %.3fV",generalConfig->cellLCSoftUnderVoltage);
		modCommandsPrintf("cellSoftOverVoltage        : %.3fV",generalConfig->cellSoftOverVoltage);
		modCommandsPrintf("cellBalanceStart           : %.3fV",generalConfig->cellBalanceStart);
		modCommandsPrintf("cellBalanceDiffThreshold   : %.3fV",generalConfig->cellBalanceDifferenceThreshold);
		modCommandsPrintf("CAN ID                     : %u",generalConfig->CANID);
		modCommandsPrintf("--- End BMS Configuration ---");
		modCommandsPrintf(" ");
		
	} else if (strcmp(argv[0], "config_default") == 0) {
		modCommandsPrintf("--Restoring default config--");
		if(modConfigStoreDefaultConfig())
			modCommandsPrintf("Succesfully restored config, new config wil be used on powercycle (or use config_read to apply it now).");
		else
			modCommandsPrintf("Error restored config.");
		modCommandsPrintf(" ");
		
	} else if (strcmp(argv[0], "config_write") == 0) {
		modCommandsPrintf("---    Writing config    ---");
		if(modConfigStoreConfig())
			modCommandsPrintf("Succesfully written config.");
		else
			modCommandsPrintf("Error writing config.");
		modCommandsPrintf(" ");
		
	} else if (strcmp(argv[0], "config_read") == 0) {
		modCommandsPrintf("---    Reading config    ---");
		if(modConfigLoadConfig())
			modCommandsPrintf("Succesfully read config.");
		else
			modCommandsPrintf("Error reading config.");
		modCommandsPrintf(" ");
		
	} else if (strcmp(argv[0], "config_set_cells") == 0) {
		modCommandsPrintf("---Setting new cell count---");
		if (argc == 2) {
			uint32_t newNumberOfCells = 0;
			sscanf(argv[1], "%u", &newNumberOfCells);
			if(newNumberOfCells < 13 && newNumberOfCells > 2) {
				modCommandsPrintf("Number of cells is set to: %u.",newNumberOfCells);
				generalConfig->noOfCells = newNumberOfCells;
			} else {
				modCommandsPrintf("Invalid number of cells (should be anything from 3 to 12).");
			}
		} else {
			modCommandsPrintf("This command requires one argument.");
		}
		modCommandsPrintf(" ");

	} else if (strcmp(argv[0], "hwinfo") == 0) {
		modCommandsPrintf("-------    BMS Info   -------");		
		modCommandsPrintf("Firmware: %s", FW_REAL_VERSION);
		modCommandsPrintf("Name    : %s", HW_NAME);
		modCommandsPrintf("UUID: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
				STM32_UUID_8[0], STM32_UUID_8[1], STM32_UUID_8[2], STM32_UUID_8[3],
				STM32_UUID_8[4], STM32_UUID_8[5], STM32_UUID_8[6], STM32_UUID_8[7],
				STM32_UUID_8[8], STM32_UUID_8[9], STM32_UUID_8[10], STM32_UUID_8[11]);
		modCommandsPrintf("------- End BMS Info  -------");
		modCommandsPrintf(" ");
		
	} else if (strcmp(argv[0], "reboot") == 0) {
		modCommandsPrintf("------  Rebooting BMS  ------");
		NVIC_SystemReset();
		
	} else if (strcmp(argv[0], "bootloader_erase") == 0) {
		modCommandsPrintf("------  erasing new app space  ------");
		if(modFlashEraseNewAppData(0x00002000) == HAL_OK)
			modCommandsPrintf("--Erase done.");
		else
			modCommandsPrintf("--Erase error.");
		
	} else if (strcmp(argv[0], "bootloader_jump") == 0) {
		modFlashJumpToBootloader();
		
	} else if (strcmp(argv[0], "slave_scan") == 0) {
		uint8_t bitPointer;
		char    outputString[9];
		uint8_t presence = modHiAmpShieldScanI2CDevices();
		
		modCommandsPrintf("------  Slave BMS I2C scan  ------");
		
		for(bitPointer = 0; bitPointer < 8; bitPointer++){
		  if(presence & (1 << bitPointer))
				outputString[7-bitPointer] = '1';
			else
				outputString[7-bitPointer] = '0';
		}

		outputString[8] = 0;
		
		modCommandsPrintf("Presence: 0b%s",outputString);
		modCommandsPrintf("Bit order: 0(MSB) - 0 - FANDriver - NTCADC - IOExt - SHT - ISLAux - ISLMain(LSB). ");
		modCommandsPrintf("SHT does not respond when it is doing a conversion.");
		modCommandsPrintf("------  Slave BMS I2C scan end  ------");
		
	} else if (strcmp(argv[0], "help") == 0) {
		modCommandsPrintf("------- Start of help -------");
		modCommandsPrintf("Valid commands for the DieBieMS are:");
		modCommandsPrintf("help");
		modCommandsPrintf("  Show this help.");
		modCommandsPrintf("ping");
		modCommandsPrintf("  Print pong here to see if the reply works.");
		modCommandsPrintf("slave_scan");
		modCommandsPrintf("  Scan the I2C devices on the slave.");
			modCommandsPrintf("status");
			modCommandsPrintf("  Print battery measurements summary.");
			modCommandsPrintf("diag");
			modCommandsPrintf("  Print bring-up measurement validity and summary diagnostics.");
			modCommandsPrintf("diag_cells");
			modCommandsPrintf("  Print 75-cell chain diagnostics, PEC status and sample voltages.");
			modCommandsPrintf("diag_temp");
			modCommandsPrintf("  Print TEMP-chain raw/converted sample data and validity.");
			modCommandsPrintf("diag_power");
			modCommandsPrintf("  Print Vbat/current/Vpack validity and input diagnostics.");
			modCommandsPrintf("diag_outputs");
			modCommandsPrintf("  Print desired flags and GPIO readback for output permissions.");
			modCommandsPrintf("diag_isospi");
			modCommandsPrintf("  Print isoSPI chain-selection and chip-select idle diagnostics.");
			modCommandsPrintf("sens");
			modCommandsPrintf("  Print all sensor values.");
		modCommandsPrintf("cells");
		modCommandsPrintf("  Print cell voltage measurements.");
		modCommandsPrintf("config");
		modCommandsPrintf("  Print BMS configuration.");
		modCommandsPrintf("config_default");
		modCommandsPrintf("  Load default BMS configuration.");
		modCommandsPrintf("config_write");
		modCommandsPrintf("  Store current BMS configuration to EEPROM.");
		modCommandsPrintf("config_read");
		modCommandsPrintf("  Read BMS configuration from EEPROM.");
		modCommandsPrintf("hwinfo");
		modCommandsPrintf("  Print some hardware information.");
		modCommandsPrintf(" ");
		modCommandsPrintf("---More functionallity to come...--");

		for (int i = 0;i < callback_write;i++) {
			if (callbacks[i].arg_names) {
				modCommandsPrintf("%s %s", callbacks[i].command, callbacks[i].arg_names);
			} else {
				modCommandsPrintf(callbacks[i].command);
			}

			if (callbacks[i].help) {
				modCommandsPrintf("  %s", callbacks[i].help);
			} else {
				modCommandsPrintf("  There is no help available for this command.");
			}
		}

		modCommandsPrintf(" ");
	} else {
		bool found = false;
		for (int i = 0;i < callback_write;i++) {
			if (strcmp(argv[0], callbacks[i].command) == 0) {
				callbacks[i].cbf(argc, (const char**)argv);
				found = true;
				break;
			}
		}

		if (!found) {
			modCommandsPrintf("Invalid command: %s\n type help to list all available commands\n", argv[0]);
		}
	}
}

/**
 * Register a custom command  callback to the terminal. If the command
 * is already registered the old command callback will be replaced.
 *
 * @param command
 * The command name.
 *
 * @param help
 * A help text for the command. Can be NULL.
 *
 * @param arg_names
 * The argument names for the command, e.g. [arg_a] [arg_b]
 * Can be NULL.
 *
 * @param cbf
 * The callback function for the command.
 */
void terminal_register_command_callback(
		const char* command,
		const char *help,
		const char *arg_names,
		void(*cbf)(int argc, const char **argv)) {

	int callback_num = callback_write;

	for (int i = 0;i < callback_write;i++) {
		// First check the address in case the same callback is registered more than once.
		if (callbacks[i].command == command) {
			callback_num = i;
			break;
		}

		// Check by string comparison.
		if (strcmp(callbacks[i].command, command) == 0) {
			callback_num = i;
			break;
		}
	}

	callbacks[callback_num].command = command;
	callbacks[callback_num].help = help;
	callbacks[callback_num].arg_names = arg_names;
	callbacks[callback_num].cbf = cbf;

	if (callback_num == callback_write) {
		callback_write++;
		if (callback_write >= CALLBACK_LEN) {
			callback_write = 0;
		}
	}
}
		

/*
	defaultConfig.cellBalanceDifferenceThreshold						=	0.01f;												// Start balancing @ 5mV difference, stop if below
*/
