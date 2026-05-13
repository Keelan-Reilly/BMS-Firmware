#include "modPowerElectronics.h"
#include "string.h"

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
uint32_t modPowerElectronicsOpenWireDiagnosticLastTick;
uint8_t  modPowerElectronicsUnderAndOverVoltageErrorCount;
driverLTC6803ConfigStructTypedef modPowerElectronicsLTCconfigStruct;
bool     modPowerElectronicsAllowForcedOnState;
uint16_t modPowerElectronicsTemperatureArray[3];
uint16_t tempTemperature;
uint8_t  modPowerElectronicsISLErrorCount;

#define MOD_POWER_ELECTRONICS_INVALID_TEMPERATURE_C     200.0f
#define MOD_POWER_ELECTRONICS_OPEN_WIRE_INTERVAL_MS     1000u
#define MOD_POWER_ELECTRONICS_TEMP_REQUIRED_MASK_ALL    0x7FFFu
#define MOD_POWER_ELECTRONICS_ENEPAQ_TABLE_POINTS       33u
#define MOD_POWER_ELECTRONICS_ENEPAQ_MIN_MV             1300u
#define MOD_POWER_ELECTRONICS_ENEPAQ_MAX_MV             2440u
#define MOD_POWER_ELECTRONICS_TEMP_LIMIT_C              70.0f
#define MOD_POWER_ELECTRONICS_PRECHARGE_COMPLETE_RATIO_FALLBACK 0.80f
#define MOD_POWER_ELECTRONICS_PRECHARGE_MIN_VBAT        10.0f
#define MOD_POWER_ELECTRONICS_CONTACTOR_WELD_VPACK_THRESHOLD 10.0f
#define MOD_POWER_ELECTRONICS_FAULT_NONE                0u
#define MOD_POWER_ELECTRONICS_NO_PRIMARY_FAULT          0xFFu

typedef struct {
	uint16_t milliVolts;
	float    temperatureC;
} modPowerElectronicsEnepaqPointTypedef;

typedef struct {
	float   cellVoltage;
	uint8_t cellIndex;
} modPowerElectronicsBalanceCandidateTypedef;

typedef struct {
	const char *name;
	uint32_t    mask;
} modPowerElectronicsFaultDescriptorTypedef;

static const modPowerElectronicsFaultDescriptorTypedef modPowerElectronicsFaultDescriptors[BMS_FAULT_COUNT] = {
	{"CELL_OV_SOFT", BMS_FAULT_MASK(BMS_FAULT_CELL_OV_SOFT)},
	{"CELL_OV_HARD", BMS_FAULT_MASK(BMS_FAULT_CELL_OV_HARD)},
	{"CELL_UV_SOFT", BMS_FAULT_MASK(BMS_FAULT_CELL_UV_SOFT)},
	{"CELL_UV_HARD", BMS_FAULT_MASK(BMS_FAULT_CELL_UV_HARD)},
	{"CELL_READ_INVALID", BMS_FAULT_MASK(BMS_FAULT_CELL_READ_INVALID)},
	{"CELL_OPEN_WIRE", BMS_FAULT_MASK(BMS_FAULT_CELL_OPEN_WIRE)},
	{"TEMP_OVER_LIMIT", BMS_FAULT_MASK(BMS_FAULT_TEMP_OVER_LIMIT)},
	{"TEMP_READ_INVALID", BMS_FAULT_MASK(BMS_FAULT_TEMP_READ_INVALID)},
	{"TEMP_SENSOR_INVALID", BMS_FAULT_MASK(BMS_FAULT_TEMP_SENSOR_INVALID)},
	{"ISL_READ_INVALID", BMS_FAULT_MASK(BMS_FAULT_ISL_READ_INVALID)},
	{"VPACK_READ_INVALID", BMS_FAULT_MASK(BMS_FAULT_VPACK_READ_INVALID)},
	{"PRECHARGE_TIMEOUT", BMS_FAULT_MASK(BMS_FAULT_PRECHARGE_TIMEOUT)},
	{"WELDED_CONTACTOR_SUSPECT", BMS_FAULT_MASK(BMS_FAULT_WELDED_CONTACTOR_SUSPECT)},
	{"INTERNAL_FATAL", BMS_FAULT_MASK(BMS_FAULT_INTERNAL_FATAL)}
};

static bool modPowerElectronicsIsRequiredTempChannel(uint8_t tempIndex);
static void modPowerElectronicsDisableCellBalancing(void);

/* The exact physical mapping from this board's TEMP-chain sensor-bias topology onto
 * the 5 x 15 LTC6812 channels is not documented in the repo. Keep the required-
 * channel policy conservative by default: require every TEMP-chain channel until
 * bench mapping proves otherwise.
 */
static const uint16_t modPowerElectronicsRequiredTempChannelMask[BMS_LTC6812_DEVICES] = {
	MOD_POWER_ELECTRONICS_TEMP_REQUIRED_MASK_ALL,
	MOD_POWER_ELECTRONICS_TEMP_REQUIRED_MASK_ALL,
	MOD_POWER_ELECTRONICS_TEMP_REQUIRED_MASK_ALL,
	MOD_POWER_ELECTRONICS_TEMP_REQUIRED_MASK_ALL,
	MOD_POWER_ELECTRONICS_TEMP_REQUIRED_MASK_ALL
};

static const modPowerElectronicsEnepaqPointTypedef modPowerElectronicsEnepaqCurve[MOD_POWER_ELECTRONICS_ENEPAQ_TABLE_POINTS] = {
	{2440u, -40.0f},
	{2420u, -35.0f},
	{2400u, -30.0f},
	{2380u, -25.0f},
	{2350u, -20.0f},
	{2320u, -15.0f},
	{2270u, -10.0f},
	{2230u, -5.0f},
	{2170u, 0.0f},
	{2110u, 5.0f},
	{2050u, 10.0f},
	{1990u, 15.0f},
	{1920u, 20.0f},
	{1860u, 25.0f},
	{1800u, 30.0f},
	{1740u, 35.0f},
	{1680u, 40.0f},
	{1630u, 45.0f},
	{1590u, 50.0f},
	{1550u, 55.0f},
	{1510u, 60.0f},
	{1480u, 65.0f},
	{1450u, 70.0f},
	{1430u, 75.0f},
	{1400u, 80.0f},
	{1380u, 85.0f},
	{1370u, 90.0f},
	{1350u, 95.0f},
	{1340u, 100.0f},
	{1330u, 105.0f},
	{1320u, 110.0f},
	{1310u, 115.0f},
	{1300u, 120.0f}
};

static uint8_t modPowerElectronicsGetActiveCellCount(void) {
	if(modPowerElectronicsPackStateHandle->cellVoltageReadoutValid)
		return modPowerElectronicsPackStateHandle->cellVoltageReadoutCount;

	return modPowerElectronicsGeneralConfigHandle->noOfCells;
}

static bool modPowerElectronicsTemperatureCoverageRequired(void) {
	return (modPowerElectronicsGeneralConfigHandle->tempEnableMaskBattery ||
	        modPowerElectronicsGeneralConfigHandle->tempEnableMaskBMS);
}

static uint8_t modPowerElectronicsCountFaultBits(uint32_t faultMask) {
	uint8_t faultCount = 0u;

	for(uint8_t bitIndex = 0u; bitIndex < BMS_FAULT_COUNT; bitIndex++) {
		if((faultMask & BMS_FAULT_MASK(bitIndex)) != 0u)
			faultCount++;
	}

	return faultCount;
}

static uint8_t modPowerElectronicsFindPrimaryFaultBit(uint32_t faultMask) {
	for(uint8_t bitIndex = 0u; bitIndex < BMS_FAULT_COUNT; bitIndex++) {
		if((faultMask & BMS_FAULT_MASK(bitIndex)) != 0u)
			return bitIndex;
	}

	return MOD_POWER_ELECTRONICS_NO_PRIMARY_FAULT;
}

static bool modPowerElectronicsRequiredTemperatureInvalid(void) {
	for(uint8_t tempIndex = 0u; tempIndex < BMS_TOTAL_TEMPS; tempIndex++) {
		if(!modPowerElectronicsIsRequiredTempChannel(tempIndex))
			continue;

		if(!modPowerElectronicsPackStateHandle->temperaturesLTC6812Valid[tempIndex] ||
		   !modPowerElectronicsPackStateHandle->tempSensorVoltagesLTC6812[tempIndex].valid)
			return true;
	}

	return false;
}

static float modPowerElectronicsGetPrechargeRatioThreshold(void) {
	float configuredThreshold = modPowerElectronicsGeneralConfigHandle->minimalPrechargePercentage;

	if((configuredThreshold <= 0.0f) || (configuredThreshold > 1.0f))
		return MOD_POWER_ELECTRONICS_PRECHARGE_COMPLETE_RATIO_FALLBACK;

	return configuredThreshold;
}

static float modPowerElectronicsGetPrechargeMinimumVbat(void) {
	float configuredMinimum = modPowerElectronicsGeneralConfigHandle->noOfCells *
	                          modPowerElectronicsGeneralConfigHandle->cellHardUnderVoltage;

	if(configuredMinimum < MOD_POWER_ELECTRONICS_PRECHARGE_MIN_VBAT)
		return MOD_POWER_ELECTRONICS_PRECHARGE_MIN_VBAT;

	return configuredMinimum;
}

static bool modPowerElectronicsPrechargeOutputsShouldBeOpen(void) {
	return !modPowerElectronicsPackStateHandle->masterOkDesired &&
	       !modPowerElectronicsPackStateHandle->disChargeDesired &&
	       (modPowerElectronicsPackStateHandle->operationalState == OP_STATE_INIT ||
	        modPowerElectronicsPackStateHandle->operationalState == OP_STATE_POWER_DOWN ||
	        modPowerElectronicsPackStateHandle->operationalState == OP_STATE_BATTERY_DEAD ||
	        modPowerElectronicsPackStateHandle->operationalState == OP_STATE_EXTERNAL ||
	        modPowerElectronicsPackStateHandle->operationalState == OP_STATE_ERROR ||
	        modPowerElectronicsPackStateHandle->operationalState == OP_STATE_ERROR_PRECHARGE);
}

static bool modPowerElectronicsWeldedContactorSuspect(void) {
	float weldedThreshold;

	if(!modPowerElectronicsPackStateHandle->prechargeMeasurementValid)
		return false;

	if(!modPowerElectronicsPrechargeOutputsShouldBeOpen())
		return false;

	weldedThreshold = modPowerElectronicsPackStateHandle->prechargeRatioThreshold *
	                  modPowerElectronicsPackStateHandle->vBatVoltage;

	if(weldedThreshold < MOD_POWER_ELECTRONICS_CONTACTOR_WELD_VPACK_THRESHOLD)
		weldedThreshold = MOD_POWER_ELECTRONICS_CONTACTOR_WELD_VPACK_THRESHOLD;

	return modPowerElectronicsPackStateHandle->vPackVoltage >= weldedThreshold;
}

static void modPowerElectronicsUpdatePrechargeStatus(void) {
	float minimumVbat = modPowerElectronicsGetPrechargeMinimumVbat();
	float ratioThreshold = modPowerElectronicsGetPrechargeRatioThreshold();

	modPowerElectronicsPackStateHandle->prechargeRatioThreshold = ratioThreshold;
	modPowerElectronicsPackStateHandle->prechargeMinimumVbat = minimumVbat;
	modPowerElectronicsPackStateHandle->prechargeVoltageRatio = 0.0f;
	modPowerElectronicsPackStateHandle->prechargeVoltageDelta = 0.0f;
	modPowerElectronicsPackStateHandle->prechargeMeasurementValid = false;
	modPowerElectronicsPackStateHandle->prechargeComplete = false;
	modPowerElectronicsPackStateHandle->weldedContactorSuspect = false;

	if(!modPowerElectronicsPackStateHandle->vBatReadoutValid ||
	   !modPowerElectronicsPackStateHandle->vPackReadoutValid)
		return;

	if(modPowerElectronicsPackStateHandle->vBatVoltage < minimumVbat)
		return;

	modPowerElectronicsPackStateHandle->prechargeVoltageRatio =
		modPowerElectronicsPackStateHandle->vPackVoltage /
		modPowerElectronicsPackStateHandle->vBatVoltage;
	modPowerElectronicsPackStateHandle->prechargeVoltageDelta =
		modPowerElectronicsPackStateHandle->vBatVoltage -
		modPowerElectronicsPackStateHandle->vPackVoltage;
	modPowerElectronicsPackStateHandle->prechargeMeasurementValid = true;
	modPowerElectronicsPackStateHandle->prechargeComplete =
		modPowerElectronicsPackStateHandle->prechargeVoltageRatio >= ratioThreshold;
	modPowerElectronicsPackStateHandle->weldedContactorSuspect =
		modPowerElectronicsWeldedContactorSuspect();
}

static uint8_t modPowerElectronicsBuildUIFaultCode(uint32_t activeFaultMask) {
	if(activeFaultMask & BMS_FAULT_MASK(BMS_FAULT_INTERNAL_FATAL))
		return 6u;

	if(activeFaultMask & (BMS_FAULT_MASK(BMS_FAULT_PRECHARGE_TIMEOUT) |
	                      BMS_FAULT_MASK(BMS_FAULT_WELDED_CONTACTOR_SUSPECT)))
		return 5u;

	if(activeFaultMask & (BMS_FAULT_MASK(BMS_FAULT_ISL_READ_INVALID) |
	                      BMS_FAULT_MASK(BMS_FAULT_VPACK_READ_INVALID)))
		return 4u;

	if(activeFaultMask & (BMS_FAULT_MASK(BMS_FAULT_TEMP_OVER_LIMIT) |
	                      BMS_FAULT_MASK(BMS_FAULT_TEMP_READ_INVALID) |
	                      BMS_FAULT_MASK(BMS_FAULT_TEMP_SENSOR_INVALID)))
		return 3u;

	if(activeFaultMask & (BMS_FAULT_MASK(BMS_FAULT_CELL_READ_INVALID) |
	                      BMS_FAULT_MASK(BMS_FAULT_CELL_OPEN_WIRE)))
		return 2u;

	if(activeFaultMask & (BMS_FAULT_MASK(BMS_FAULT_CELL_OV_SOFT) |
	                      BMS_FAULT_MASK(BMS_FAULT_CELL_OV_HARD) |
	                      BMS_FAULT_MASK(BMS_FAULT_CELL_UV_SOFT) |
	                      BMS_FAULT_MASK(BMS_FAULT_CELL_UV_HARD)))
		return 1u;

	return 0u;
}

static void modPowerElectronicsEvaluateFaults(void) {
	uint32_t activeFaultMask = MOD_POWER_ELECTRONICS_FAULT_NONE;
	bool temperatureCoverageRequired = modPowerElectronicsTemperatureCoverageRequired();

	if(!modPowerElectronicsPackStateHandle->cellVoltageReadoutValid) {
		activeFaultMask |= BMS_FAULT_MASK(BMS_FAULT_CELL_READ_INVALID);
	} else {
		if(modPowerElectronicsPackStateHandle->cellVoltageHigh >= modPowerElectronicsGeneralConfigHandle->cellSoftOverVoltage)
			activeFaultMask |= BMS_FAULT_MASK(BMS_FAULT_CELL_OV_SOFT);

		if(modPowerElectronicsPackStateHandle->cellVoltageHigh >= modPowerElectronicsGeneralConfigHandle->cellHardOverVoltage)
			activeFaultMask |= BMS_FAULT_MASK(BMS_FAULT_CELL_OV_HARD);

		if((modPowerElectronicsPackStateHandle->cellVoltageLow <= modPowerElectronicsGeneralConfigHandle->cellLCSoftUnderVoltage) ||
		   (modPowerElectronicsPackStateHandle->cellVoltageLow <= modPowerElectronicsGeneralConfigHandle->cellHCSoftUnderVoltage))
			activeFaultMask |= BMS_FAULT_MASK(BMS_FAULT_CELL_UV_SOFT);

		if(modPowerElectronicsPackStateHandle->cellVoltageLow <= modPowerElectronicsGeneralConfigHandle->cellHardUnderVoltage)
			activeFaultMask |= BMS_FAULT_MASK(BMS_FAULT_CELL_UV_HARD);
	}

	if(!modPowerElectronicsPackStateHandle->cellOpenWireValid ||
	   (modPowerElectronicsPackStateHandle->cellOpenWireFaultCount != 0u))
		activeFaultMask |= BMS_FAULT_MASK(BMS_FAULT_CELL_OPEN_WIRE);

	if(temperatureCoverageRequired && !modPowerElectronicsPackStateHandle->temperatureReadoutValid)
		activeFaultMask |= BMS_FAULT_MASK(BMS_FAULT_TEMP_READ_INVALID);

	if(temperatureCoverageRequired && modPowerElectronicsRequiredTemperatureInvalid())
		activeFaultMask |= BMS_FAULT_MASK(BMS_FAULT_TEMP_SENSOR_INVALID);

	if((modPowerElectronicsGeneralConfigHandle->tempEnableMaskBattery &&
	    (modPowerElectronicsPackStateHandle->tempBatteryHigh > MOD_POWER_ELECTRONICS_TEMP_LIMIT_C)) ||
	   (modPowerElectronicsGeneralConfigHandle->tempEnableMaskBMS &&
	    (modPowerElectronicsPackStateHandle->tempBMSHigh > MOD_POWER_ELECTRONICS_TEMP_LIMIT_C)))
		activeFaultMask |= BMS_FAULT_MASK(BMS_FAULT_TEMP_OVER_LIMIT);

	if(!modPowerElectronicsPackStateHandle->vBatReadoutValid ||
	   !modPowerElectronicsPackStateHandle->currentReadoutValid ||
	   !modPowerElectronicsPackStateHandle->powerMonitorReadoutValid)
		activeFaultMask |= BMS_FAULT_MASK(BMS_FAULT_ISL_READ_INVALID);

	if(!modPowerElectronicsPackStateHandle->vPackReadoutValid)
		activeFaultMask |= BMS_FAULT_MASK(BMS_FAULT_VPACK_READ_INVALID);

	if(modPowerElectronicsPackStateHandle->operationalState == OP_STATE_ERROR_PRECHARGE)
		activeFaultMask |= BMS_FAULT_MASK(BMS_FAULT_PRECHARGE_TIMEOUT);

	if(modPowerElectronicsWeldedContactorSuspect())
		activeFaultMask |= BMS_FAULT_MASK(BMS_FAULT_WELDED_CONTACTOR_SUSPECT);

	if((modPowerElectronicsPackStateHandle->operationalState == OP_STATE_ERROR) &&
	   (activeFaultMask == MOD_POWER_ELECTRONICS_FAULT_NONE))
		activeFaultMask |= BMS_FAULT_MASK(BMS_FAULT_INTERNAL_FATAL);

	modPowerElectronicsPackStateHandle->activeFaultMask = activeFaultMask;
	modPowerElectronicsPackStateHandle->latchedFaultMask |= activeFaultMask;
	modPowerElectronicsPackStateHandle->activeFaultCount =
		modPowerElectronicsCountFaultBits(activeFaultMask);
	modPowerElectronicsPackStateHandle->primaryFaultBit =
		modPowerElectronicsFindPrimaryFaultBit(activeFaultMask);
	modPowerElectronicsPackStateHandle->uiFaultCode =
		modPowerElectronicsBuildUIFaultCode(activeFaultMask);
}

static uint32_t modPowerElectronicsGetMasterOkBlockingFaultMask(void) {
	return BMS_FAULT_MASK(BMS_FAULT_CELL_READ_INVALID) |
	       BMS_FAULT_MASK(BMS_FAULT_CELL_OPEN_WIRE) |
	       BMS_FAULT_MASK(BMS_FAULT_TEMP_OVER_LIMIT) |
	       BMS_FAULT_MASK(BMS_FAULT_TEMP_READ_INVALID) |
	       BMS_FAULT_MASK(BMS_FAULT_TEMP_SENSOR_INVALID) |
	       BMS_FAULT_MASK(BMS_FAULT_ISL_READ_INVALID) |
	       BMS_FAULT_MASK(BMS_FAULT_VPACK_READ_INVALID) |
	       BMS_FAULT_MASK(BMS_FAULT_PRECHARGE_TIMEOUT) |
	       BMS_FAULT_MASK(BMS_FAULT_WELDED_CONTACTOR_SUSPECT) |
	       BMS_FAULT_MASK(BMS_FAULT_INTERNAL_FATAL) |
	       BMS_FAULT_MASK(BMS_FAULT_CELL_OV_HARD) |
	       BMS_FAULT_MASK(BMS_FAULT_CELL_UV_HARD);
}

static uint32_t modPowerElectronicsGetDischargeBlockingFaultMask(void) {
	return modPowerElectronicsGetMasterOkBlockingFaultMask() |
	       BMS_FAULT_MASK(BMS_FAULT_CELL_UV_SOFT);
}

static uint32_t modPowerElectronicsGetChargeBlockingFaultMask(void) {
	return modPowerElectronicsGetMasterOkBlockingFaultMask() |
	       BMS_FAULT_MASK(BMS_FAULT_CELL_OV_SOFT);
}

static uint32_t modPowerElectronicsGetBalancingBlockingFaultMask(void) {
	return BMS_FAULT_MASK(BMS_FAULT_CELL_READ_INVALID) |
	       BMS_FAULT_MASK(BMS_FAULT_CELL_OPEN_WIRE) |
	       BMS_FAULT_MASK(BMS_FAULT_CELL_OV_HARD) |
	       BMS_FAULT_MASK(BMS_FAULT_CELL_UV_HARD) |
	       BMS_FAULT_MASK(BMS_FAULT_PRECHARGE_TIMEOUT) |
	       BMS_FAULT_MASK(BMS_FAULT_WELDED_CONTACTOR_SUSPECT) |
	       BMS_FAULT_MASK(BMS_FAULT_INTERNAL_FATAL);
}

static void modPowerElectronicsMirrorLegacyCellVoltages(void) {
	for(uint8_t cellPointer = 0u; cellPointer < NoOfCellsPossibleOnChip; cellPointer++) {
		modPowerElectronicsPackStateHandle->cellVoltagesIndividual[cellPointer].cellVoltage = modPowerElectronicsPackStateHandle->cellVoltagesLTC6812[cellPointer].cellVoltage;
		modPowerElectronicsPackStateHandle->cellVoltagesIndividual[cellPointer].cellNumber = modPowerElectronicsPackStateHandle->cellVoltagesLTC6812[cellPointer].cellNumber;
	}
}

static bool modPowerElectronicsRefreshCellMeasurement(bool disableBalancingOnFailure) {
	driverLTC6812StatusTypedef cellChainStatus;
	bool cellReadValid = driverSWLTC6812ReadCellVoltages(modPowerElectronicsPackStateHandle->cellVoltagesLTC6812);

	cellChainStatus = driverSWLTC6812GetCellChainStatus();
	modPowerElectronicsPackStateHandle->cellVoltageReadoutValid = cellReadValid;
	modPowerElectronicsPackStateHandle->cellVoltageReadoutErrorCount = cellChainStatus.lastReadPECErrors;

	if(cellReadValid)
		modPowerElectronicsMirrorLegacyCellVoltages();

	modPowerElectronicsCalculateCellStats();

	if(!cellReadValid && disableBalancingOnFailure)
		modPowerElectronicsDisableCellBalancing();

	if(!driverSWLTC6812StartCellVoltageConversion())
		modPowerElectronicsPackStateHandle->cellVoltageReadoutValid = false;

	return cellReadValid;
}

static void modPowerElectronicsClearCellBalanceState(void) {
	modPowerElectronicsPackStateHandle->cellBalanceResistorEnableMask = 0u;
	modPowerElectronicsPackStateHandle->cellBalancingValid = false;
	modPowerElectronicsPackStateHandle->cellBalancingErrorCount = 0u;
	modPowerElectronicsPackStateHandle->cellBalancingActiveCount = 0u;
	memset(modPowerElectronicsPackStateHandle->cellBalanceMaskPerDevice, 0, sizeof(modPowerElectronicsPackStateHandle->cellBalanceMaskPerDevice));
	memset(modPowerElectronicsPackStateHandle->cellBalanceFlags, 0, sizeof(modPowerElectronicsPackStateHandle->cellBalanceFlags));
}

static void modPowerElectronicsMirrorLegacyBalanceMask(void) {
	uint16_t legacyMask = 0u;

	for(uint8_t cellIndex = 0u; cellIndex < 16u && cellIndex < BMS_TOTAL_CELLS; cellIndex++) {
		if(modPowerElectronicsPackStateHandle->cellBalanceFlags[cellIndex] != 0u)
			legacyMask |= (uint16_t)(1u << cellIndex);
	}

	modPowerElectronicsPackStateHandle->cellBalanceResistorEnableMask = legacyMask;
}

static void modPowerElectronicsStoreCellBalanceMask(const uint16_t balanceMaskPerDevice[BMS_LTC6812_DEVICES], bool valid, uint8_t errorCount) {
	uint8_t activeCount = 0u;

	for(uint8_t deviceIndex = 0u; deviceIndex < BMS_LTC6812_DEVICES; deviceIndex++) {
		modPowerElectronicsPackStateHandle->cellBalanceMaskPerDevice[deviceIndex] = balanceMaskPerDevice[deviceIndex];

		for(uint8_t cellIndexOnDevice = 0u; cellIndexOnDevice < BMS_LTC6812_CELLS_PER_DEVICE; cellIndexOnDevice++) {
			uint8_t flatIndex = (uint8_t)((deviceIndex * BMS_LTC6812_CELLS_PER_DEVICE) + cellIndexOnDevice);
			bool active = (balanceMaskPerDevice[deviceIndex] & (1u << cellIndexOnDevice)) != 0u;

			modPowerElectronicsPackStateHandle->cellBalanceFlags[flatIndex] = (uint8_t)(active ? 1u : 0u);
			if(active)
				activeCount++;
		}
	}

	modPowerElectronicsPackStateHandle->cellBalancingValid = valid;
	modPowerElectronicsPackStateHandle->cellBalancingErrorCount = errorCount;
	modPowerElectronicsPackStateHandle->cellBalancingActiveCount = activeCount;
	modPowerElectronicsMirrorLegacyBalanceMask();
}

static bool modPowerElectronicsCellBalancingStateAllowed(void) {
	switch(modPowerElectronicsPackStateHandle->operationalState) {
		case OP_STATE_CHARGING:
		case OP_STATE_BALANCING:
			return true;
		case OP_STATE_ERROR:
		case OP_STATE_ERROR_PRECHARGE:
		case OP_STATE_POWER_DOWN:
		case OP_STATE_BATTERY_DEAD:
		case OP_STATE_EXTERNAL:
			return false;
		default:
			return false;
	}
}

static bool modPowerElectronicsCellBalancingShouldRun(void) {
	if((modPowerElectronicsPackStateHandle->activeFaultMask &
	    modPowerElectronicsGetBalancingBlockingFaultMask()) != 0u)
		return false;

	if(!modPowerElectronicsCellBalancingStateAllowed())
		return false;

	return ((modPowerElectronicsPackStateHandle->chargeDesired && !modPowerElectronicsPackStateHandle->disChargeDesired) ||
	        modPowerElectronicsPackStateHandle->chargeBalanceActive ||
	        !modPowerElectronicsPackStateHandle->chargeAllowed);
}

static void modPowerElectronicsDisableCellBalancing(void) {
	if(!driverSWLTC6812DisableAllCellBalancing()) {
		driverLTC6812BalanceStatusTypedef balanceStatus = driverSWLTC6812GetCellBalanceStatus();
		modPowerElectronicsClearCellBalanceState();
		modPowerElectronicsPackStateHandle->cellBalancingErrorCount =
			(uint8_t)(balanceStatus.lastErrorCount + balanceStatus.lastConfigPECErrors);
		return;
	}

	modPowerElectronicsStoreCellBalanceMask(driverSWLTC6812GetCellBalanceStatus().balanceMaskPerDevice, true, 0u);
}

static void modPowerElectronicsSortBalanceCandidates(
	modPowerElectronicsBalanceCandidateTypedef *cells,
	uint8_t cellCount) {
	bool switched = false;

	for(uint8_t j = 0u; j < cellCount; j++) {
		switched = false;

		for(uint8_t i = 0u; i < (cellCount - 1u); i++) {
			if(cells[i].cellVoltage < cells[i + 1u].cellVoltage) {
				modPowerElectronicsBalanceCandidateTypedef tempCell = cells[i];
				cells[i] = cells[i + 1u];
				cells[i + 1u] = tempCell;
				switched = true;
			}
		}

		if(!switched)
			break;
	}
}

static void modPowerElectronicsMarkCellOpenWireUnavailable(void) {
	modPowerElectronicsPackStateHandle->cellOpenWireValid = false;
	modPowerElectronicsPackStateHandle->cellOpenWireFaultCount = 0u;
	modPowerElectronicsPackStateHandle->cellOpenWireDiagnosticErrorCount = 0u;
	modPowerElectronicsPackStateHandle->activeFaultMask = MOD_POWER_ELECTRONICS_FAULT_NONE;
	modPowerElectronicsPackStateHandle->latchedFaultMask = MOD_POWER_ELECTRONICS_FAULT_NONE;
	modPowerElectronicsPackStateHandle->activeFaultCount = 0u;
	modPowerElectronicsPackStateHandle->primaryFaultBit = MOD_POWER_ELECTRONICS_NO_PRIMARY_FAULT;
	modPowerElectronicsPackStateHandle->uiFaultCode = 0u;
	memset(modPowerElectronicsPackStateHandle->cellOpenWireFlags, 0, sizeof(modPowerElectronicsPackStateHandle->cellOpenWireFlags));
}

static void modPowerElectronicsUpdateCellOpenWireStatus(bool shouldRunDiagnostic) {
	driverLTC6812OpenWireStatusTypedef openWireStatus;

	if(!shouldRunDiagnostic) {
		modPowerElectronicsMarkCellOpenWireUnavailable();
		return;
	}

	openWireStatus = driverSWLTC6812GetCellOpenWireStatus();
	modPowerElectronicsPackStateHandle->cellOpenWireValid = openWireStatus.lastDiagnosticValid;
	modPowerElectronicsPackStateHandle->cellOpenWireFaultCount = openWireStatus.openWireFaultCount;
	modPowerElectronicsPackStateHandle->cellOpenWireDiagnosticErrorCount =
		(uint8_t)(openWireStatus.lastDiagnosticErrorCount + openWireStatus.lastDiagnosticPECErrors);

	for(uint8_t cellIndex = 0u; cellIndex < BMS_TOTAL_CELLS; cellIndex++) {
		modPowerElectronicsPackStateHandle->cellOpenWireFlags[cellIndex] =
			(uint8_t)(openWireStatus.openWireFlags[cellIndex] ? 1u : 0u);
	}
}

static bool modPowerElectronicsIsRequiredTempChannel(uint8_t tempIndex) {
	uint8_t deviceIndex = (uint8_t)(tempIndex / BMS_LTC6812_CELLS_PER_DEVICE);
	uint8_t channelIndex = (uint8_t)(tempIndex % BMS_LTC6812_CELLS_PER_DEVICE);

	if(deviceIndex >= BMS_LTC6812_DEVICES)
		return false;

	return (modPowerElectronicsRequiredTempChannelMask[deviceIndex] & (1u << channelIndex)) != 0u;
}

static uint8_t modPowerElectronicsCountRequiredTempChannels(void) {
	uint8_t requiredCount = 0u;

	for(uint8_t tempIndex = 0u; tempIndex < BMS_TOTAL_TEMPS; tempIndex++) {
		if(modPowerElectronicsIsRequiredTempChannel(tempIndex))
			requiredCount++;
	}

	return requiredCount;
}

static void modPowerElectronicsMarkTemperatureReadoutUnavailable(void) {
	for(uint8_t sensorPointer = 0u; sensorPointer < NoOfTempSensors; sensorPointer++) {
		modPowerElectronicsPackStateHandle->temperatures[sensorPointer] = MOD_POWER_ELECTRONICS_INVALID_TEMPERATURE_C;
	}

	for(uint8_t sensorPointer = 0u; sensorPointer < BMS_TOTAL_TEMPS; sensorPointer++) {
		modPowerElectronicsPackStateHandle->temperaturesLTC6812[sensorPointer] = MOD_POWER_ELECTRONICS_INVALID_TEMPERATURE_C;
		modPowerElectronicsPackStateHandle->temperaturesLTC6812Valid[sensorPointer] = false;
		modPowerElectronicsPackStateHandle->tempSensorVoltagesLTC6812[sensorPointer].valid = false;
	}

	/* Temperature coverage must remain conservative until the TEMP-chain readout
	 * and all required Enepaq conversions are valid.
	 */
	modPowerElectronicsPackStateHandle->temperatureReadoutValid = false;
}

static bool modPowerElectronicsConvertEnepaqTemperatureVoltage(driverLTC6812AnalogVoltageTypedef *sensorVoltage, float *temperatureC) {
	/* Enepaq VTC6 module datasheet Rev. D, p.3 Table 5:
	 * the temperature-sensor output is specified from 2.44V at -40C down to
	 * 1.30V at 120C, and the datasheet explicitly allows linear interpolation.
	 */
	if((sensorVoltage->milliVolts < MOD_POWER_ELECTRONICS_ENEPAQ_MIN_MV) ||
	   (sensorVoltage->milliVolts > MOD_POWER_ELECTRONICS_ENEPAQ_MAX_MV)) {
		*temperatureC = MOD_POWER_ELECTRONICS_INVALID_TEMPERATURE_C;
		sensorVoltage->valid = false;
		return false;
	}

	for(uint8_t pointIndex = 0u; pointIndex < (MOD_POWER_ELECTRONICS_ENEPAQ_TABLE_POINTS - 1u); pointIndex++) {
		const modPowerElectronicsEnepaqPointTypedef *upperPoint = &modPowerElectronicsEnepaqCurve[pointIndex];
		const modPowerElectronicsEnepaqPointTypedef *lowerPoint = &modPowerElectronicsEnepaqCurve[pointIndex + 1u];

		if(sensorVoltage->milliVolts > upperPoint->milliVolts)
			continue;

		if(sensorVoltage->milliVolts < lowerPoint->milliVolts)
			continue;

		if(sensorVoltage->milliVolts == upperPoint->milliVolts) {
			*temperatureC = upperPoint->temperatureC;
		} else if(sensorVoltage->milliVolts == lowerPoint->milliVolts) {
			*temperatureC = lowerPoint->temperatureC;
		} else {
			float voltageSpan = (float)((int32_t)upperPoint->milliVolts - (int32_t)lowerPoint->milliVolts);
			float voltageOffset = (float)((int32_t)upperPoint->milliVolts - (int32_t)sensorVoltage->milliVolts);
			float temperatureSpan = lowerPoint->temperatureC - upperPoint->temperatureC;

			*temperatureC = upperPoint->temperatureC + ((temperatureSpan * voltageOffset) / voltageSpan);
		}

		sensorVoltage->valid = true;
		return true;
	}

	*temperatureC = MOD_POWER_ELECTRONICS_INVALID_TEMPERATURE_C;
	sensorVoltage->valid = false;
	return false;
}

static void modPowerElectronicsUpdateTemperatureChainReadout(bool tempReadValid) {
	uint8_t convertedTemperatureCount = 0u;
	uint8_t requiredTemperatureCount = modPowerElectronicsCountRequiredTempChannels();

	modPowerElectronicsMarkTemperatureReadoutUnavailable();
	modPowerElectronicsPackStateHandle->temperatureReadoutCount = requiredTemperatureCount;

	if(!tempReadValid)
		return;

	for(uint8_t tempIndex = 0u; tempIndex < BMS_TOTAL_TEMPS; tempIndex++) {
		float convertedTemperature = MOD_POWER_ELECTRONICS_INVALID_TEMPERATURE_C;
		bool convertedValid = modPowerElectronicsConvertEnepaqTemperatureVoltage(
			&modPowerElectronicsPackStateHandle->tempSensorVoltagesLTC6812[tempIndex],
			&convertedTemperature);

		modPowerElectronicsPackStateHandle->temperaturesLTC6812[tempIndex] = convertedTemperature;
		modPowerElectronicsPackStateHandle->temperaturesLTC6812Valid[tempIndex] = convertedValid;
		if(convertedValid && modPowerElectronicsIsRequiredTempChannel(tempIndex))
			convertedTemperatureCount++;
	}

	modPowerElectronicsPackStateHandle->temperatureReadoutValid =
		(requiredTemperatureCount > 0u) && (convertedTemperatureCount == requiredTemperatureCount);
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
	modPowerElectronicsPackStateHandle->prechargeRatioThreshold  = modPowerElectronicsGetPrechargeRatioThreshold();
	modPowerElectronicsPackStateHandle->prechargeMinimumVbat     = modPowerElectronicsGetPrechargeMinimumVbat();
	modPowerElectronicsPackStateHandle->prechargeVoltageRatio    = 0.0f;
	modPowerElectronicsPackStateHandle->prechargeVoltageDelta    = 0.0f;
	modPowerElectronicsPackStateHandle->prechargeMeasurementValid = false;
	modPowerElectronicsPackStateHandle->prechargeComplete        = false;
	modPowerElectronicsPackStateHandle->weldedContactorSuspect   = false;
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
	modPowerElectronicsMarkCellOpenWireUnavailable();
	modPowerElectronicsClearCellBalanceState();
	modPowerElectronicsPackStateHandle->temperatureReadoutValid  = false;
	modPowerElectronicsPackStateHandle->temperatureReadoutErrorCount = 0;
	modPowerElectronicsPackStateHandle->temperatureReadoutCount = BMS_TOTAL_TEMPS;
	modPowerElectronicsPackStateHandle->vBatReadoutValid         = false;
	modPowerElectronicsPackStateHandle->currentReadoutValid      = false;
	modPowerElectronicsPackStateHandle->vPackReadoutValid        = false;
	modPowerElectronicsPackStateHandle->powerMonitorReadoutValid = false;
	modPowerElectronicsPackStateHandle->powerMonitorReadoutErrorCount = 0;
	modPowerElectronicsPackStateHandle->packOperationalCellState = PACK_STATE_NORMAL;
	modPowerElectronicsPackStateHandle->temperatures[0]          = MOD_POWER_ELECTRONICS_INVALID_TEMPERATURE_C;
	modPowerElectronicsPackStateHandle->temperatures[1]          = MOD_POWER_ELECTRONICS_INVALID_TEMPERATURE_C;
	modPowerElectronicsPackStateHandle->temperatures[2]          = MOD_POWER_ELECTRONICS_INVALID_TEMPERATURE_C;
	modPowerElectronicsPackStateHandle->temperatures[3]          = MOD_POWER_ELECTRONICS_INVALID_TEMPERATURE_C;
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
	
	modPowerElectronicsChargeCurrentDetectionLastTick = HAL_GetTick();
	modPowerElectronicsBalanceModeActiveLastTick = HAL_GetTick();
	modPowerElectronicsOpenWireDiagnosticLastTick = HAL_GetTick();
};

bool modPowerElectronicsMeasurePowerOnce(void) {
	float measuredVBat = 0.0f;
	float measuredPackCurrent = 0.0f;
	float measuredVPack = 0.0f;
	bool vBatReadValid;
	bool currentReadValid;
	bool vPackReadValid;

	vBatReadValid = driverSWISL28022GetBusVoltage(ISL28022_MASTER_ADDRES,ISL28022_MASTER_BUS,&measuredVBat,0.004f);
	currentReadValid = driverSWISL28022GetBusCurrent(ISL28022_MASTER_ADDRES,ISL28022_MASTER_BUS,&measuredPackCurrent,modPowerElectronicsGeneralConfigHandle->shuntLCOffset,modPowerElectronicsGeneralConfigHandle->shuntLCFactor);
	vPackReadValid = driverHWADCGetVPackVoltage(&measuredVPack);

	if(vBatReadValid &&
	   modPowerElectronicsPackStateHandle->cellVoltageAverage > 0.0f &&
	   fabs(measuredVBat - modPowerElectronicsGeneralConfigHandle->noOfCells*modPowerElectronicsPackStateHandle->cellVoltageAverage) >= 1.0f) {
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
		modPowerElectronicsISLErrorCount = 0;
	}else{
		if(modPowerElectronicsISLErrorCount++ >= ISLErrorThreshold){
			modPowerElectronicsISLErrorCount = ISLErrorThreshold;
		}else{
			modPowerElectronicsInitISL();
		}
	}

	modPowerElectronicsPackStateHandle->powerMonitorReadoutErrorCount = modPowerElectronicsISLErrorCount;
	modPowerElectronicsUpdatePrechargeStatus();
	modPowerElectronicsPackStateHandle->packCurrent = modPowerElectronicsPackStateHandle->loCurrentLoadCurrent + modPowerElectronicsPackStateHandle->hiCurrentLoadCurrent;
	modPowerElectronicsPackStateHandle->packPower = modPowerElectronicsPackStateHandle->packCurrent * modPowerElectronicsPackStateHandle->packVoltage;
	modPowerElectronicsEvaluateFaults();

	return modPowerElectronicsPackStateHandle->powerMonitorReadoutValid;
}

bool modPowerElectronicsMeasureCellsOnce(void) {
	bool cellReadValid = modPowerElectronicsRefreshCellMeasurement(false);

	/* Keep the terminal one-shot path measurement-only: refresh cached cell data and
	 * fault visibility, but do not run voltage-watch permission updates, do not
	 * change desired/allowed output state, and do not touch balancing state.
	 */
	modPowerElectronicsEvaluateFaults();

	return cellReadValid;
}

bool modPowerElectronicsMeasureTempOnce(void) {
	driverLTC6812StatusTypedef tempChainStatus;
	bool tempReadValid = driverSWLTC6812ReadTemperatureVoltagesWithSensorEnable(
		modPowerElectronicsPackStateHandle->tempSensorVoltagesLTC6812,
		modPowerElectronicsRequiredTempChannelMask);

	tempChainStatus = driverSWLTC6812GetTemperatureChainStatus();
	modPowerElectronicsPackStateHandle->temperatureReadoutErrorCount = tempChainStatus.lastReadPECErrors;
	modPowerElectronicsUpdateTemperatureChainReadout(tempReadValid);
	modPowerElectronicsCalcTempStats();
	modPowerElectronicsEvaluateFaults();

	return tempReadValid;
}

bool modPowerElectronicsTask(void) {
	bool returnValue = false;
	
	if(modDelayTick1ms(&modPowerElectronicsMeasureIntervalLastTick,100)) {
		bool cellReadValid;
		bool tempReadValid;
		bool runOpenWireDiagnostic;
		driverLTC6812StatusTypedef tempChainStatus;

		// reset tick for LTC Temp start conversion delay
		modPowerElectronicsTempMeasureDelayLastTick = HAL_GetTick();
		modPowerElectronicsMeasurePowerOnce();

		cellReadValid = modPowerElectronicsRefreshCellMeasurement(true);

		runOpenWireDiagnostic = cellReadValid && modDelayTick1ms(&modPowerElectronicsOpenWireDiagnosticLastTick, MOD_POWER_ELECTRONICS_OPEN_WIRE_INTERVAL_MS);
		if(runOpenWireDiagnostic) {
			(void)driverSWLTC6812RunCellOpenWireDiagnostic();
			modPowerElectronicsUpdateCellOpenWireStatus(true);
		} else if(!cellReadValid) {
			modPowerElectronicsMarkCellOpenWireUnavailable();
		}

		tempReadValid = driverSWLTC6812ReadTemperatureVoltagesWithSensorEnable(
			modPowerElectronicsPackStateHandle->tempSensorVoltagesLTC6812,
			modPowerElectronicsRequiredTempChannelMask);
		tempChainStatus = driverSWLTC6812GetTemperatureChainStatus();
		modPowerElectronicsPackStateHandle->temperatureReadoutErrorCount = tempChainStatus.lastReadPECErrors;
		/* TODO(phase5): define the final shutdown/derate action for TEMP-chain comms faults.
		 * Phase 4 records validity/errors and keeps missing temperature coverage conservative.
		 */
		modPowerElectronicsUpdateTemperatureChainReadout(tempReadValid);

		/* The local STM32 NTC remains a board-local temperature only.
		 * Pack temperature coverage comes from the measurement-only TEMP chain; the
		 * local board NTC must not be substituted for missing pack coverage.
		 */
		driverHWADCGetNTCValue(&modPowerElectronicsPackStateHandle->temperatures[3],modPowerElectronicsGeneralConfigHandle->NTC25DegResistance[modConfigNTCGroupMasterPCB],modPowerElectronicsGeneralConfigHandle->NTCTopResistor[modConfigNTCGroupMasterPCB],modPowerElectronicsGeneralConfigHandle->NTCBetaFactor[modConfigNTCGroupMasterPCB],25.0f);
		
		// Calculate temperature statisticks
		modPowerElectronicsCalcTempStats();
		
		// When temperature and cellvoltages are known calculate charge and discharge throttle.
		modPowerElectronicsCalcThrottle();
		
		/* Phase 3 keeps balancing disabled until the LTC6812 cell-chain migration is complete.
		 * TODO(phase5): rework balancing to target the LTC6812 cell chain only.
		 */
		
		// Check and respond to the measured voltage values
		modPowerElectronicsSubTaskVoltageWatch();
		modPowerElectronicsEvaluateFaults();
		
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

		if(modPowerElectronicsCellBalancingShouldRun()) {
			modPowerElectronicsSubTaskBalaning();
		} else {
			modPowerElectronicsDisableCellBalancing();
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
	return modPowerElectronicsPackStateHandle->prechargeMeasurementValid &&
	       modPowerElectronicsPackStateHandle->prechargeComplete;
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
	modPowerElectronicsDisableCellBalancing();
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
	static uint16_t lastCellBalanceMaskPerDevice[BMS_LTC6812_DEVICES] = {0u};
	static bool delaytoggle = false;
	uint16_t balanceMaskPerDevice[BMS_LTC6812_DEVICES] = {0u};
	uint8_t activeCellCount = modPowerElectronicsGetActiveCellCount();
	modPowerElectronicsBalanceCandidateTypedef sortedCellArray[BMS_TOTAL_CELLS];
	bool maskChanged = false;
	
	if(modDelayTick1ms(&modPowerElectronicsCellBalanceUpdateLastTick,delayTimeHolder)) {
		delaytoggle ^= true;
		delayTimeHolder = delaytoggle ? modPowerElectronicsGeneralConfigHandle->cellBalanceUpdateInterval : 200;
		
		if(delaytoggle) {
			for(uint8_t cellIndex = 0u; cellIndex < activeCellCount; cellIndex++) {
				sortedCellArray[cellIndex].cellVoltage = modPowerElectronicsPackStateHandle->cellVoltagesLTC6812[cellIndex].cellVoltage;
				sortedCellArray[cellIndex].cellIndex = cellIndex;
			}
				
			modPowerElectronicsSortBalanceCandidates(sortedCellArray, activeCellCount);
			
			for(uint8_t i = 0u;
			    i < activeCellCount && i < modPowerElectronicsGeneralConfigHandle->maxSimultaneousDischargingCells;
			    i++) {
				if(sortedCellArray[i].cellVoltage >= (modPowerElectronicsPackStateHandle->cellVoltageLow + modPowerElectronicsGeneralConfigHandle->cellBalanceDifferenceThreshold) &&
				   sortedCellArray[i].cellVoltage >= modPowerElectronicsGeneralConfigHandle->cellBalanceStart) {
					uint8_t cellIndex = sortedCellArray[i].cellIndex;
					uint8_t deviceIndex = (uint8_t)(cellIndex / BMS_LTC6812_CELLS_PER_DEVICE);
					uint8_t cellIndexOnDevice = (uint8_t)(cellIndex % BMS_LTC6812_CELLS_PER_DEVICE);

					balanceMaskPerDevice[deviceIndex] |= (uint16_t)(1u << cellIndexOnDevice);
				}
			}
		}

		for(uint8_t deviceIndex = 0u; deviceIndex < BMS_LTC6812_DEVICES; deviceIndex++) {
			if(lastCellBalanceMaskPerDevice[deviceIndex] != balanceMaskPerDevice[deviceIndex]) {
				maskChanged = true;
				break;
			}
		}

		if(maskChanged) {
			if(driverSWLTC6812SetCellBalanceMask(balanceMaskPerDevice)) {
				driverLTC6812BalanceStatusTypedef balanceStatus = driverSWLTC6812GetCellBalanceStatus();
				modPowerElectronicsStoreCellBalanceMask(balanceStatus.balanceMaskPerDevice, true, 0u);
				memcpy(lastCellBalanceMaskPerDevice, balanceMaskPerDevice, sizeof(lastCellBalanceMaskPerDevice));
			} else {
				driverLTC6812BalanceStatusTypedef balanceStatus = driverSWLTC6812GetCellBalanceStatus();
				modPowerElectronicsClearCellBalanceState();
				modPowerElectronicsPackStateHandle->cellBalancingErrorCount =
					(uint8_t)(balanceStatus.lastErrorCount + balanceStatus.lastConfigPECErrors);
				memset(lastCellBalanceMaskPerDevice, 0, sizeof(lastCellBalanceMaskPerDevice));
			}
		} else {
			driverLTC6812BalanceStatusTypedef balanceStatus = driverSWLTC6812GetCellBalanceStatus();
			modPowerElectronicsStoreCellBalanceMask(
				lastCellBalanceMaskPerDevice,
				balanceStatus.lastConfigValid,
				(uint8_t)(balanceStatus.lastErrorCount + balanceStatus.lastConfigPECErrors));
		}
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
	uint32_t activeFaultMask;
	bool dischargePermissionAllowed = modPowerElectronicsPackStateHandle->disChargeDesired &&
	                                  (modPowerElectronicsPackStateHandle->disChargeLCAllowed || modPowerElectronicsAllowForcedOnState) &&
	                                  true;
	bool masterOkAllowed = modPowerElectronicsPackStateHandle->masterOkDesired &&
	                       true;
	bool chargePermissionAllowed = modPowerElectronicsPackStateHandle->chargeDesired &&
	                               modPowerElectronicsPackStateHandle->chargeAllowed &&
	                               true;
	bool chargerSafetyAllowed = modPowerElectronicsPackStateHandle->chargerSafetyDesired &&
	                            chargePermissionAllowed;

	modPowerElectronicsEvaluateFaults();
	activeFaultMask = modPowerElectronicsPackStateHandle->activeFaultMask;

	if((activeFaultMask & modPowerElectronicsGetDischargeBlockingFaultMask()) != 0u)
		dischargePermissionAllowed = false;

	if((activeFaultMask & modPowerElectronicsGetMasterOkBlockingFaultMask()) != 0u)
		masterOkAllowed = false;

	if((activeFaultMask & modPowerElectronicsGetChargeBlockingFaultMask()) != 0u)
		chargePermissionAllowed = false;

	chargerSafetyAllowed = modPowerElectronicsPackStateHandle->chargerSafetyDesired &&
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

uint32_t modPowerElectronicsGetActiveFaultMask(void) {
	return modPowerElectronicsPackStateHandle->activeFaultMask;
}

uint32_t modPowerElectronicsGetLatchedFaultMask(void) {
	return modPowerElectronicsPackStateHandle->latchedFaultMask;
}

uint8_t modPowerElectronicsGetUIFaultCode(void) {
	return modPowerElectronicsPackStateHandle->uiFaultCode;
}

bool modPowerElectronicsIsWeldedContactorSuspect(void) {
	return modPowerElectronicsWeldedContactorSuspect();
}
