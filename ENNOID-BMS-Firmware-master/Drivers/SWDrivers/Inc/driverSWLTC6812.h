#ifndef DRIVERS_SWDRIVERS_INC_DRIVERSWLTC6812_H_
#define DRIVERS_SWDRIVERS_INC_DRIVERSWLTC6812_H_

#include "driverHWSPI1.h"
#include "stdbool.h"
#include "stdint.h"

#define BMS_LTC6812_DEVICES                   5u
#define BMS_LTC6812_CELLS_PER_DEVICE          15u
#define BMS_TOTAL_CELLS                       (BMS_LTC6812_DEVICES * BMS_LTC6812_CELLS_PER_DEVICE)
#define BMS_TOTAL_TEMPS                       75u
#define BMS_LTC6812_CELL_REGISTER_GROUPS      5u
#define BMS_LTC6812_CELLS_PER_REGISTER_GROUP  3u

typedef struct {
	uint16_t rawCode;
	uint16_t milliVolts;
	float    cellVoltage;
	uint8_t  cellNumber;
	uint8_t  deviceIndex;
	uint8_t  cellIndexOnDevice;
} driverLTC6812CellVoltageTypedef;

typedef struct {
	uint16_t rawCode;
	uint16_t milliVolts;
	float    sensorVoltage;
	uint8_t  channelNumber;
	uint8_t  deviceIndex;
	uint8_t  channelIndexOnDevice;
	bool     valid;
} driverLTC6812AnalogVoltageTypedef;

typedef struct {
	uint8_t lastReadPECErrors;
	bool    lastReadValid;
} driverLTC6812StatusTypedef;

void driverSWLTC6812Init(void);
void driverSWLTC6812WakeupCellChain(void);
void driverSWLTC6812WakeupTempChain(void);
bool driverSWLTC6812StartCellVoltageConversion(void);
bool driverSWLTC6812StartTemperatureVoltageConversion(void);
bool driverSWLTC6812ReadCellVoltages(driverLTC6812CellVoltageTypedef cellVoltages[BMS_TOTAL_CELLS]);
bool driverSWLTC6812ReadTemperatureVoltages(driverLTC6812AnalogVoltageTypedef sensorVoltages[BMS_TOTAL_TEMPS]);
bool driverSWLTC6812SetTempSensorEnableMask(uint16_t enableMaskPerDevice[BMS_LTC6812_DEVICES]);
bool driverSWLTC6812DisableTempSensorEnables(void);
bool driverSWLTC6812ReadTemperatureVoltagesWithSensorEnable(
	driverLTC6812AnalogVoltageTypedef sensorVoltages[BMS_TOTAL_TEMPS],
	uint16_t enableMaskPerDevice[BMS_LTC6812_DEVICES]);
driverLTC6812StatusTypedef driverSWLTC6812GetCellChainStatus(void);
driverLTC6812StatusTypedef driverSWLTC6812GetTemperatureChainStatus(void);
uint16_t driverSWLTC6812CalculatePEC15(const uint8_t *data, uint16_t length);

#endif
