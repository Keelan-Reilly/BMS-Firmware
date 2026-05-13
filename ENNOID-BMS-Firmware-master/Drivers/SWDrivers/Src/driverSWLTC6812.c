#include "driverSWLTC6812.h"
#include "string.h"

#define DRIVER_LTC6812_PEC15_POLY          0x4599u
#define DRIVER_LTC6812_PEC15_SEED          16u
#define DRIVER_LTC6812_BYTES_PER_REGISTER  6u
#define DRIVER_LTC6812_BYTES_PER_DEVICE    8u
#define DRIVER_LTC6812_WAKEUP_BYTES        2u

typedef enum {
	DRIVER_LTC6812_CMD_RDCVA = 0x0004u,
	DRIVER_LTC6812_CMD_RDCVB = 0x0006u,
	DRIVER_LTC6812_CMD_RDCVC = 0x0008u,
	DRIVER_LTC6812_CMD_RDCVD = 0x000Au,
	DRIVER_LTC6812_CMD_RDCVE = 0x0009u
} driverLTC6812CommandCodeTypedef;

typedef enum {
	DRIVER_LTC6812_ADC_MODE_422HZ = 0u,
	DRIVER_LTC6812_ADC_MODE_FAST = 1u,
	DRIVER_LTC6812_ADC_MODE_NORMAL = 2u,
	DRIVER_LTC6812_ADC_MODE_FILTERED = 3u
} driverLTC6812ADCModeTypedef;

typedef enum {
	DRIVER_LTC6812_CELL_SELECTION_ALL = 0u
} driverLTC6812CellSelectionTypedef;

static driverLTC6812StatusTypedef driverSWLTC6812CellChainStatus;
static driverLTC6812StatusTypedef driverSWLTC6812TempChainStatus;

static uint16_t driverSWLTC6812BuildADCVCommand(driverLTC6812ADCModeTypedef adcMode, bool dischargePermitted, driverLTC6812CellSelectionTypedef cellSelection) {
	/* LTC6812-1 data sheet Rev. B, Table 37:
	 * ADCV = 0 | 1 | MD[1] | MD[0] | 1 | 1 | DCP | 0 | CH[2] | CH[1] | CH[0]
	 */
	return (uint16_t)((1u << 9) |
	                  ((uint16_t)adcMode << 7) |
	                  (3u << 5) |
	                  ((uint16_t)(dischargePermitted ? 1u : 0u) << 4) |
	                  (uint16_t)cellSelection);
}

static void driverSWLTC6812EncodeCommand(uint16_t commandCode, uint8_t commandBytes[4]) {
	commandBytes[0] = (uint8_t)((commandCode >> 8) & 0xFFu);
	commandBytes[1] = (uint8_t)(commandCode & 0xFFu);

	{
		uint16_t commandPEC = driverSWLTC6812CalculatePEC15(commandBytes, 2u);
		commandBytes[2] = (uint8_t)((commandPEC >> 8) & 0xFFu);
		commandBytes[3] = (uint8_t)(commandPEC & 0xFFu);
	}
}

static void driverSWLTC6812WakeupChain(BMS_IsoSpiChain_t chain) {
	uint8_t wakeupBytes[DRIVER_LTC6812_WAKEUP_BYTES] = {0xFFu, 0xFFu};

	/* The LTC6812 daisy chain requires isoSPI activity to wake sleeping devices.
	 * The TEMP chain is measurement-only in this firmware migration: no balancing,
	 * configuration, or discharge writes are routed to BMS_ISOSPI_CHAIN_TEMP.
	 */
	(void)driverHWIsoSpiWrite(chain, wakeupBytes, DRIVER_LTC6812_WAKEUP_BYTES);
}

static bool driverSWLTC6812StartVoltageConversionForChain(BMS_IsoSpiChain_t chain) {
	uint8_t commandBytes[4];
	uint16_t commandCode = driverSWLTC6812BuildADCVCommand(DRIVER_LTC6812_ADC_MODE_NORMAL, false, DRIVER_LTC6812_CELL_SELECTION_ALL);

	/* ADCV is used here only to sample analogue voltage channels. TEMP-chain balancing
	 * and configuration writes are intentionally not implemented in this phase.
	 */
	driverSWLTC6812WakeupChain(chain);
	driverSWLTC6812EncodeCommand(commandCode, commandBytes);

	return driverHWIsoSpiWrite(chain, commandBytes, sizeof(commandBytes));
}

static bool driverSWLTC6812ReadVoltageRegistersForChain(BMS_IsoSpiChain_t chain, driverLTC6812StatusTypedef *chainStatus, driverLTC6812AnalogVoltageTypedef sensorVoltages[BMS_TOTAL_TEMPS]) {
	static const driverLTC6812CommandCodeTypedef cellRegisterCommands[BMS_LTC6812_CELL_REGISTER_GROUPS] = {
		DRIVER_LTC6812_CMD_RDCVA,
		DRIVER_LTC6812_CMD_RDCVB,
		DRIVER_LTC6812_CMD_RDCVC,
		DRIVER_LTC6812_CMD_RDCVD,
		DRIVER_LTC6812_CMD_RDCVE
	};
	uint8_t commandBytes[4];
	uint8_t readBytes[BMS_LTC6812_DEVICES * DRIVER_LTC6812_BYTES_PER_DEVICE];
	uint8_t pecErrorCount = 0u;
	bool readValid = true;

	driverSWLTC6812WakeupChain(chain);

	for(uint8_t groupIndex = 0u; groupIndex < BMS_LTC6812_CELL_REGISTER_GROUPS; groupIndex++) {
		driverSWLTC6812EncodeCommand((uint16_t)cellRegisterCommands[groupIndex], commandBytes);

		if(!driverHWIsoSpiWriteRead(chain, commandBytes, sizeof(commandBytes), readBytes, sizeof(readBytes))) {
			readValid = false;
			break;
		}

		for(uint8_t deviceIndex = 0u; deviceIndex < BMS_LTC6812_DEVICES; deviceIndex++) {
			uint8_t deviceBaseIndex = (uint8_t)(deviceIndex * DRIVER_LTC6812_BYTES_PER_DEVICE);
			uint16_t receivedPEC = (uint16_t)((readBytes[deviceBaseIndex + 6u] << 8) | readBytes[deviceBaseIndex + 7u]);
			uint16_t calculatedPEC = driverSWLTC6812CalculatePEC15(&readBytes[deviceBaseIndex], DRIVER_LTC6812_BYTES_PER_REGISTER);

			if(receivedPEC != calculatedPEC) {
				pecErrorCount++;
				readValid = false;
			}

			for(uint8_t cellInGroup = 0u; cellInGroup < BMS_LTC6812_CELLS_PER_REGISTER_GROUP; cellInGroup++) {
				uint8_t dataIndex = (uint8_t)(deviceBaseIndex + (cellInGroup * 2u));
				uint8_t channelIndexOnDevice = (uint8_t)((groupIndex * BMS_LTC6812_CELLS_PER_REGISTER_GROUP) + cellInGroup);
				uint8_t flatChannelIndex = (uint8_t)((deviceIndex * BMS_LTC6812_CELLS_PER_DEVICE) + channelIndexOnDevice);
				uint16_t rawCode = (uint16_t)(readBytes[dataIndex] | (readBytes[dataIndex + 1u] << 8));

				sensorVoltages[flatChannelIndex].rawCode = rawCode;
				sensorVoltages[flatChannelIndex].milliVolts = (uint16_t)(rawCode / 10u);
				sensorVoltages[flatChannelIndex].sensorVoltage = (float)rawCode * 0.0001f;
				sensorVoltages[flatChannelIndex].channelNumber = flatChannelIndex;
				sensorVoltages[flatChannelIndex].deviceIndex = deviceIndex;
				sensorVoltages[flatChannelIndex].channelIndexOnDevice = channelIndexOnDevice;
				sensorVoltages[flatChannelIndex].valid = readValid;
			}
		}
	}

	chainStatus->lastReadPECErrors = pecErrorCount;
	chainStatus->lastReadValid = readValid;

	return readValid;
}

void driverSWLTC6812Init(void) {
	driverSWLTC6812CellChainStatus.lastReadPECErrors = 0u;
	driverSWLTC6812CellChainStatus.lastReadValid = false;
	driverSWLTC6812TempChainStatus.lastReadPECErrors = 0u;
	driverSWLTC6812TempChainStatus.lastReadValid = false;
}

void driverSWLTC6812WakeupCellChain(void) {
	driverSWLTC6812WakeupChain(BMS_ISOSPI_CHAIN_CELL);
}

void driverSWLTC6812WakeupTempChain(void) {
	driverSWLTC6812WakeupChain(BMS_ISOSPI_CHAIN_TEMP);
}

bool driverSWLTC6812StartCellVoltageConversion(void) {
	return driverSWLTC6812StartVoltageConversionForChain(BMS_ISOSPI_CHAIN_CELL);
}

bool driverSWLTC6812StartTemperatureVoltageConversion(void) {
	return driverSWLTC6812StartVoltageConversionForChain(BMS_ISOSPI_CHAIN_TEMP);
}

bool driverSWLTC6812ReadCellVoltages(driverLTC6812CellVoltageTypedef cellVoltages[BMS_TOTAL_CELLS]) {
	driverLTC6812AnalogVoltageTypedef analogVoltages[BMS_TOTAL_CELLS];
	bool readValid = driverSWLTC6812ReadVoltageRegistersForChain(BMS_ISOSPI_CHAIN_CELL, &driverSWLTC6812CellChainStatus, analogVoltages);

	for(uint8_t cellIndex = 0u; cellIndex < BMS_TOTAL_CELLS; cellIndex++) {
		cellVoltages[cellIndex].rawCode = analogVoltages[cellIndex].rawCode;
		cellVoltages[cellIndex].milliVolts = analogVoltages[cellIndex].milliVolts;
		cellVoltages[cellIndex].cellVoltage = analogVoltages[cellIndex].sensorVoltage;
		cellVoltages[cellIndex].cellNumber = analogVoltages[cellIndex].channelNumber;
		cellVoltages[cellIndex].deviceIndex = analogVoltages[cellIndex].deviceIndex;
		cellVoltages[cellIndex].cellIndexOnDevice = analogVoltages[cellIndex].channelIndexOnDevice;
	}

	return readValid;
}

bool driverSWLTC6812ReadTemperatureVoltages(driverLTC6812AnalogVoltageTypedef sensorVoltages[BMS_TOTAL_TEMPS]) {
	return driverSWLTC6812ReadVoltageRegistersForChain(BMS_ISOSPI_CHAIN_TEMP, &driverSWLTC6812TempChainStatus, sensorVoltages);
}

driverLTC6812StatusTypedef driverSWLTC6812GetCellChainStatus(void) {
	return driverSWLTC6812CellChainStatus;
}

driverLTC6812StatusTypedef driverSWLTC6812GetTemperatureChainStatus(void) {
	return driverSWLTC6812TempChainStatus;
}

uint16_t driverSWLTC6812CalculatePEC15(const uint8_t *data, uint16_t length) {
	uint16_t remainder = DRIVER_LTC6812_PEC15_SEED;

	for(uint16_t byteIndex = 0u; byteIndex < length; byteIndex++) {
		remainder ^= (uint16_t)data[byteIndex] << 7;

		for(uint8_t bitIndex = 0u; bitIndex < 8u; bitIndex++) {
			if((remainder & 0x4000u) != 0u) {
				remainder = (uint16_t)((remainder << 1) ^ DRIVER_LTC6812_PEC15_POLY);
			} else {
				remainder = (uint16_t)(remainder << 1);
			}
		}
	}

	return (uint16_t)((remainder << 1) & 0xFFFFu);
}
