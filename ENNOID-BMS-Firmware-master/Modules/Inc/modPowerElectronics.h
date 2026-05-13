#ifndef __MODPOWERELECTRONICS_H
#define __MODPOWERELECTRONICS_H

#include "driverSWISL28022.h"
#include "driverHWADC.h"
#include "driverSWLTC6803.h"
#include "driverSWLTC6812.h"
#include "driverHWSwitches.h"
#include "driverSWEMC2305.h"
#include "modDelay.h"
#include "modConfig.h"
#include "modPowerState.h"
#include "stdbool.h"
#include "math.h"

#define NoOfCellsPossibleOnChip	BMS_TOTAL_CELLS
#define NoOfTempSensors         13
#define PRECHARGE_PERCENTAGE 		0.75f
#define TotalLTCICs							BMS_LTC6812_DEVICES
#define ISLErrorThreshold       10

typedef enum {
	TEMP_EXT_LTC_NTC0 = 0,									// EXT on master BMS on LTC
	TEMP_EXT_LTC_NTC1,											// EXT on master BMS on LTC
	TEMP_INT_LTC_CHIP,											// Int on master BMS inside LTC Chip
	TEMP_INT_STM_NTC,												// Int on master BMS outside STM Chip
	TEMP_EXT_ADC_NTC0,											// Ext on slave BMS
	TEMP_EXT_ADC_NTC1,											// Ext on slave BMS
	TEMP_EXT_ADC_NTC2,											// Ext on slave BMS
	TEMP_EXT_ADC_NTC3,											// Ext on slave BMS
	TEMP_EXT_ADC_NTC4,											// Ext on slave BMS
	TEMP_EXT_ADC_NTC5,											// Ext on slave BMS
	TEMP_INT_ADC_NTC6,											// Int on slave BMS
	TEMP_INT_ADC_NTC7,											// Int on slave BMS
	TEMP_INT_SHT														// Int on slave BMS
} modPowerElectronicsTemperatureSensorMapping;

typedef enum {
	PACK_STATE_ERROR_HARD_CELLVOLTAGE = 0,
	PACK_STATE_ERROR_SOFT_CELLVOLTAGE,
	PACK_STATE_ERROR_OVER_CURRENT,
	PACK_STATE_NORMAL,
} modPowerElectronicsPackOperationalCellStatesTypedef;

typedef enum {
	BMS_FAULT_CELL_OV_SOFT = 0,
	BMS_FAULT_CELL_OV_HARD,
	BMS_FAULT_CELL_UV_SOFT,
	BMS_FAULT_CELL_UV_HARD,
	BMS_FAULT_CELL_READ_INVALID,
	BMS_FAULT_CELL_OPEN_WIRE,
	BMS_FAULT_TEMP_OVER_LIMIT,
	BMS_FAULT_TEMP_READ_INVALID,
	BMS_FAULT_TEMP_SENSOR_INVALID,
	BMS_FAULT_ISL_READ_INVALID,
	BMS_FAULT_VPACK_READ_INVALID,
	BMS_FAULT_PRECHARGE_TIMEOUT,
	BMS_FAULT_WELDED_CONTACTOR_SUSPECT,
	BMS_FAULT_INTERNAL_FATAL,
	BMS_FAULT_COUNT
} modPowerElectronicsFaultBitTypedef;

#define BMS_FAULT_MASK(bit) (1UL << (bit))

typedef struct {
	// Master BMS
	uint8_t  throttleDutyCharge;
	uint8_t  throttleDutyDischarge;
	float    SoC;
	float    SoCCapacityAh;
	OperationalStateTypedef operationalState;
	/* Battery-side Vbat from the ISL28022 on I2C2 (PA9/PA10). */
	float    packVoltage;
	float    vBatVoltage;
	float    packCurrent;
	float    packPower;
	/* Low-current path current from the ISL28022 shunt monitor. */
	float    loCurrentLoadCurrent;
	/* Load-side / precharge-bus Vpack from PA1 ADC. */
	float    loCurrentLoadVoltage;
	float    vPackVoltage;
	float    prechargeRatioThreshold;
	float    prechargeMinimumVbat;
	float    prechargeVoltageRatio;
	float    prechargeVoltageDelta;
	uint8_t  prechargeMeasurementValid;
	uint8_t  prechargeComplete;
	uint8_t  weldedContactorSuspect;
	float    cellVoltageHigh;
	float    cellVoltageLow;
	float    cellVoltageAverage;
	float    cellVoltageMisMatch;
	/* Deprecated 16-bit legacy compatibility surface only.
	 * Phase 13 balancing uses the 75-cell per-device masks below.
	 */
	uint16_t cellBalanceResistorEnableMask;
	uint16_t cellBalanceMaskPerDevice[BMS_LTC6812_DEVICES];
	uint8_t  cellBalanceFlags[BMS_TOTAL_CELLS];
	uint8_t  cellBalancingValid;
	uint8_t  cellBalancingErrorCount;
	uint8_t  cellBalancingActiveCount;
	float    temperatures[NoOfTempSensors];
	float    tempBatteryHigh;
	float    tempBatteryLow;
	float    tempBatteryAverage;
	float    tempBMSHigh;
	float    tempBMSLow;
	float    tempBMSAverage;
	uint8_t  masterOkDesired;
	uint8_t  preChargeDesired;
	uint8_t  disChargeDesired;
	uint8_t  disChargeLCAllowed;
	uint8_t  disChargeHCAllowed;
	uint8_t  chargeDesired;
	uint8_t  chargeAllowed;
	uint8_t  safetyOverCANHCSafeNSafe;
	uint8_t  chargerSafetyDesired;
	uint8_t  chargeCurrentDetected;
	uint8_t  chargeBalanceActive;
	uint8_t  powerButtonActuated;
	uint8_t  packInSOA;
	uint8_t  watchDogTime;
	uint8_t  cellVoltageReadoutValid;
	uint8_t  cellVoltageReadoutErrorCount;
	uint8_t  cellVoltageReadoutCount;
	uint8_t  cellOpenWireValid;
	uint8_t  cellOpenWireFaultCount;
	uint8_t  cellOpenWireDiagnosticErrorCount;
	uint32_t activeFaultMask;
	uint32_t latchedFaultMask;
	uint8_t  activeFaultCount;
	uint8_t  primaryFaultBit;
	uint8_t  uiFaultCode;
	uint8_t  temperatureReadoutValid;
	uint8_t  temperatureReadoutErrorCount;
	uint8_t  temperatureReadoutCount;
	uint8_t  vBatReadoutValid;
	uint8_t  currentReadoutValid;
	uint8_t  vPackReadoutValid;
	uint8_t  powerMonitorReadoutValid;
	uint8_t  powerMonitorReadoutErrorCount;
	driverLTC6803CellsTypedef cellVoltagesIndividual[NoOfCellsPossibleOnChip];
	driverLTC6812CellVoltageTypedef cellVoltagesLTC6812[BMS_TOTAL_CELLS];
	uint8_t  cellOpenWireFlags[BMS_TOTAL_CELLS];
	driverLTC6812AnalogVoltageTypedef tempSensorVoltagesLTC6812[BMS_TOTAL_TEMPS];
	float    temperaturesLTC6812[BMS_TOTAL_TEMPS];
	uint8_t  temperaturesLTC6812Valid[BMS_TOTAL_TEMPS];
	modPowerElectronicsPackOperationalCellStatesTypedef packOperationalCellState;
	
	// Slave BMS
	uint8_t  hiAmpShieldPresent;
	float    hiCurrentLoadVoltage;
	float    hiCurrentLoadCurrent;
	float		 hiCurrentLoadPower;
	float    auxVoltage;
	float    auxCurrent;
	float		 auxPower;
	uint8_t  aux0EnableDesired;
	uint8_t  aux0Enabled;
	uint8_t  aux0LoadIncorrect;
	uint8_t  aux1EnableDesired;
	uint8_t  aux1Enabled;
	uint8_t  aux1LoadIncorrect;
	uint8_t  auxDCDCEnabled;
	uint8_t  auxDCDCOutputOK;
	float    humidity;
	uint8_t  hiLoadEnabled;
	uint8_t  hiLoadPreChargeEnabled;
	uint8_t  hiLoadPreChargeError;
	uint8_t	 IOIN1;
	uint8_t  IOOUT0;
	uint8_t  FANSpeedDutyDesired;
	driverSWEMC2305FanStatusTypeDef FANStatus;
} modPowerElectricsPackStateTypedef;

void modPowerElectronicsInit(modPowerElectricsPackStateTypedef *packState, modConfigGeneralConfigStructTypedef *generalConfig);
bool modPowerElectronicsTask(void);
bool modPowerElectronicsMeasurePowerOnce(void);
bool modPowerElectronicsMeasureCellsOnce(void);
bool modPowerElectronicsMeasureTempOnce(void);
void modPowerElectronicsAllowForcedOn(bool allowedState);
void modPowerElectronicsSetMasterOk(bool allowed);
void modPowerElectronicsSetDischargePermission(bool allowed);
void modPowerElectronicsSetChargePermission(bool allowed);
void modPowerElectronicsSetChargerSafety(bool allowed);
bool modPowerElectronicsCanCloseDischargePath(void);
/* Legacy compatibility wrappers only.
 * Do not use these in new code: they preserve removed precharge/discharge
 * relay terminology and exist only to keep the migration reviewable.
 */
void modPowerElectronicsSetPreCharge(bool newState);
bool modPowerElectronicsSetDisCharge(bool newState);
void modPowerElectronicsSetCharge(bool newState);
void modPowerElectronicsDisableAll(void);
void modPowerElectronicsCalculateCellStats(void);
void modPowerElectronicsSubTaskBalaning(void);
void modPowerElectronicsSubTaskVoltageWatch(void);
void modPowerElectronicsUpdateSwitches(void);
void modPowerElectronicsSortCells(driverLTC6803CellsTypedef *cells, uint8_t cellCount);
void modPowerElectronicsCalcTempStats(void);
void modPowerElectronicsCalcThrottle(void);
uint32_t modPowerElectronicsGetActiveFaultMask(void);
uint32_t modPowerElectronicsGetLatchedFaultMask(void);
uint8_t modPowerElectronicsGetUIFaultCode(void);
bool modPowerElectronicsIsWeldedContactorSuspect(void);
int32_t modPowerElectronicsMapVariableInt(int32_t inputVariable, int32_t inputLowerLimit, int32_t inputUpperLimit, int32_t outputLowerLimit, int32_t outputUpperLimit);
float modPowerElectronicsMapVariableFloat(float inputVariable, float inputLowerLimit, float inputUpperLimit, float outputLowerLimit, float outputUpperLimit);
void modPowerElectronicsInitISL(void);
bool modPowerElectronicsHCSafetyCANAndPowerButtonCheck(void);
void modPowerElectronicsResetBalanceModeActiveTimeout(void);

#endif
