#include "driverSWLTC6812.h"
#include "string.h"

/* LTC6812-1 Rev. B, "Packet Error Code" (pp. 52-54):
 * x^15 + x^14 + x^10 + x^8 + x^7 + x^4 + x^3 + 1, seeded with 0x0010,
 * then a zero bit is appended at the LSB of the returned 16-bit PEC field.
 */
#define DRIVER_LTC6812_PEC15_POLY          0x4599u
#define DRIVER_LTC6812_PEC15_SEED          16u
#define DRIVER_LTC6812_BYTES_PER_REGISTER  6u
#define DRIVER_LTC6812_BYTES_PER_DEVICE    8u
#define DRIVER_LTC6812_WAKEUP_BYTES        2u
#define DRIVER_LTC6812_CONFIG_GROUPS       2u
#define DRIVER_LTC6812_TEMP_SETTLE_DELAY_MS 1u
#define DRIVER_LTC6812_TEMP_ADCV_DELAY_MS   3u
#define DRIVER_LTC6812_OPEN_WIRE_PASSES     2u
#define DRIVER_LTC6812_OPEN_WIRE_DELAY_MS   3u
#define DRIVER_LTC6812_OPEN_WIRE_THRESHOLD_MV (-400)

typedef enum {
	/* LTC6812-1 Rev. B command table: configuration register access and
	 * cell-voltage register groups A-E.
	 */
	DRIVER_LTC6812_CMD_WRCFGA = 0x0001u,
	DRIVER_LTC6812_CMD_RDCFGA = 0x0002u,
	DRIVER_LTC6812_CMD_RDCVA = 0x0004u,
	DRIVER_LTC6812_CMD_RDCVB = 0x0006u,
	DRIVER_LTC6812_CMD_RDCVC = 0x0008u,
	DRIVER_LTC6812_CMD_RDCVD = 0x000Au,
	DRIVER_LTC6812_CMD_RDCVE = 0x0009u,
	DRIVER_LTC6812_CMD_WRCFGB = 0x0024u,
	DRIVER_LTC6812_CMD_RDCFGB = 0x0026u
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
static driverLTC6812BalanceStatusTypedef driverSWLTC6812CellBalanceStatus;
static driverLTC6812OpenWireStatusTypedef driverSWLTC6812CellOpenWireStatus;

static void driverSWLTC6812EncodeCommand(uint16_t commandCode, uint8_t commandBytes[4]);
static void driverSWLTC6812WakeupChain(BMS_IsoSpiChain_t chain);
static bool driverSWLTC6812ReadVoltageRegistersForChain(
	BMS_IsoSpiChain_t chain,
	driverLTC6812StatusTypedef *chainStatus,
	driverLTC6812AnalogVoltageTypedef sensorVoltages[BMS_TOTAL_TEMPS]);
static bool driverSWLTC6812ReadConfigRegistersForChain(
	BMS_IsoSpiChain_t chain,
	uint8_t configA[BMS_LTC6812_DEVICES][DRIVER_LTC6812_BYTES_PER_REGISTER],
	uint8_t configB[BMS_LTC6812_DEVICES][DRIVER_LTC6812_BYTES_PER_REGISTER],
	uint8_t *pecErrorCount);
static bool driverSWLTC6812WriteConfigRegistersForChain(
	BMS_IsoSpiChain_t chain,
	const uint8_t configA[BMS_LTC6812_DEVICES][DRIVER_LTC6812_BYTES_PER_REGISTER],
	const uint8_t configB[BMS_LTC6812_DEVICES][DRIVER_LTC6812_BYTES_PER_REGISTER]);

static void driverSWLTC6812ClearTempEnableBits(
	uint8_t configA[BMS_LTC6812_DEVICES][DRIVER_LTC6812_BYTES_PER_REGISTER],
	uint8_t configB[BMS_LTC6812_DEVICES][DRIVER_LTC6812_BYTES_PER_REGISTER]) {
	for(uint8_t deviceIndex = 0u; deviceIndex < BMS_LTC6812_DEVICES; deviceIndex++) {
		/* LTC6812-1 Rev. B configuration register map:
		 * DCC1-DCC8 live in CFGA byte 4 bits 0-7, DCC9-DCC12 in CFGA byte 5 bits 0-3,
		 * and DCC13-DCC15 in CFGB byte 0 bits 4-6 for the 15-cell device.
		 */
		configA[deviceIndex][4] &= 0x00u;
		configA[deviceIndex][5] &= 0xF0u;
		configB[deviceIndex][0] &= 0x8Fu;
		configB[deviceIndex][1] &= 0xFBu;
	}
}

static void driverSWLTC6812ApplyTempEnableMask(
	const uint16_t enableMaskPerDevice[BMS_LTC6812_DEVICES],
	uint8_t configA[BMS_LTC6812_DEVICES][DRIVER_LTC6812_BYTES_PER_REGISTER],
	uint8_t configB[BMS_LTC6812_DEVICES][DRIVER_LTC6812_BYTES_PER_REGISTER]) {
	driverSWLTC6812ClearTempEnableBits(configA, configB);

	for(uint8_t deviceIndex = 0u; deviceIndex < BMS_LTC6812_DEVICES; deviceIndex++) {
		for(uint8_t channelIndex = 0u; channelIndex < BMS_LTC6812_CELLS_PER_DEVICE; channelIndex++) {
			if((enableMaskPerDevice[deviceIndex] & (1u << channelIndex)) == 0u)
				continue;

			if(channelIndex < 8u) {
				configA[deviceIndex][4] |= (uint8_t)(1u << channelIndex);
			} else if(channelIndex < 12u) {
				configA[deviceIndex][5] |= (uint8_t)(1u << (channelIndex - 8u));
			} else {
				configB[deviceIndex][0] |= (uint8_t)(1u << (channelIndex - 8u));
			}
		}
	}
}

static bool driverSWLTC6812TempEnableMaskMatches(
	const uint16_t enableMaskPerDevice[BMS_LTC6812_DEVICES],
	const uint8_t configA[BMS_LTC6812_DEVICES][DRIVER_LTC6812_BYTES_PER_REGISTER],
	const uint8_t configB[BMS_LTC6812_DEVICES][DRIVER_LTC6812_BYTES_PER_REGISTER]) {
	for(uint8_t deviceIndex = 0u; deviceIndex < BMS_LTC6812_DEVICES; deviceIndex++) {
		uint16_t readMask = 0u;

		readMask |= configA[deviceIndex][4];
		readMask |= (uint16_t)(configA[deviceIndex][5] & 0x0Fu) << 8;
		readMask |= (uint16_t)((configB[deviceIndex][0] >> 4) & 0x07u) << 12;

		if(readMask != enableMaskPerDevice[deviceIndex])
			return false;
	}

	return true;
}

static void driverSWLTC6812ClearCellBalanceBits(
	uint8_t configA[BMS_LTC6812_DEVICES][DRIVER_LTC6812_BYTES_PER_REGISTER],
	uint8_t configB[BMS_LTC6812_DEVICES][DRIVER_LTC6812_BYTES_PER_REGISTER]) {
	for(uint8_t deviceIndex = 0u; deviceIndex < BMS_LTC6812_DEVICES; deviceIndex++) {
		/* LTC6812-1 Rev. B configuration register map:
		 * DCC1-DCC8 live in CFGA byte 4 bits 0-7, DCC9-DCC12 in CFGA byte 5 bits 0-3,
		 * and DCC13-DCC15 in CFGB byte 0 bits 4-6 for the 15-cell device.
		 * Keep this helper CELL-chain specific so TEMP sensor-bias control cannot
		 * accidentally route balancing writes to BMS_ISOSPI_CHAIN_TEMP.
		 */
		configA[deviceIndex][4] = 0x00u;
		configA[deviceIndex][5] &= 0xF0u;
		configB[deviceIndex][0] &= 0x8Fu;
	}
}

static void driverSWLTC6812ApplyCellBalanceMask(
	const uint16_t balanceMaskPerDevice[BMS_LTC6812_DEVICES],
	uint8_t configA[BMS_LTC6812_DEVICES][DRIVER_LTC6812_BYTES_PER_REGISTER],
	uint8_t configB[BMS_LTC6812_DEVICES][DRIVER_LTC6812_BYTES_PER_REGISTER]) {
	driverSWLTC6812ClearCellBalanceBits(configA, configB);

	for(uint8_t deviceIndex = 0u; deviceIndex < BMS_LTC6812_DEVICES; deviceIndex++) {
		for(uint8_t cellIndex = 0u; cellIndex < BMS_LTC6812_CELLS_PER_DEVICE; cellIndex++) {
			if((balanceMaskPerDevice[deviceIndex] & (1u << cellIndex)) == 0u)
				continue;

			if(cellIndex < 8u) {
				configA[deviceIndex][4] |= (uint8_t)(1u << cellIndex);
			} else if(cellIndex < 12u) {
				configA[deviceIndex][5] |= (uint8_t)(1u << (cellIndex - 8u));
			} else {
				configB[deviceIndex][0] |= (uint8_t)(1u << (cellIndex - 8u));
			}
		}
	}
}

static bool driverSWLTC6812CellBalanceMaskMatches(
	const uint16_t balanceMaskPerDevice[BMS_LTC6812_DEVICES],
	const uint8_t configA[BMS_LTC6812_DEVICES][DRIVER_LTC6812_BYTES_PER_REGISTER],
	const uint8_t configB[BMS_LTC6812_DEVICES][DRIVER_LTC6812_BYTES_PER_REGISTER]) {
	for(uint8_t deviceIndex = 0u; deviceIndex < BMS_LTC6812_DEVICES; deviceIndex++) {
		uint16_t readMask = 0u;

		readMask |= configA[deviceIndex][4];
		readMask |= (uint16_t)(configA[deviceIndex][5] & 0x0Fu) << 8;
		readMask |= (uint16_t)((configB[deviceIndex][0] >> 4) & 0x07u) << 12;

		if(readMask != balanceMaskPerDevice[deviceIndex])
			return false;
	}

	return true;
}

static uint8_t driverSWLTC6812CountMaskBits(const uint16_t maskPerDevice[BMS_LTC6812_DEVICES]) {
	uint8_t activeCount = 0u;

	for(uint8_t deviceIndex = 0u; deviceIndex < BMS_LTC6812_DEVICES; deviceIndex++) {
		for(uint8_t cellIndex = 0u; cellIndex < BMS_LTC6812_CELLS_PER_DEVICE; cellIndex++) {
			if((maskPerDevice[deviceIndex] & (1u << cellIndex)) != 0u)
				activeCount++;
		}
	}

	return activeCount;
}

static void driverSWLTC6812ClearCellBalanceStatus(void) {
	memset(&driverSWLTC6812CellBalanceStatus, 0, sizeof(driverSWLTC6812CellBalanceStatus));
	driverSWLTC6812CellBalanceStatus.lastConfigValid = false;
}

static void driverSWLTC6812StoreCellBalanceStatus(
	const uint16_t balanceMaskPerDevice[BMS_LTC6812_DEVICES],
	uint8_t pecErrorCount,
	bool configValid) {
	for(uint8_t deviceIndex = 0u; deviceIndex < BMS_LTC6812_DEVICES; deviceIndex++) {
		driverSWLTC6812CellBalanceStatus.balanceMaskPerDevice[deviceIndex] = balanceMaskPerDevice[deviceIndex];
	}

	driverSWLTC6812CellBalanceStatus.lastConfigPECErrors = pecErrorCount;
	driverSWLTC6812CellBalanceStatus.activeCellCount = driverSWLTC6812CountMaskBits(balanceMaskPerDevice);
	driverSWLTC6812CellBalanceStatus.lastConfigValid = configValid;
}

static void driverSWLTC6812RecordCellBalanceFailure(uint8_t additionalErrorCount) {
	driverSWLTC6812CellBalanceStatus.lastConfigValid = false;
	driverSWLTC6812CellBalanceStatus.lastErrorCount =
		(uint8_t)(driverSWLTC6812CellBalanceStatus.lastErrorCount + additionalErrorCount);
}

static bool driverSWLTC6812SetCellBalanceMaskInternal(
	const uint16_t balanceMaskPerDevice[BMS_LTC6812_DEVICES],
	bool allowDisableRecovery);

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

static uint16_t driverSWLTC6812BuildADOWCommand(
	driverLTC6812ADCModeTypedef adcMode,
	bool pullUpCurrent,
	bool dischargePermitted,
	driverLTC6812CellSelectionTypedef cellSelection) {
	/* LTC6812-1 Rev. B, command table and "Open Wire Check (ADOW Command)":
	 * ADOW = 0 | 1 | MD[1] | MD[0] | PUP | 1 | DCP | 1 | CH[2] | CH[1] | CH[0]
	 */
	return (uint16_t)((1u << 9) |
	                  ((uint16_t)adcMode << 7) |
	                  ((uint16_t)(pullUpCurrent ? 1u : 0u) << 6) |
	                  (1u << 5) |
	                  ((uint16_t)(dischargePermitted ? 1u : 0u) << 4) |
	                  (1u << 3) |
	                  (uint16_t)cellSelection);
}

static void driverSWLTC6812ConvertAnalogToCellVoltages(
	const driverLTC6812AnalogVoltageTypedef analogVoltages[BMS_TOTAL_CELLS],
	driverLTC6812CellVoltageTypedef cellVoltages[BMS_TOTAL_CELLS]) {
	for(uint8_t cellIndex = 0u; cellIndex < BMS_TOTAL_CELLS; cellIndex++) {
		cellVoltages[cellIndex].rawCode = analogVoltages[cellIndex].rawCode;
		cellVoltages[cellIndex].milliVolts = analogVoltages[cellIndex].milliVolts;
		cellVoltages[cellIndex].cellVoltage = analogVoltages[cellIndex].sensorVoltage;
		cellVoltages[cellIndex].cellNumber = analogVoltages[cellIndex].channelNumber;
		cellVoltages[cellIndex].deviceIndex = analogVoltages[cellIndex].deviceIndex;
		cellVoltages[cellIndex].cellIndexOnDevice = analogVoltages[cellIndex].channelIndexOnDevice;
	}
}

static void driverSWLTC6812ClearCellOpenWireStatus(void) {
	memset(&driverSWLTC6812CellOpenWireStatus, 0, sizeof(driverSWLTC6812CellOpenWireStatus));
	driverSWLTC6812CellOpenWireStatus.lastDiagnosticValid = false;
}

static void driverSWLTC6812FlagOpenWireCell(uint8_t cellIndex) {
	if(driverSWLTC6812CellOpenWireStatus.openWireFlags[cellIndex])
		return;

	driverSWLTC6812CellOpenWireStatus.openWireFlags[cellIndex] = true;
	driverSWLTC6812CellOpenWireStatus.openWireFaultCount++;
}

static bool driverSWLTC6812RunCellOpenWirePass(
	bool pullUpCurrent,
	driverLTC6812CellVoltageTypedef cellVoltages[BMS_TOTAL_CELLS],
	uint8_t *pecErrorCount) {
	driverLTC6812AnalogVoltageTypedef analogVoltages[BMS_TOTAL_CELLS];
	driverLTC6812StatusTypedef passStatus = {0u};
	uint8_t commandBytes[4];
	uint16_t commandCode = driverSWLTC6812BuildADOWCommand(
		DRIVER_LTC6812_ADC_MODE_NORMAL,
		pullUpCurrent,
		false,
		DRIVER_LTC6812_CELL_SELECTION_ALL);

	/* LTC6812-1 Rev. B "Open Wire Check (ADOW Command)":
	 * run the 15-cell ADOW command at least twice for PUP=1 and PUP=0, then
	 * read the cell voltages once at the end of each direction.
	 */
	driverSWLTC6812EncodeCommand(commandCode, commandBytes);

	for(uint8_t passIndex = 0u; passIndex < DRIVER_LTC6812_OPEN_WIRE_PASSES; passIndex++) {
		driverSWLTC6812WakeupChain(BMS_ISOSPI_CHAIN_CELL);
		if(!driverHWIsoSpiWrite(BMS_ISOSPI_CHAIN_CELL, commandBytes, sizeof(commandBytes)))
			return false;

		/* TODO(phase12): tighten this delay if bench timing proves a better board-
		 * specific value. The datasheet algorithm is implemented conservatively in
		 * normal mode for the <=10nF case from Table 14.
		 */
		HAL_Delay(DRIVER_LTC6812_OPEN_WIRE_DELAY_MS);
	}

	if(!driverSWLTC6812ReadVoltageRegistersForChain(BMS_ISOSPI_CHAIN_CELL, &passStatus, analogVoltages)) {
		*pecErrorCount = (uint8_t)(*pecErrorCount + passStatus.lastReadPECErrors);
		return false;
	}

	*pecErrorCount = (uint8_t)(*pecErrorCount + passStatus.lastReadPECErrors);
	driverSWLTC6812ConvertAnalogToCellVoltages(analogVoltages, cellVoltages);
	return passStatus.lastReadValid;
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
	 * The TEMP chain is measurement-only in this firmware migration. Its S outputs
	 * are reserved only for temporary sensor-bias enables, never for cell balancing.
	 */
	(void)driverHWIsoSpiWrite(chain, wakeupBytes, DRIVER_LTC6812_WAKEUP_BYTES);
}

static bool driverSWLTC6812StartVoltageConversionForChain(BMS_IsoSpiChain_t chain) {
	uint8_t commandBytes[4];
	uint16_t commandCode = driverSWLTC6812BuildADCVCommand(DRIVER_LTC6812_ADC_MODE_NORMAL, false, DRIVER_LTC6812_CELL_SELECTION_ALL);

	/* ADCV is used here only to sample analogue voltage channels. TEMP-chain sensor
	 * bias control via S outputs is not implemented here yet, and no balancing path
	 * is allowed to target BMS_ISOSPI_CHAIN_TEMP.
	 */
	driverSWLTC6812WakeupChain(chain);
	driverSWLTC6812EncodeCommand(commandCode, commandBytes);

	return driverHWIsoSpiWrite(chain, commandBytes, sizeof(commandBytes));
}

static bool driverSWLTC6812ReadRegisterGroupForChain(
	BMS_IsoSpiChain_t chain,
	driverLTC6812CommandCodeTypedef commandCode,
	uint8_t registerData[BMS_LTC6812_DEVICES][DRIVER_LTC6812_BYTES_PER_REGISTER],
	uint8_t *pecErrorCount) {
	uint8_t commandBytes[4];
	uint8_t readBytes[BMS_LTC6812_DEVICES * DRIVER_LTC6812_BYTES_PER_DEVICE];
	bool readValid = true;

	driverSWLTC6812WakeupChain(chain);
	driverSWLTC6812EncodeCommand((uint16_t)commandCode, commandBytes);

	if(!driverHWIsoSpiWriteRead(chain, commandBytes, sizeof(commandBytes), readBytes, sizeof(readBytes)))
		return false;

	for(uint8_t deviceIndex = 0u; deviceIndex < BMS_LTC6812_DEVICES; deviceIndex++) {
		uint8_t deviceBaseIndex = (uint8_t)(deviceIndex * DRIVER_LTC6812_BYTES_PER_DEVICE);
		uint16_t receivedPEC = (uint16_t)((readBytes[deviceBaseIndex + 6u] << 8) | readBytes[deviceBaseIndex + 7u]);
		uint16_t calculatedPEC = driverSWLTC6812CalculatePEC15(&readBytes[deviceBaseIndex], DRIVER_LTC6812_BYTES_PER_REGISTER);

		if(receivedPEC != calculatedPEC) {
			(*pecErrorCount)++;
			readValid = false;
		}

		memcpy(registerData[deviceIndex], &readBytes[deviceBaseIndex], DRIVER_LTC6812_BYTES_PER_REGISTER);
	}

	return readValid;
}

static bool driverSWLTC6812WriteRegisterGroupForChain(
	BMS_IsoSpiChain_t chain,
	driverLTC6812CommandCodeTypedef commandCode,
	const uint8_t registerData[BMS_LTC6812_DEVICES][DRIVER_LTC6812_BYTES_PER_REGISTER]) {
	uint8_t commandAndData[4 + (BMS_LTC6812_DEVICES * DRIVER_LTC6812_BYTES_PER_DEVICE)];
	uint8_t writeIndex = 4u;

	driverSWLTC6812WakeupChain(chain);
	driverSWLTC6812EncodeCommand((uint16_t)commandCode, commandAndData);

	for(uint8_t deviceIndex = 0u; deviceIndex < BMS_LTC6812_DEVICES; deviceIndex++) {
		uint16_t dataPEC = driverSWLTC6812CalculatePEC15(registerData[deviceIndex], DRIVER_LTC6812_BYTES_PER_REGISTER);

		memcpy(&commandAndData[writeIndex], registerData[deviceIndex], DRIVER_LTC6812_BYTES_PER_REGISTER);
		commandAndData[writeIndex + 6u] = (uint8_t)((dataPEC >> 8) & 0xFFu);
		commandAndData[writeIndex + 7u] = (uint8_t)(dataPEC & 0xFFu);
		writeIndex = (uint8_t)(writeIndex + DRIVER_LTC6812_BYTES_PER_DEVICE);
	}

	return driverHWIsoSpiWrite(chain, commandAndData, sizeof(commandAndData));
}

static bool driverSWLTC6812ReadConfigRegistersForChain(
	BMS_IsoSpiChain_t chain,
	uint8_t configA[BMS_LTC6812_DEVICES][DRIVER_LTC6812_BYTES_PER_REGISTER],
	uint8_t configB[BMS_LTC6812_DEVICES][DRIVER_LTC6812_BYTES_PER_REGISTER],
	uint8_t *pecErrorCount) {
	static const driverLTC6812CommandCodeTypedef readCommands[DRIVER_LTC6812_CONFIG_GROUPS] = {
		DRIVER_LTC6812_CMD_RDCFGA,
		DRIVER_LTC6812_CMD_RDCFGB
	};
	uint8_t (*configGroups[DRIVER_LTC6812_CONFIG_GROUPS])[DRIVER_LTC6812_BYTES_PER_REGISTER] = {
		configA,
		configB
	};

	for(uint8_t groupIndex = 0u; groupIndex < DRIVER_LTC6812_CONFIG_GROUPS; groupIndex++) {
		if(!driverSWLTC6812ReadRegisterGroupForChain(chain, readCommands[groupIndex], configGroups[groupIndex], pecErrorCount))
			return false;
	}

	return true;
}

static bool driverSWLTC6812WriteConfigRegistersForChain(
	BMS_IsoSpiChain_t chain,
	const uint8_t configA[BMS_LTC6812_DEVICES][DRIVER_LTC6812_BYTES_PER_REGISTER],
	const uint8_t configB[BMS_LTC6812_DEVICES][DRIVER_LTC6812_BYTES_PER_REGISTER]) {
	static const driverLTC6812CommandCodeTypedef writeCommands[DRIVER_LTC6812_CONFIG_GROUPS] = {
		DRIVER_LTC6812_CMD_WRCFGA,
		DRIVER_LTC6812_CMD_WRCFGB
	};
	const uint8_t (*configGroups[DRIVER_LTC6812_CONFIG_GROUPS])[DRIVER_LTC6812_BYTES_PER_REGISTER] = {
		configA,
		configB
	};

	for(uint8_t groupIndex = 0u; groupIndex < DRIVER_LTC6812_CONFIG_GROUPS; groupIndex++) {
		if(!driverSWLTC6812WriteRegisterGroupForChain(chain, writeCommands[groupIndex], configGroups[groupIndex]))
			return false;
	}

	return true;
}

static bool driverSWLTC6812ReadTempConfigRegisters(
	uint8_t configA[BMS_LTC6812_DEVICES][DRIVER_LTC6812_BYTES_PER_REGISTER],
	uint8_t configB[BMS_LTC6812_DEVICES][DRIVER_LTC6812_BYTES_PER_REGISTER],
	uint8_t *pecErrorCount) {
	return driverSWLTC6812ReadConfigRegistersForChain(BMS_ISOSPI_CHAIN_TEMP, configA, configB, pecErrorCount);
}

static bool driverSWLTC6812WriteTempConfigRegisters(
	const uint8_t configA[BMS_LTC6812_DEVICES][DRIVER_LTC6812_BYTES_PER_REGISTER],
	const uint8_t configB[BMS_LTC6812_DEVICES][DRIVER_LTC6812_BYTES_PER_REGISTER]) {
	return driverSWLTC6812WriteConfigRegistersForChain(BMS_ISOSPI_CHAIN_TEMP, configA, configB);
}

static bool driverSWLTC6812ReadVoltageRegistersForChain(BMS_IsoSpiChain_t chain, driverLTC6812StatusTypedef *chainStatus, driverLTC6812AnalogVoltageTypedef sensorVoltages[BMS_TOTAL_TEMPS]) {
	/* LTC6812-1 Rev. B, Tables 40-44: 15 cells are returned as five register
	 * groups (A-E), three 16-bit channels per group, 6 data bytes plus 2 PEC bytes
	 * per device. For five daisy-chained devices that is 5 * 8 = 40 return bytes.
	 */
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
				/* LTC6812-1 Rev. B, "ADC Range and Resolution" and Table 55:
				 * CxV/GxV use 100uV per LSB, so mV = raw / 10.
				 */
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
	driverSWLTC6812ClearCellBalanceStatus();
	driverSWLTC6812ClearCellOpenWireStatus();
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
	driverSWLTC6812ConvertAnalogToCellVoltages(analogVoltages, cellVoltages);

	return readValid;
}

bool driverSWLTC6812ReadTemperatureVoltages(driverLTC6812AnalogVoltageTypedef sensorVoltages[BMS_TOTAL_TEMPS]) {
	return driverSWLTC6812ReadVoltageRegistersForChain(BMS_ISOSPI_CHAIN_TEMP, &driverSWLTC6812TempChainStatus, sensorVoltages);
}

static bool driverSWLTC6812SetCellBalanceMaskInternal(
	const uint16_t balanceMaskPerDevice[BMS_LTC6812_DEVICES],
	bool allowDisableRecovery) {
	uint8_t configA[BMS_LTC6812_DEVICES][DRIVER_LTC6812_BYTES_PER_REGISTER];
	uint8_t configB[BMS_LTC6812_DEVICES][DRIVER_LTC6812_BYTES_PER_REGISTER];
	uint8_t pecErrorCount = 0u;
	uint16_t disabledMask[BMS_LTC6812_DEVICES] = {0u};

	/* CELL balancing is private to the CELL chain only.
	 * Do not create any public arbitrary-chain DCC API: TEMP chain S outputs are
	 * reserved for sensor-bias enables and must never receive balancing commands.
	 */
	driverSWLTC6812ClearCellBalanceStatus();

	if(!driverSWLTC6812ReadConfigRegistersForChain(BMS_ISOSPI_CHAIN_CELL, configA, configB, &pecErrorCount)) {
		driverSWLTC6812CellBalanceStatus.lastConfigPECErrors = pecErrorCount;
		driverSWLTC6812RecordCellBalanceFailure(1u);
		return false;
	}

	driverSWLTC6812ApplyCellBalanceMask(balanceMaskPerDevice, configA, configB);

	if(!driverSWLTC6812WriteConfigRegistersForChain(BMS_ISOSPI_CHAIN_CELL, configA, configB)) {
		driverSWLTC6812ClearCellBalanceBits(configA, configB);
		(void)driverSWLTC6812WriteConfigRegistersForChain(BMS_ISOSPI_CHAIN_CELL, configA, configB);
		driverSWLTC6812RecordCellBalanceFailure(1u);
		return false;
	}

	memset(configA, 0, sizeof(configA));
	memset(configB, 0, sizeof(configB));
	pecErrorCount = 0u;
	if(!driverSWLTC6812ReadConfigRegistersForChain(BMS_ISOSPI_CHAIN_CELL, configA, configB, &pecErrorCount)) {
		driverSWLTC6812CellBalanceStatus.lastConfigPECErrors = pecErrorCount;
		if(allowDisableRecovery)
			(void)driverSWLTC6812SetCellBalanceMaskInternal(disabledMask, false);
		driverSWLTC6812RecordCellBalanceFailure(1u);
		return false;
	}

	driverSWLTC6812StoreCellBalanceStatus(
		balanceMaskPerDevice,
		pecErrorCount,
		driverSWLTC6812CellBalanceMaskMatches(balanceMaskPerDevice, configA, configB));

	if(!driverSWLTC6812CellBalanceStatus.lastConfigValid) {
		if(allowDisableRecovery)
			(void)driverSWLTC6812SetCellBalanceMaskInternal(disabledMask, false);
		driverSWLTC6812RecordCellBalanceFailure(1u);
		return false;
	}

	return true;
}

bool driverSWLTC6812SetCellBalanceMask(const uint16_t balanceMaskPerDevice[BMS_LTC6812_DEVICES]) {
	return driverSWLTC6812SetCellBalanceMaskInternal(balanceMaskPerDevice, true);
}

bool driverSWLTC6812DisableAllCellBalancing(void) {
	uint16_t disabledMask[BMS_LTC6812_DEVICES] = {0u};
	bool disabled = driverSWLTC6812SetCellBalanceMaskInternal(disabledMask, false);

	if(!disabled) {
		driverSWLTC6812CellBalanceStatus.activeCellCount = 0u;
		memset(driverSWLTC6812CellBalanceStatus.balanceMaskPerDevice, 0, sizeof(driverSWLTC6812CellBalanceStatus.balanceMaskPerDevice));
	}

	return disabled;
}

bool driverSWLTC6812RunCellOpenWireDiagnostic(void) {
	driverLTC6812CellVoltageTypedef pullUpVoltages[BMS_TOTAL_CELLS];
	driverLTC6812CellVoltageTypedef pullDownVoltages[BMS_TOTAL_CELLS];

	driverSWLTC6812ClearCellOpenWireStatus();

	if(!driverSWLTC6812RunCellOpenWirePass(true, pullUpVoltages, &driverSWLTC6812CellOpenWireStatus.lastDiagnosticPECErrors)) {
		driverSWLTC6812CellOpenWireStatus.lastDiagnosticErrorCount = 1u;
		return false;
	}

	if(!driverSWLTC6812RunCellOpenWirePass(false, pullDownVoltages, &driverSWLTC6812CellOpenWireStatus.lastDiagnosticPECErrors)) {
		driverSWLTC6812CellOpenWireStatus.lastDiagnosticErrorCount = (uint8_t)(driverSWLTC6812CellOpenWireStatus.lastDiagnosticErrorCount + 1u);
		return false;
	}

	for(uint8_t deviceIndex = 0u; deviceIndex < BMS_LTC6812_DEVICES; deviceIndex++) {
		uint8_t deviceBaseIndex = (uint8_t)(deviceIndex * BMS_LTC6812_CELLS_PER_DEVICE);

		/* LTC6812-1 Rev. B "Open Wire Check (ADOW Command)":
		 * - if CELLPU(1) == 0.0000, treat the first measured cell connection as open
		 * - if CELLPU(n+1) - CELLPD(n+1) < -400mV, treat cell n as open
		 * - if CELLPD(15) == 0.0000, treat the last measured cell connection as open
		 */
		if(pullUpVoltages[deviceBaseIndex].rawCode == 0u) {
			driverSWLTC6812FlagOpenWireCell(deviceBaseIndex);
		}

		for(uint8_t cellIndexOnDevice = 0u; cellIndexOnDevice < (BMS_LTC6812_CELLS_PER_DEVICE - 1u); cellIndexOnDevice++) {
			uint8_t nextCellIndex = (uint8_t)(deviceBaseIndex + cellIndexOnDevice + 1u);
			int32_t deltaMilliVolts =
				(int32_t)pullUpVoltages[nextCellIndex].milliVolts - (int32_t)pullDownVoltages[nextCellIndex].milliVolts;

			if(deltaMilliVolts < DRIVER_LTC6812_OPEN_WIRE_THRESHOLD_MV) {
				uint8_t flaggedCellIndex = (uint8_t)(deviceBaseIndex + cellIndexOnDevice);
				driverSWLTC6812FlagOpenWireCell(flaggedCellIndex);
			}
		}

		if(pullDownVoltages[deviceBaseIndex + (BMS_LTC6812_CELLS_PER_DEVICE - 1u)].rawCode == 0u) {
			uint8_t lastCellIndex = (uint8_t)(deviceBaseIndex + (BMS_LTC6812_CELLS_PER_DEVICE - 1u));
			driverSWLTC6812FlagOpenWireCell(lastCellIndex);
		}
	}

	driverSWLTC6812CellOpenWireStatus.lastDiagnosticValid =
		(driverSWLTC6812CellOpenWireStatus.lastDiagnosticErrorCount == 0u);
	return driverSWLTC6812CellOpenWireStatus.lastDiagnosticValid;
}

bool driverSWLTC6812SetTempSensorEnableMask(const uint16_t enableMaskPerDevice[BMS_LTC6812_DEVICES]) {
	uint8_t configA[BMS_LTC6812_DEVICES][DRIVER_LTC6812_BYTES_PER_REGISTER];
	uint8_t configB[BMS_LTC6812_DEVICES][DRIVER_LTC6812_BYTES_PER_REGISTER];
	uint8_t pecErrorCount = 0u;

	/* The TEMP chain uses the LTC6812 discharge-control outputs only as temporary
	 * sensor-bias enables. Keep this write path private to BMS_ISOSPI_CHAIN_TEMP so
	 * cell-balancing code cannot accidentally reuse it.
	 */
	if(!driverSWLTC6812ReadTempConfigRegisters(configA, configB, &pecErrorCount)) {
		driverSWLTC6812TempChainStatus.lastReadPECErrors = pecErrorCount;
		driverSWLTC6812TempChainStatus.lastReadValid = false;
		return false;
	}

	driverSWLTC6812ApplyTempEnableMask(enableMaskPerDevice, configA, configB);

	if(!driverSWLTC6812WriteTempConfigRegisters(configA, configB))
		return false;

	memset(configA, 0, sizeof(configA));
	memset(configB, 0, sizeof(configB));
	pecErrorCount = 0u;
	if(!driverSWLTC6812ReadTempConfigRegisters(configA, configB, &pecErrorCount)) {
		driverSWLTC6812TempChainStatus.lastReadPECErrors = pecErrorCount;
		driverSWLTC6812TempChainStatus.lastReadValid = false;
		return false;
	}

	driverSWLTC6812TempChainStatus.lastReadPECErrors = pecErrorCount;
	driverSWLTC6812TempChainStatus.lastReadValid = driverSWLTC6812TempEnableMaskMatches(enableMaskPerDevice, configA, configB);
	return driverSWLTC6812TempChainStatus.lastReadValid;
}

bool driverSWLTC6812DisableTempSensorEnables(void) {
	uint16_t disabledMask[BMS_LTC6812_DEVICES] = {0u};
	return driverSWLTC6812SetTempSensorEnableMask(disabledMask);
}

bool driverSWLTC6812ReadTemperatureVoltagesWithSensorEnable(
	driverLTC6812AnalogVoltageTypedef sensorVoltages[BMS_TOTAL_TEMPS],
	const uint16_t enableMaskPerDevice[BMS_LTC6812_DEVICES]) {
	bool readValid = false;
	bool disableValid;

	if(!driverSWLTC6812SetTempSensorEnableMask(enableMaskPerDevice))
		return false;

	/* TODO(phase9): tighten these delays once bench captures confirm the board-level
	 * bias-settle time and the chosen ADC mode's full conversion time.
	 */
	HAL_Delay(DRIVER_LTC6812_TEMP_SETTLE_DELAY_MS);

	if(!driverSWLTC6812StartTemperatureVoltageConversion()) {
		(void)driverSWLTC6812DisableTempSensorEnables();
		driverSWLTC6812TempChainStatus.lastReadValid = false;
		return false;
	}

	HAL_Delay(DRIVER_LTC6812_TEMP_ADCV_DELAY_MS);
	readValid = driverSWLTC6812ReadTemperatureVoltages(sensorVoltages);
	disableValid = driverSWLTC6812DisableTempSensorEnables();

	if(!disableValid) {
		driverSWLTC6812TempChainStatus.lastReadValid = false;
		return false;
	}

	return readValid;
}

driverLTC6812StatusTypedef driverSWLTC6812GetCellChainStatus(void) {
	return driverSWLTC6812CellChainStatus;
}

driverLTC6812StatusTypedef driverSWLTC6812GetTemperatureChainStatus(void) {
	return driverSWLTC6812TempChainStatus;
}

driverLTC6812BalanceStatusTypedef driverSWLTC6812GetCellBalanceStatus(void) {
	return driverSWLTC6812CellBalanceStatus;
}

driverLTC6812OpenWireStatusTypedef driverSWLTC6812GetCellOpenWireStatus(void) {
	return driverSWLTC6812CellOpenWireStatus;
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
