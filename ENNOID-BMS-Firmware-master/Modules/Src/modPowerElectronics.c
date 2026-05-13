#include "modPowerElectronics.h"

modPowerElectricsPackStateTypedef *modPowerElectronicsPackStateHandle;
modConfigGeneralConfigStructTypedef *modPowerElectronicsGeneralConfigHandle;
uint32_t modPowerElectronicsMeasureIntervalLastTick;

uint32_t modPowerElectronicsChargeRetryLastTick;
uint32_t modPowerElectronicsDisChargeLCRetryLastTick;
uint32_t modPowerElectronicsDisChargeHCRetryLastTick;
uint32_t modPowerElectronicsCellBalanceUpdateLastTick;
uint32_t modPowerElectronicsTempMeasureDelayLastTick;
uint32_t modPowerElectronicsChargeCurrentDetectionLastTick;
uint32_t modPowerElectronicsBalanceModeActiveLastTick;
uint8_t  modPowerElectronicsUnderAndOverVoltageErrorCount;
driverLTC6803ConfigStructTypedef modPowerElectronicsLTCconfigStruct;
bool     modPowerElectronicsAllowForcedOnState;
uint16_t modPowerElectronicsTemperatureArray[3];
uint16_t tempTemperature;
uint8_t  modPowerElectronicsISLErrorCount;

static uint8_t modPowerElectronicsGetActiveCellCount(void) {
	if(modPowerElectronicsPackStateHandle->cellVoltageReadoutValid)
		return modPowerElectronicsPackStateHandle->cellVoltageReadoutCount;

	return modPowerElectronicsGeneralConfigHandle->noOfCells;
}

static void modPowerElectronicsMirrorLegacyCellVoltages(void) {
	for(uint8_t cellPointer = 0u; cellPointer < NoOfCellsPossibleOnChip; cellPointer++) {
		modPowerElectronicsPackStateHandle->cellVoltagesIndividual[cellPointer].cellVoltage = modPowerElectronicsPackStateHandle->cellVoltagesLTC6812[cellPointer].cellVoltage;
		modPowerElectronicsPackStateHandle->cellVoltagesIndividual[cellPointer].cellNumber = modPowerElectronicsPackStateHandle->cellVoltagesLTC6812[cellPointer].cellNumber;
	}
}

static void modPowerElectronicsMarkTemperatureReadoutUnavailable(void) {
	for(uint8_t sensorPointer = 0u; sensorPointer < NoOfTempSensors; sensorPointer++) {
		modPowerElectronicsPackStateHandle->temperatures[sensorPointer] = 200.0f;
	}

	for(uint8_t sensorPointer = 0u; sensorPointer < BMS_TOTAL_TEMPS; sensorPointer++) {
		modPowerElectronicsPackStateHandle->temperaturesLTC6812[sensorPointer] = 200.0f;
		modPowerElectronicsPackStateHandle->temperaturesLTC6812Valid[sensorPointer] = false;
	}

	/* Temperature coverage must remain conservative until the TEMP-chain readout
	 * and final sensor conversion are both valid.
	 * TODO(phase4): replace the placeholder conversion with the Enepaq transfer curve.
	 */
	modPowerElectronicsPackStateHandle->temperatureReadoutValid = false;
}

static bool modPowerElectronicsConvertEnepaqTemperatureVoltage(driverLTC6812AnalogVoltageTypedef *sensorVoltage, float *temperatureC) {
	if((sensorVoltage->milliVolts < 100u) || (sensorVoltage->milliVolts > 4900u)) {
		*temperatureC = 200.0f;
		sensorVoltage->valid = false;
		return false;
	}

	/* TODO(phase4): replace this placeholder with the actual Enepaq voltage-to-temperature curve.
	 * We keep the converted channel invalid so missing curve information cannot look safe.
	 */
	*temperatureC = 200.0f;
	sensorVoltage->valid = false;
	return false;
}

static void modPowerElectronicsUpdateTemperatureChainReadout(bool tempReadValid) {
	uint8_t convertedTemperatureCount = 0u;

	modPowerElectronicsMarkTemperatureReadoutUnavailable();
	modPowerElectronicsPackStateHandle->temperatureReadoutCount = BMS_TOTAL_TEMPS;

	if(!tempReadValid)
		return;

	for(uint8_t tempIndex = 0u; tempIndex < BMS_TOTAL_TEMPS; tempIndex++) {
		float convertedTemperature = 200.0f;
		bool convertedValid = modPowerElectronicsConvertEnepaqTemperatureVoltage(
			&modPowerElectronicsPackStateHandle->tempSensorVoltagesLTC6812[tempIndex],
			&convertedTemperature);

		modPowerElectronicsPackStateHandle->temperaturesLTC6812[tempIndex] = convertedTemperature;
		modPowerElectronicsPackStateHandle->temperaturesLTC6812Valid[tempIndex] = convertedValid;
		if(convertedValid)
			convertedTemperatureCount++;
	}

	modPowerElectronicsPackStateHandle->temperatureReadoutValid = (convertedTemperatureCount == BMS_TOTAL_TEMPS);
}

void modPowerElectronicsInit(modPowerElectricsPackStateTypedef *packState, modConfigGeneralConfigStructTypedef *generalConfigPointer) {
	modPowerElectronicsGeneralConfigHandle = generalConfigPointer;
	modPowerElectronicsPackStateHandle = packState;
	modPowerElectronicsUnderAndOverVoltageErrorCount = 0;
	modPowerElectronicsAllowForcedOnState = false;
	modPowerElectronicsISLErrorCount = 0;
	
	// Init pack status
	modPowerElectronicsPackStateHandle->throttleDutyCharge       = 0;
	modPowerElectronicsPackStateHandle->throttleDutyDischarge    = 0;
	modPowerElectronicsPackStateHandle->SoC                      = 0.0f;
	modPowerElectronicsPackStateHandle->SoCCapacityAh            = 0.0f;
	modPowerElectronicsPackStateHandle->operationalState         = OP_STATE_INIT;
	modPowerElectronicsPackStateHandle->packVoltage              = 0.0f;
	modPowerElectronicsPackStateHandle->vBatVoltage              = 0.0f;
	modPowerElectronicsPackStateHandle->packCurrent              = 0.0f;
	modPowerElectronicsPackStateHandle->packPower                = 0.0f;
	modPowerElectronicsPackStateHandle->loCurrentLoadCurrent     = 0.0f;
	modPowerElectronicsPackStateHandle->loCurrentLoadVoltage     = 0.0f;
	modPowerElectronicsPackStateHandle->vPackVoltage             = 0.0f;
	modPowerElectronicsPackStateHandle->cellVoltageHigh          = 0.0f;
	modPowerElectronicsPackStateHandle->cellVoltageLow           = 0.0f;
	modPowerElectronicsPackStateHandle->cellVoltageAverage       = 0.0;
	modPowerElectronicsPackStateHandle->masterOkDesired          = false;
	modPowerElectronicsPackStateHandle->disChargeDesired         = false;
	modPowerElectronicsPackStateHandle->disChargeLCAllowed       = true;
	modPowerElectronicsPackStateHandle->disChargeHCAllowed       = true;
	modPowerElectronicsPackStateHandle->preChargeDesired         = false;
	modPowerElectronicsPackStateHandle->chargeDesired            = false;
	modPowerElectronicsPackStateHandle->chargeAllowed 					 = true;
	modPowerElectronicsPackStateHandle->safetyOverCANHCSafeNSafe = false;
	modPowerElectronicsPackStateHandle->chargerSafetyDesired     = false;
	modPowerElectronicsPackStateHandle->chargeBalanceActive      = false;
	modPowerElectronicsPackStateHandle->chargeCurrentDetected    = false;
	modPowerElectronicsPackStateHandle->powerButtonActuated      = false;
	modPowerElectronicsPackStateHandle->packInSOA                = true;
	modPowerElectronicsPackStateHandle->watchDogTime             = 255;
	modPowerElectronicsPackStateHandle->cellVoltageReadoutValid  = false;
	modPowerElectronicsPackStateHandle->cellVoltageReadoutErrorCount = 0;
	modPowerElectronicsPackStateHandle->cellVoltageReadoutCount  = BMS_TOTAL_CELLS;
	modPowerElectronicsPackStateHandle->temperatureReadoutValid  = false;
	modPowerElectronicsPackStateHandle->temperatureReadoutErrorCount = 0;
	modPowerElectronicsPackStateHandle->temperatureReadoutCount = BMS_TOTAL_TEMPS;
	modPowerElectronicsPackStateHandle->vBatReadoutValid         = false;
	modPowerElectronicsPackStateHandle->currentReadoutValid      = false;
	modPowerElectronicsPackStateHandle->vPackReadoutValid        = false;
	modPowerElectronicsPackStateHandle->powerMonitorReadoutValid = false;
	modPowerElectronicsPackStateHandle->powerMonitorReadoutErrorCount = 0;
	modPowerElectronicsPackStateHandle->packOperationalCellState = PACK_STATE_NORMAL;
	modPowerElectronicsPackStateHandle->temperatures[0]          = 200.0f;
	modPowerElectronicsPackStateHandle->temperatures[1]          = 200.0f;
	modPowerElectronicsPackStateHandle->temperatures[2]          = 200.0f;
	modPowerElectronicsPackStateHandle->temperatures[3]          = 200.0f;
	modPowerElectronicsPackStateHandle->tempBatteryHigh          = 0.0f;
	modPowerElectronicsPackStateHandle->tempBatteryLow           = 0.0f;
	modPowerElectronicsPackStateHandle->tempBatteryAverage       = 0.0f;
	modPowerElectronicsPackStateHandle->tempBMSHigh              = 0.0f;
	modPowerElectronicsPackStateHandle->tempBMSLow               = 0.0f;
	modPowerElectronicsPackStateHandle->tempBMSAverage           = 0.0f;
	
  modPowerElectronicsInitISL();
	
	// Init internal ADC
	driverHWADCInit();
	driverHWSwitchesInit();

	/* Phase 3 migrates cell readout to LTC6812 on the CELL chain only.
	 * Legacy LTC6803 configuration/balancing commands are intentionally not sent here.
	 * TODO(phase4): migrate temp-chain and balancing/fault logic off the LTC6803 assumptions.
	 */
	driverSWLTC6812Init();
	(void)driverSWLTC6812StartCellVoltageConversion();
	(void)driverSWLTC6812StartTemperatureVoltageConversion();
	
	modPowerElectronicsChargeCurrentDetectionLastTick = HAL_GetTick();
	modPowerElectronicsBalanceModeActiveLastTick = HAL_GetTick();
};

bool modPowerElectronicsTask(void) {
	bool returnValue = false;
	
	if(modDelayTick1ms(&modPowerElectronicsMeasureIntervalLastTick,100)) {
		bool cellReadValid;
		bool tempReadValid;
		bool vBatReadValid;
		bool currentReadValid;
		bool vPackReadValid;
		driverLTC6812StatusTypedef cellChainStatus;
		driverLTC6812StatusTypedef tempChainStatus;
		float measuredVBat = 0.0f;
		float measuredPackCurrent = 0.0f;
		float measuredVPack = 0.0f;

		// reset tick for LTC Temp start conversion delay
		modPowerElectronicsTempMeasureDelayLastTick = HAL_GetTick();
		
		// Collect battery-side Vbat/current from the ISL28022 on I2C2 and the
		// load-side / precharge-bus Vpack from the PA1 ADC. Never treat failed reads
		// as valid or silently keep stale values active.
		/* TODO(phase7): verify the final Vbat register-to-voltage scalar against the
		 * assembled ISL28022 input divider and production calibration.
		 */
		vBatReadValid = driverSWISL28022GetBusVoltage(ISL28022_MASTER_ADDRES,ISL28022_MASTER_BUS,&measuredVBat,0.004f);
		currentReadValid = driverSWISL28022GetBusCurrent(ISL28022_MASTER_ADDRES,ISL28022_MASTER_BUS,&measuredPackCurrent,modPowerElectronicsGeneralConfigHandle->shuntLCOffset,modPowerElectronicsGeneralConfigHandle->shuntLCFactor);
		vPackReadValid = driverHWADCGetVPackVoltage(&measuredVPack);

		if(vBatReadValid &&
		   modPowerElectronicsPackStateHandle->cellVoltageAverage > 0.0f &&
		   fabs(measuredVBat - modPowerElectronicsGeneralConfigHandle->noOfCells*modPowerElectronicsPackStateHandle->cellVoltageAverage) >= 1.0f) {
			/* TODO(phase7): replace this coarse Vbat plausibility check with calibrated
			 * pack-level validation once the final analogue scaling is verified.
			 */
			vBatReadValid = false;
		}

		modPowerElectronicsPackStateHandle->vBatReadoutValid = vBatReadValid;
		modPowerElectronicsPackStateHandle->currentReadoutValid = currentReadValid;
		modPowerElectronicsPackStateHandle->vPackReadoutValid = vPackReadValid;
		modPowerElectronicsPackStateHandle->powerMonitorReadoutValid = vBatReadValid && currentReadValid && vPackReadValid;

		if(vBatReadValid) {
			modPowerElectronicsPackStateHandle->vBatVoltage = measuredVBat;
			modPowerElectronicsPackStateHandle->packVoltage = measuredVBat;
		}else{
			modPowerElectronicsPackStateHandle->vBatVoltage = 0.0f;
			modPowerElectronicsPackStateHandle->packVoltage = 0.0f;
		}

		if(currentReadValid) {
			modPowerElectronicsPackStateHandle->loCurrentLoadCurrent = measuredPackCurrent;
		}else{
			modPowerElectronicsPackStateHandle->loCurrentLoadCurrent = 0.0f;
		}

		if(vPackReadValid) {
			modPowerElectronicsPackStateHandle->vPackVoltage = measuredVPack;
			modPowerElectronicsPackStateHandle->loCurrentLoadVoltage = measuredVPack;
		}else{
			modPowerElectronicsPackStateHandle->vPackVoltage = 0.0f;
			modPowerElectronicsPackStateHandle->loCurrentLoadVoltage = 0.0f;
		}

		if(modPowerElectronicsPackStateHandle->powerMonitorReadoutValid) {
			modPowerElectronicsISLErrorCount = 0;																								// Reset error count.
		}else{
			if(modPowerElectronicsISLErrorCount++ >= ISLErrorThreshold){												// Increase error count
				modPowerElectronicsISLErrorCount = ISLErrorThreshold;
				// Make BMS signal error state and power down.
			}else{
				modPowerElectronicsInitISL();																											// Reinit I2C and ISL	
			}
		}
		modPowerElectronicsPackStateHandle->powerMonitorReadoutErrorCount = modPowerElectronicsISLErrorCount;
		
		// Combine the two currents and calculate pack power.
		modPowerElectronicsPackStateHandle->packCurrent = modPowerElectronicsPackStateHandle->loCurrentLoadCurrent + modPowerElectronicsPackStateHandle->hiCurrentLoadCurrent;
		modPowerElectronicsPackStateHandle->packPower   = modPowerElectronicsPackStateHandle->packCurrent * modPowerElectronicsPackStateHandle->packVoltage;

		cellReadValid = driverSWLTC6812ReadCellVoltages(modPowerElectronicsPackStateHandle->cellVoltagesLTC6812);
		cellChainStatus = driverSWLTC6812GetCellChainStatus();
		modPowerElectronicsPackStateHandle->cellVoltageReadoutValid = cellReadValid;
		modPowerElectronicsPackStateHandle->cellVoltageReadoutErrorCount = cellChainStatus.lastReadPECErrors;
		if(cellReadValid)
			modPowerElectronicsMirrorLegacyCellVoltages();

		tempReadValid = driverSWLTC6812ReadTemperatureVoltages(modPowerElectronicsPackStateHandle->tempSensorVoltagesLTC6812);
		tempChainStatus = driverSWLTC6812GetTemperatureChainStatus();
		modPowerElectronicsPackStateHandle->temperatureReadoutErrorCount = tempChainStatus.lastReadPECErrors;
		/* TODO(phase5): define the final shutdown/derate action for TEMP-chain comms faults.
		 * Phase 4 records validity/errors and keeps missing temperature coverage conservative.
		 */
		modPowerElectronicsUpdateTemperatureChainReadout(tempReadValid);

		/* The local STM32 NTC remains a board-local temperature only.
		 * Pack temperature coverage comes from the read-only TEMP chain and must not
		 * be treated as valid until the Enepaq conversion curve is implemented.
		 */
		driverHWADCGetNTCValue(&modPowerElectronicsPackStateHandle->temperatures[3],modPowerElectronicsGeneralConfigHandle->NTC25DegResistance[modConfigNTCGroupMasterPCB],modPowerElectronicsGeneralConfigHandle->NTCTopResistor[modConfigNTCGroupMasterPCB],modPowerElectronicsGeneralConfigHandle->NTCBetaFactor[modConfigNTCGroupMasterPCB],25.0f);
		
		// Calculate temperature statisticks
		modPowerElectronicsCalcTempStats();
		
		// When temperature and cellvoltages are known calculate charge and discharge throttle.
		modPowerElectronicsCalcThrottle();
		
		/* Phase 3 keeps balancing disabled until the LTC6812 cell-chain migration is complete.
		 * TODO(phase5): rework balancing to target the LTC6812 cell chain only.
		 */
		
		// Start the next LTC6812 cell conversion on the CELL chain.
		if(!driverSWLTC6812StartCellVoltageConversion())
			modPowerElectronicsPackStateHandle->cellVoltageReadoutValid = false;

		// Start the next read-only TEMP-chain conversion.
		if(!driverSWLTC6812StartTemperatureVoltageConversion())
			modPowerElectronicsPackStateHandle->temperatureReadoutValid = false;
		
		// Check and respond to the measured voltage values
		modPowerElectronicsSubTaskVoltageWatch();
		
		// Check and respond to the measured temperature values
		// modPowerElectronicsSubTaskTemperatureWatch();
		
		// Check and determine whether or not there is a charge current and we need to balance.
		if(modPowerElectronicsPackStateHandle->packCurrent >= modPowerElectronicsGeneralConfigHandle->chargerEnabledThreshold) {
			if(modDelayTick1ms(&modPowerElectronicsChargeCurrentDetectionLastTick,5000)) {
				modPowerElectronicsPackStateHandle->chargeBalanceActive = modPowerElectronicsGeneralConfigHandle->allowChargingDuringDischarge;
				modPowerElectronicsPackStateHandle->chargeCurrentDetected = true;																																								// Charge current is detected after 2 seconds
			}
			
			if(modPowerElectronicsPackStateHandle->chargeCurrentDetected) {
				modPowerElectronicsResetBalanceModeActiveTimeout();
			}
		}else{
			modPowerElectronicsPackStateHandle->chargeCurrentDetected = false;
			modPowerElectronicsChargeCurrentDetectionLastTick = HAL_GetTick();
		}
		// TODO: have balance time configureable
		if(modDelayTick1ms(&modPowerElectronicsBalanceModeActiveLastTick,10*60*1000)) {																																			// When a charge current is derected, balance for 10 minutes
			modPowerElectronicsPackStateHandle->chargeBalanceActive = false;
		}
		
		modPowerElectronicsPackStateHandle->powerButtonActuated = modPowerStateGetButtonPressedState();
		
		returnValue = cellReadValid;
	}else
		returnValue = false;
	
	return returnValue;
};

void modPowerElectronicsAllowForcedOn(bool allowedState){
	modPowerElectronicsAllowForcedOnState = allowedState;
}

void modPowerElectronicsSetMasterOk(bool allowed) {
	/* PB11 no longer controls a precharge relay. This bool represents the firmware's
	 * desire to assert the master/BMS-healthy permission into the shutdown circuit.
	 */
	modPowerElectronicsPackStateHandle->masterOkDesired = allowed;
	modPowerElectronicsPackStateHandle->preChargeDesired = allowed;
	modPowerElectronicsUpdateSwitches();
}

void modPowerElectronicsSetDischargePermission(bool allowed) {
	modPowerElectronicsPackStateHandle->disChargeDesired = allowed;
	modPowerElectronicsUpdateSwitches();
}

void modPowerElectronicsSetChargePermission(bool allowed) {
	modPowerElectronicsPackStateHandle->chargeDesired = allowed;
	modPowerElectronicsUpdateSwitches();
}

void modPowerElectronicsSetChargerSafety(bool allowed) {
	modPowerElectronicsPackStateHandle->chargerSafetyDesired = allowed;
	modPowerElectronicsUpdateSwitches();
}

bool modPowerElectronicsCanCloseDischargePath(void) {
	if(!modPowerElectronicsPackStateHandle->vBatReadoutValid ||
	   !modPowerElectronicsPackStateHandle->vPackReadoutValid)
		return false;

	return modPowerElectronicsPackStateHandle->loCurrentLoadVoltage >=
	       (PRECHARGE_PERCENTAGE * modPowerElectronicsPackStateHandle->packVoltage);
}

void modPowerElectronicsSetPreCharge(bool newState) {
	/* TODO(migration): remove legacy wrapper after operational-state call sites stop
	 * referring to precharge. PB11 is now the master_ok / multipurpose permission.
	 * This wrapper is deprecated for new code.
	 */
	modPowerElectronicsSetMasterOk(newState);
};

bool modPowerElectronicsSetDisCharge(bool newState) {
	/* TODO(migration): remove legacy wrapper after operational-state call sites are
	 * updated. PB10 is now a discharge permission into shutdown logic.
	 * Guard the old bool-return behavior before changing any output intent.
	 * This wrapper is deprecated for new code.
	 */
	if(!newState) {
		modPowerElectronicsSetDischargePermission(false);
		return true;
	}

	if(!modPowerElectronicsCanCloseDischargePath())
		return false;

	modPowerElectronicsSetDischargePermission(true);
	return true;
};

void modPowerElectronicsSetCharge(bool newState) {
	/* TODO(migration): remove legacy wrapper after call sites are updated to the
	 * explicit charge-permission API. This wrapper is deprecated for new code.
	 */
	modPowerElectronicsSetChargePermission(newState);
};

void modPowerElectronicsDisableAll(void) {
	if(modPowerElectronicsPackStateHandle->masterOkDesired |
	   modPowerElectronicsPackStateHandle->disChargeDesired |
	   modPowerElectronicsPackStateHandle->preChargeDesired |
	   modPowerElectronicsPackStateHandle->chargeDesired) {
		modPowerElectronicsPackStateHandle->masterOkDesired = false;
		modPowerElectronicsPackStateHandle->disChargeDesired = false;
		modPowerElectronicsPackStateHandle->preChargeDesired = false;
		modPowerElectronicsPackStateHandle->chargeDesired = false;
	}
	modPowerElectronicsPackStateHandle->chargerSafetyDesired = false;
	driverHWSwitchesDisableAll();
};

void modPowerElectronicsCalculateCellStats(void) {
	float cellVoltagesSummed = 0.0f;
	uint8_t activeCellCount = modPowerElectronicsGetActiveCellCount();

	if(!modPowerElectronicsPackStateHandle->cellVoltageReadoutValid) {
		modPowerElectronicsPackStateHandle->cellVoltageHigh = 0.0f;
		modPowerElectronicsPackStateHandle->cellVoltageLow = 0.0f;
		modPowerElectronicsPackStateHandle->cellVoltageAverage = 0.0f;
		modPowerElectronicsPackStateHandle->cellVoltageMisMatch = 0.0f;
		return;
	}

	modPowerElectronicsPackStateHandle->cellVoltageHigh = 0.0f;
	modPowerElectronicsPackStateHandle->cellVoltageLow = 10.0f;
	
	for(uint8_t cellPointer = 0u; cellPointer < activeCellCount; cellPointer++) {
		float cellVoltage = modPowerElectronicsPackStateHandle->cellVoltagesLTC6812[cellPointer].cellVoltage;

		cellVoltagesSummed += cellVoltage;
		
		if(cellVoltage > modPowerElectronicsPackStateHandle->cellVoltageHigh)
			modPowerElectronicsPackStateHandle->cellVoltageHigh = cellVoltage;
		
		if(cellVoltage < modPowerElectronicsPackStateHandle->cellVoltageLow)
			modPowerElectronicsPackStateHandle->cellVoltageLow = cellVoltage;		
	}
	
	modPowerElectronicsPackStateHandle->cellVoltageAverage = cellVoltagesSummed/(float)activeCellCount;
	modPowerElectronicsPackStateHandle->cellVoltageMisMatch = modPowerElectronicsPackStateHandle->cellVoltageHigh - modPowerElectronicsPackStateHandle->cellVoltageLow;
};

void modPowerElectronicsSubTaskBalaning(void) {
	static uint32_t delayTimeHolder = 100;
	static uint16_t lastCellBalanceRegister = 0;
	static bool delaytoggle = false;
	uint16_t cellBalanceMaskEnableRegister = 0;
	driverLTC6803CellsTypedef sortedCellArray[modPowerElectronicsGeneralConfigHandle->noOfCells];
	
	if(modDelayTick1ms(&modPowerElectronicsCellBalanceUpdateLastTick,delayTimeHolder)) {
		delaytoggle ^= true;
		delayTimeHolder = delaytoggle ? modPowerElectronicsGeneralConfigHandle->cellBalanceUpdateInterval : 200;
		
		if(delaytoggle) {
			for(int k=0; k<modPowerElectronicsGeneralConfigHandle->noOfCells; k++) {
				sortedCellArray[k] = modPowerElectronicsPackStateHandle->cellVoltagesIndividual[k];	// This will contain the voltages that are unloaded by balance resistors
			}
				
			modPowerElectronicsSortCells(sortedCellArray,modPowerElectronicsGeneralConfigHandle->noOfCells);
			
			if((modPowerElectronicsPackStateHandle->chargeDesired && !modPowerElectronicsPackStateHandle->disChargeDesired) || modPowerElectronicsPackStateHandle->chargeBalanceActive || !modPowerElectronicsPackStateHandle->chargeAllowed) {																							// Check if charging is desired
				for(uint8_t i = 0; i < modPowerElectronicsGeneralConfigHandle->maxSimultaneousDischargingCells; i++) {
					if(sortedCellArray[i].cellVoltage >= (modPowerElectronicsPackStateHandle->cellVoltageLow + modPowerElectronicsGeneralConfigHandle->cellBalanceDifferenceThreshold)) {
						if(sortedCellArray[i].cellVoltage >= modPowerElectronicsGeneralConfigHandle->cellBalanceStart) {
							cellBalanceMaskEnableRegister |= (1 << sortedCellArray[i].cellNumber);
						}
					};
				}
			}
		}
		
		modPowerElectronicsPackStateHandle->cellBalanceResistorEnableMask = cellBalanceMaskEnableRegister;
		
		if(lastCellBalanceRegister != cellBalanceMaskEnableRegister)
			driverSWLTC6803EnableBalanceResistors(cellBalanceMaskEnableRegister);
		lastCellBalanceRegister = cellBalanceMaskEnableRegister;
	}
};

void modPowerElectronicsSubTaskVoltageWatch(void) {
	static bool lastdisChargeLCAllowed = false;
	static bool lastChargeAllowed = false;
	uint16_t hardUnderVoltageFlags = 0u;
	uint16_t hardOverVoltageFlags = 0u;
	uint8_t activeCellCount = modPowerElectronicsGetActiveCellCount();

	/* Phase 3 repair: until LTC6812 hardware UV/OV flag and open-wire reads are migrated,
	 * derive hard voltage faults conservatively from the validated cell measurements.
	 * TODO(phase5): migrate to LTC6812 register-flag and open-wire diagnostics.
	 */
	modPowerElectronicsCalculateCellStats();

	if(modPowerElectronicsPackStateHandle->cellVoltageReadoutValid) {
		for(uint8_t cellPointer = 0u; cellPointer < activeCellCount; cellPointer++) {
			float cellVoltage = modPowerElectronicsPackStateHandle->cellVoltagesLTC6812[cellPointer].cellVoltage;

			if(cellVoltage <= modPowerElectronicsGeneralConfigHandle->cellHardUnderVoltage)
				hardUnderVoltageFlags = 1u;

			if(cellVoltage >= modPowerElectronicsGeneralConfigHandle->cellHardOverVoltage)
				hardOverVoltageFlags = 1u;
		}
	}
	
	if(modPowerElectronicsPackStateHandle->packOperationalCellState != PACK_STATE_ERROR_HARD_CELLVOLTAGE) {
		// Handle soft cell voltage limits
		// Low current
		if(modPowerElectronicsPackStateHandle->cellVoltageLow <= modPowerElectronicsGeneralConfigHandle->cellLCSoftUnderVoltage) {
			modPowerElectronicsPackStateHandle->disChargeLCAllowed = false;
			modPowerElectronicsDisChargeLCRetryLastTick = HAL_GetTick();
		}
		
		// High current
		if(modPowerElectronicsPackStateHandle->cellVoltageLow <= modPowerElectronicsGeneralConfigHandle->cellHCSoftUnderVoltage) {
			modPowerElectronicsPackStateHandle->disChargeHCAllowed = false;
			modPowerElectronicsDisChargeHCRetryLastTick = HAL_GetTick();
		}
		
		if(modPowerElectronicsPackStateHandle->cellVoltageHigh >= modPowerElectronicsGeneralConfigHandle->cellSoftOverVoltage) {
			modPowerElectronicsPackStateHandle->chargeAllowed = false;
			modPowerElectronicsChargeRetryLastTick = HAL_GetTick();
		}
		
		// Low current
		if(modPowerElectronicsPackStateHandle->cellVoltageLow >= (modPowerElectronicsGeneralConfigHandle->cellLCSoftUnderVoltage + modPowerElectronicsGeneralConfigHandle->hysteresisDischarge)) {
			if(modDelayTick1ms(&modPowerElectronicsDisChargeLCRetryLastTick,modPowerElectronicsGeneralConfigHandle->timeoutDischargeRetry))
				modPowerElectronicsPackStateHandle->disChargeLCAllowed = true;
		}
		
		// High current
		if(modPowerElectronicsPackStateHandle->cellVoltageLow >= (modPowerElectronicsGeneralConfigHandle->cellHCSoftUnderVoltage + modPowerElectronicsGeneralConfigHandle->hysteresisDischarge)) {
			if(modDelayTick1ms(&modPowerElectronicsDisChargeHCRetryLastTick,modPowerElectronicsGeneralConfigHandle->timeoutDischargeRetry))
				modPowerElectronicsPackStateHandle->disChargeHCAllowed = true;
		}		
		
		if(modPowerElectronicsPackStateHandle->cellVoltageHigh <= (modPowerElectronicsGeneralConfigHandle->cellSoftOverVoltage - modPowerElectronicsGeneralConfigHandle->hysteresisCharge)) {
			if(modDelayTick1ms(&modPowerElectronicsChargeRetryLastTick,modPowerElectronicsGeneralConfigHandle->timeoutChargeRetry))
				modPowerElectronicsPackStateHandle->chargeAllowed = true;
		}
		
		if(modPowerElectronicsPackStateHandle->chargeAllowed && modPowerElectronicsPackStateHandle->disChargeLCAllowed)
			modPowerElectronicsPackStateHandle->packOperationalCellState = PACK_STATE_NORMAL;
		else
			modPowerElectronicsPackStateHandle->packOperationalCellState = PACK_STATE_ERROR_SOFT_CELLVOLTAGE;
	}
	
	// Handle hard cell voltage limits
	if(!modPowerElectronicsPackStateHandle->cellVoltageReadoutValid ||
	   hardUnderVoltageFlags ||
	   hardOverVoltageFlags ||
	   (modPowerElectronicsPackStateHandle->packVoltage > activeCellCount*modPowerElectronicsGeneralConfigHandle->cellHardOverVoltage)) {
		if(modPowerElectronicsUnderAndOverVoltageErrorCount++ > modPowerElectronicsGeneralConfigHandle->maxUnderAndOverVoltageErrorCount)
			modPowerElectronicsPackStateHandle->packOperationalCellState = PACK_STATE_ERROR_HARD_CELLVOLTAGE;
		modPowerElectronicsPackStateHandle->disChargeLCAllowed = false;
		modPowerElectronicsPackStateHandle->chargeAllowed = false;
	}else
		modPowerElectronicsUnderAndOverVoltageErrorCount = 0;
	
	
	// update outputs directly if needed
	if((lastChargeAllowed != modPowerElectronicsPackStateHandle->chargeAllowed) || (lastdisChargeLCAllowed != modPowerElectronicsPackStateHandle->disChargeLCAllowed)) {
		lastChargeAllowed = modPowerElectronicsPackStateHandle->chargeAllowed;
		lastdisChargeLCAllowed = modPowerElectronicsPackStateHandle->disChargeLCAllowed;
		modPowerElectronicsUpdateSwitches();
	}
};

// Update switch states, should be called after every desired/allowed switch state change
void modPowerElectronicsUpdateSwitches(void) {
	bool temperatureCoverageRequired = (modPowerElectronicsGeneralConfigHandle->tempEnableMaskBattery ||
	                                    modPowerElectronicsGeneralConfigHandle->tempEnableMaskBMS);
	bool dataHealthy = modPowerElectronicsPackStateHandle->cellVoltageReadoutValid &&
	                   (!temperatureCoverageRequired || modPowerElectronicsPackStateHandle->temperatureReadoutValid) &&
	                   (modPowerElectronicsPackStateHandle->packOperationalCellState != PACK_STATE_ERROR_HARD_CELLVOLTAGE);
	bool dischargePermissionAllowed = modPowerElectronicsPackStateHandle->disChargeDesired &&
	                                  (modPowerElectronicsPackStateHandle->disChargeLCAllowed || modPowerElectronicsAllowForcedOnState) &&
	                                  dataHealthy;
	bool masterOkAllowed = modPowerElectronicsPackStateHandle->masterOkDesired &&
	                       dataHealthy;
	bool chargePermissionAllowed = modPowerElectronicsPackStateHandle->chargeDesired &&
	                               modPowerElectronicsPackStateHandle->chargeAllowed &&
	                               dataHealthy;
	bool chargerSafetyAllowed = modPowerElectronicsPackStateHandle->chargerSafetyDesired &&
	                            chargePermissionAllowed;

	driverHWSwitchesSetMasterOkPermission(masterOkAllowed);
	driverHWSwitchesSetDischargePermission(dischargePermissionAllowed);
	driverHWSwitchesSetChargePermission(chargePermissionAllowed);
	driverHWSwitchesSetChargerSafetyPermission(chargerSafetyAllowed);
};

void modPowerElectronicsSortCells(driverLTC6803CellsTypedef *cells, uint8_t cellCount) {
	int i,j;
	driverLTC6803CellsTypedef value;

	for(i=0 ; i<(cellCount-1) ; i++) {
		for(j=0 ; j<(cellCount-i-1) ; j++) {
				if(cells[j].cellVoltage < cells[j+1].cellVoltage) {
						value = cells[j+1];
						cells[j+1] = cells[j];
						cells[j] = value;
				}
		}
	}
};

void modPowerElectronicsCalcTempStats(void) {
	uint8_t sensorPointer;

	if(modPowerElectronicsPackStateHandle->temperatureReadoutValid) {
		float tempBatteryMax = -100.0f;
		float tempBatteryMin = 200.0f;
		float tempBatterySum = 0.0f;
		uint8_t tempBatterySumCount = 0u;

		for(sensorPointer = 0u; sensorPointer < BMS_TOTAL_TEMPS; sensorPointer++) {
			if(!modPowerElectronicsPackStateHandle->temperaturesLTC6812Valid[sensorPointer])
				continue;

			if(modPowerElectronicsPackStateHandle->temperaturesLTC6812[sensorPointer] > tempBatteryMax)
				tempBatteryMax = modPowerElectronicsPackStateHandle->temperaturesLTC6812[sensorPointer];

			if(modPowerElectronicsPackStateHandle->temperaturesLTC6812[sensorPointer] < tempBatteryMin)
				tempBatteryMin = modPowerElectronicsPackStateHandle->temperaturesLTC6812[sensorPointer];

			tempBatterySum += modPowerElectronicsPackStateHandle->temperaturesLTC6812[sensorPointer];
			tempBatterySumCount++;
		}

		modPowerElectronicsPackStateHandle->tempBatteryHigh = tempBatteryMax;
		modPowerElectronicsPackStateHandle->tempBatteryLow = tempBatteryMin;
		modPowerElectronicsPackStateHandle->tempBatteryAverage = tempBatterySumCount ?
			(tempBatterySum / (float)tempBatterySumCount) : 200.0f;

		modPowerElectronicsPackStateHandle->tempBMSHigh = modPowerElectronicsPackStateHandle->temperatures[TEMP_INT_STM_NTC];
		modPowerElectronicsPackStateHandle->tempBMSLow = modPowerElectronicsPackStateHandle->temperatures[TEMP_INT_STM_NTC];
		modPowerElectronicsPackStateHandle->tempBMSAverage = modPowerElectronicsPackStateHandle->temperatures[TEMP_INT_STM_NTC];
		return;
	}
	
	// Battery
	float   tempBatteryMax;
	float   tempBatteryMin;
	float   tempBatterySum = 0.0f;
	uint8_t tempBatterySumCount = 0;
	
	// BMS
	float   tempBMSMax;
	float   tempBMSMin;
	float   tempBMSSum = 0.0f;
	uint8_t tempBMSSumCount = 0;
	
	if(modPowerElectronicsGeneralConfigHandle->tempEnableMaskBattery){
		tempBatteryMax = -100.0f;
		tempBatteryMin = 100.0f;
	}else{
		tempBatteryMax = 0.0f;
		tempBatteryMin = 0.0f;
	}
	
	if(modPowerElectronicsGeneralConfigHandle->tempEnableMaskBMS){
		tempBMSMax = -100.0f;
		tempBMSMin = 100.0f;
	}else{
		tempBMSMax = 0.0f;
		tempBMSMin = 0.0f;
	}
	
	for(sensorPointer = 0; sensorPointer < 16; sensorPointer++){
		// Battery temperatures
		if(modPowerElectronicsGeneralConfigHandle->tempEnableMaskBattery & (1 << sensorPointer)){
			if(modPowerElectronicsPackStateHandle->temperatures[sensorPointer] > tempBatteryMax)
				tempBatteryMax = modPowerElectronicsPackStateHandle->temperatures[sensorPointer];
			
			if(modPowerElectronicsPackStateHandle->temperatures[sensorPointer] < tempBatteryMin)
				tempBatteryMin = modPowerElectronicsPackStateHandle->temperatures[sensorPointer];
			
			tempBatterySum += modPowerElectronicsPackStateHandle->temperatures[sensorPointer];
			tempBatterySumCount++;
		}
	
		// BMS temperatures
		if(modPowerElectronicsGeneralConfigHandle->tempEnableMaskBMS & (1 << sensorPointer)){
			if(modPowerElectronicsPackStateHandle->temperatures[sensorPointer] > tempBMSMax)
				tempBMSMax = modPowerElectronicsPackStateHandle->temperatures[sensorPointer];
			
			if(modPowerElectronicsPackStateHandle->temperatures[sensorPointer] < tempBMSMin)
				tempBMSMin = modPowerElectronicsPackStateHandle->temperatures[sensorPointer];
			
			tempBMSSum += modPowerElectronicsPackStateHandle->temperatures[sensorPointer];
			tempBMSSumCount++;
		}
	}
	
	// Battery temperatures
	modPowerElectronicsPackStateHandle->tempBatteryHigh    = tempBatteryMax;
	modPowerElectronicsPackStateHandle->tempBatteryLow     = tempBatteryMin;
	if(tempBatterySumCount)
		modPowerElectronicsPackStateHandle->tempBatteryAverage = tempBatterySum/tempBatterySumCount;
	else
		modPowerElectronicsPackStateHandle->tempBatteryAverage = 0.0f;
	
	// BMS temperatures
	modPowerElectronicsPackStateHandle->tempBMSHigh        = tempBMSMax;
	modPowerElectronicsPackStateHandle->tempBMSLow         = tempBMSMin;
	if(tempBMSSumCount)
		modPowerElectronicsPackStateHandle->tempBMSAverage = tempBMSSum/tempBMSSumCount;
	else
		modPowerElectronicsPackStateHandle->tempBMSAverage = 0.0f;

	if(!modPowerElectronicsPackStateHandle->temperatureReadoutValid) {
		/* Phase 3 repair: local STM32 temperature is not equivalent to migrated pack coverage.
		 * Keep aggregate temperature telemetry/fault inputs conservative until Phase 4 lands.
		 */
		if(modPowerElectronicsGeneralConfigHandle->tempEnableMaskBattery) {
			modPowerElectronicsPackStateHandle->tempBatteryHigh = 200.0f;
			modPowerElectronicsPackStateHandle->tempBatteryLow = 200.0f;
			modPowerElectronicsPackStateHandle->tempBatteryAverage = 200.0f;
		}

		if(modPowerElectronicsGeneralConfigHandle->tempEnableMaskBMS) {
			modPowerElectronicsPackStateHandle->tempBMSHigh = 200.0f;
			modPowerElectronicsPackStateHandle->tempBMSLow = 200.0f;
			modPowerElectronicsPackStateHandle->tempBMSAverage = 200.0f;
		}
	}
};

void modPowerElectronicsCalcThrottle(void) {
	uint8_t calculatedChargeThrottle = 0;
	uint8_t calculatedDisChargeThrottle = 0;
	
	static uint8_t filteredChargeThrottle = 0;
	static uint8_t filteredDisChargeThrottle = 0;
	
	float inputLowerLimitCharge = modPowerElectronicsGeneralConfigHandle->cellSoftOverVoltage - modPowerElectronicsGeneralConfigHandle->cellThrottleUpperMargin - modPowerElectronicsGeneralConfigHandle->cellThrottleUpperStart;
	float inputUpperLimitCharge = modPowerElectronicsGeneralConfigHandle->cellSoftOverVoltage - modPowerElectronicsGeneralConfigHandle->cellThrottleUpperMargin;
	float outputLowerLimitCharge = 100.0f;
	float outputUpperLimitCharge = 10.0f;
	
	float inputLowerLimitDisCharge  = modPowerElectronicsGeneralConfigHandle->cellLCSoftUnderVoltage + modPowerElectronicsGeneralConfigHandle->cellThrottleLowerMargin;
  float inputUpperLimitDisCharge  = modPowerElectronicsGeneralConfigHandle->cellLCSoftUnderVoltage + modPowerElectronicsGeneralConfigHandle->cellThrottleLowerMargin + modPowerElectronicsGeneralConfigHandle->cellThrottleLowerStart;
  float outputLowerLimitDisCharge = 5.0f;
	float outputUpperLimitDisCharge = 100.0f;

	// Calculate (dis)charge throttle
	calculatedChargeThrottle    = (uint8_t)modPowerElectronicsMapVariableFloat(modPowerElectronicsPackStateHandle->cellVoltageHigh,inputLowerLimitCharge,inputUpperLimitCharge,outputLowerLimitCharge,outputUpperLimitCharge);
	calculatedDisChargeThrottle = (uint8_t)modPowerElectronicsMapVariableFloat(modPowerElectronicsPackStateHandle->cellVoltageLow,inputLowerLimitDisCharge,inputUpperLimitDisCharge,outputLowerLimitDisCharge,outputUpperLimitDisCharge);
	
	// Filter the calculated throttle
	if(calculatedChargeThrottle > filteredChargeThrottle){
		if((calculatedChargeThrottle-filteredChargeThrottle) > modPowerElectronicsGeneralConfigHandle->throttleChargeIncreaseRate)
			filteredChargeThrottle += modPowerElectronicsGeneralConfigHandle->throttleChargeIncreaseRate;
		else
			filteredChargeThrottle = calculatedChargeThrottle;
	}else{
		filteredChargeThrottle = calculatedChargeThrottle;
	}
	
	if(calculatedDisChargeThrottle > filteredDisChargeThrottle){
		if((calculatedDisChargeThrottle-filteredDisChargeThrottle) > modPowerElectronicsGeneralConfigHandle->throttleDisChargeIncreaseRate)
			filteredDisChargeThrottle += modPowerElectronicsGeneralConfigHandle->throttleDisChargeIncreaseRate;
		else
			filteredDisChargeThrottle = calculatedDisChargeThrottle;
	}else{
		filteredDisChargeThrottle = calculatedDisChargeThrottle;
	}
	
  // Output the filtered output
	if(modPowerElectronicsPackStateHandle->chargeAllowed)
		modPowerElectronicsPackStateHandle->throttleDutyCharge = filteredChargeThrottle;
	else 
		modPowerElectronicsPackStateHandle->throttleDutyCharge = 0;
	
	if(modPowerElectronicsPackStateHandle->disChargeLCAllowed)
		modPowerElectronicsPackStateHandle->throttleDutyDischarge = filteredDisChargeThrottle;
	else 
		modPowerElectronicsPackStateHandle->throttleDutyDischarge = 0;
}

int32_t modPowerElectronicsMapVariableInt(int32_t inputVariable, int32_t inputLowerLimit, int32_t inputUpperLimit, int32_t outputLowerLimit, int32_t outputUpperLimit) {
	inputVariable = inputVariable < inputLowerLimit ? inputLowerLimit : inputVariable;
	inputVariable = inputVariable > inputUpperLimit ? inputUpperLimit : inputVariable;
	
	return (inputVariable - inputLowerLimit) * (outputUpperLimit - outputLowerLimit) / (inputUpperLimit - inputLowerLimit) + outputLowerLimit;
}

float modPowerElectronicsMapVariableFloat(float inputVariable, float inputLowerLimit, float inputUpperLimit, float outputLowerLimit, float outputUpperLimit) {
	inputVariable = inputVariable < inputLowerLimit ? inputLowerLimit : inputVariable;
	inputVariable = inputVariable > inputUpperLimit ? inputUpperLimit : inputVariable;
	
	return (inputVariable - inputLowerLimit) * (outputUpperLimit - outputLowerLimit) / (inputUpperLimit - inputLowerLimit) + outputLowerLimit;
}

void modPowerElectronicsInitISL(void) {
	// Init BUS monitor
	driverSWISL28022InitStruct ISLInitStruct;
	ISLInitStruct.ADCSetting = ADC_128_64010US;
	ISLInitStruct.busVoltageRange = BRNG_60V_1;
	ISLInitStruct.currentShuntGain = PGA_4_160MV;
	ISLInitStruct.Mode = MODE_SHUNTANDBUS_CONTINIOUS;
	driverSWISL28022Init(ISL28022_MASTER_ADDRES,ISL28022_MASTER_BUS,ISLInitStruct);
}

void modPowerElectronicsCheckPackSOA(void) {
	bool packOutsideLimits = false;
	
	if(modPowerElectronicsGeneralConfigHandle->tempEnableMaskBMS) {
		packOutsideLimits |= (!modPowerElectronicsPackStateHandle->temperatureReadoutValid) ? true : false;
		packOutsideLimits |= (modPowerElectronicsPackStateHandle->tempBMSHigh > 70.0f) ? true : false;
	}
	
	if(modPowerElectronicsGeneralConfigHandle->tempEnableMaskBattery) {
		packOutsideLimits |= (!modPowerElectronicsPackStateHandle->temperatureReadoutValid) ? true : false;
		packOutsideLimits |= (modPowerElectronicsPackStateHandle->tempBatteryHigh > 70.0f) ? true : false;
	}
	
	packOutsideLimits |= (modPowerElectronicsISLErrorCount >= ISLErrorThreshold) ? true : false;
	
	// TODO: timout when restoring SOA state.
  modPowerElectronicsPackStateHandle->packInSOA = packOutsideLimits;
}

bool modPowerElectronicsHCSafetyCANAndPowerButtonCheck(void) {
	if(modPowerElectronicsGeneralConfigHandle->useCANSafetyInput)	
		return (modPowerElectronicsPackStateHandle->safetyOverCANHCSafeNSafe && modPowerElectronicsPackStateHandle->powerButtonActuated);
	else
		return true;
}

void modPowerElectronicsResetBalanceModeActiveTimeout(void) {
	modPowerElectronicsBalanceModeActiveLastTick = HAL_GetTick();
}
