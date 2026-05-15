#include "modCommands.h"

// Private variables
static uint8_t modCommandsSendBuffer[PACKET_MAX_PL_LEN];
static void(*modCommandsSendFunction)(unsigned char *data, unsigned int len) = 0;
bool jumpBootloaderTrue;
modConfigGeneralConfigStructTypedef *modCommandsGeneralConfig;
modConfigGeneralConfigStructTypedef *modCommandsToBeSendConfig;
modConfigGeneralConfigStructTypedef modCommandsConfigStorage;
modPowerElectricsPackStateTypedef   *modCommandsGeneralState;
static bms_config_v2_t modCommandsActiveConfigV2;
static bms_config_v2_t modCommandsDefaultConfigV2;

#define MOD_COMMANDS_APP_START_ADDRESS         0x08000000u
#define MOD_COMMANDS_EEPROM_PAGE0_ADDRESS      0x08000800u
#define MOD_COMMANDS_EEPROM_PAGE1_ADDRESS      0x08001000u
#define MOD_COMMANDS_APP_BODY_START_ADDRESS    0x08001800u
#define MOD_COMMANDS_STAGED_UPDATE_ADDRESS     0x08019000u
#define MOD_COMMANDS_BOOTLOADER_ADDRESS        0x08032000u
#define MOD_COMMANDS_FLASH_END_ADDRESS         0x08040000u
#define MOD_COMMANDS_STAGED_IMAGE_MAX_SIZE     (MOD_COMMANDS_BOOTLOADER_ADDRESS - MOD_COMMANDS_STAGED_UPDATE_ADDRESS)
#define MOD_COMMANDS_APPLICATION_MAX_SIZE      (MOD_COMMANDS_BOOTLOADER_ADDRESS - MOD_COMMANDS_APP_START_ADDRESS)
#define MOD_COMMANDS_CONFIG_V2_PAYLOAD_VERSION 1u
#define MOD_COMMANDS_CONFIG_V2_BODY_OFFSET     20u

static uint16_t modCommandsCalcCRC16(const uint8_t *data, uint32_t len) {
	uint16_t crc = 0u;

	for(uint32_t index = 0u; index < len; index++) {
		crc ^= (uint16_t)data[index] << 8;

		for(uint8_t bit = 0u; bit < 8u; bit++) {
			if(crc & 0x8000u) {
				crc = (uint16_t)((crc << 1) ^ 0x1021u);
			} else {
				crc <<= 1;
			}
		}
	}

	return crc;
}

static void modCommandsConfigV2LoadDefaults(bms_config_v2_t *config) {
	memset(config, 0, sizeof(*config));
	config->magic = BMS_CONFIG_V2_MAGIC;
	config->schemaVersion = BMS_CONFIG_V2_SCHEMA_VERSION;
	config->payloadLength = BMS_CONFIG_V2_WIRE_SIZE;
	config->generation = 1u;
	config->hardwareProfile = BMS_HARDWARE_PROFILE_STM32F303_LTC6812_DUAL_ISOSPI_75S75T;
	config->cellCount = BMS_TOTAL_CELLS;
	config->tempCount = BMS_TOTAL_TEMPS;
	config->minimalPrechargePermille = 800u;
	config->lowCurrentPrechargeTimeoutMs = 300u;
	config->cellOvSoftMv = 4150u;
	config->cellOvHardMv = 4250u;
	config->cellUvSoftMv = 2900u;
	config->cellUvHardMv = 2300u;
	config->chargeTempLimitDeciC = 450;
	config->dischargeTempLimitDeciC = 600;
	config->hardTempLimitDeciC = 700;
	config->currentSign = 0u;
	config->openWirePolicy = 1u;
	config->balanceStartMv = 3800u;
	config->balanceDiffMv = 10u;
	config->tempSettleTimeMs = 1u;
	config->featureFlags = BMS_FEATURE_MIGRATED_LTC6812_MODEL |
		BMS_FEATURE_DUAL_ISOSPI |
		BMS_FEATURE_EXP_TEMP |
		BMS_FEATURE_AUX_COUNT_ZERO |
		BMS_FEATURE_CONFIG_V2 |
		BMS_FEATURE_CONFIG_WRITE |
		BMS_FEATURE_BOOTLOADER_UPDATE;
	memset(config->requiredCellMask, 0xFF, 9u);
	config->requiredCellMask[9] = 0x07u;
	memset(config->requiredTempMask, 0xFF, 9u);
	config->requiredTempMask[9] = 0x07u;
	memset(config->balanceAllowedMask, 0xFF, 9u);
	config->balanceAllowedMask[9] = 0x07u;
	config->vpackGainMicroPerVolt = 1000000;
	config->islVbatGainMicroPerVolt = 1000000;
	config->currentGainMicroPerAmp = 1000000;
	config->bodyCrc = modCommandsCalcCRC16(((const uint8_t*)config) + MOD_COMMANDS_CONFIG_V2_BODY_OFFSET,
		BMS_CONFIG_V2_WIRE_SIZE - MOD_COMMANDS_CONFIG_V2_BODY_OFFSET);
}

static bool modCommandsConfigV2MaskValid(const uint8_t mask[BMS_CONFIG_V2_MASK_BYTES]) {
	return (mask[9] & 0xF8u) == 0u;
}

static bms_config_v2_result_t modCommandsValidateConfigV2(const bms_config_v2_t *config) {
	uint16_t expectedBodyCrc = modCommandsCalcCRC16(((const uint8_t*)config) + MOD_COMMANDS_CONFIG_V2_BODY_OFFSET,
		BMS_CONFIG_V2_WIRE_SIZE - MOD_COMMANDS_CONFIG_V2_BODY_OFFSET);

	if(config->magic != BMS_CONFIG_V2_MAGIC) {
		return BMS_CONFIG_V2_RESULT_BAD_MAGIC;
	}
	if(config->schemaVersion != BMS_CONFIG_V2_SCHEMA_VERSION) {
		return BMS_CONFIG_V2_RESULT_UNSUPPORTED_VERSION;
	}
	if(config->payloadLength != BMS_CONFIG_V2_WIRE_SIZE) {
		return BMS_CONFIG_V2_RESULT_BAD_LENGTH;
	}
	if(config->hardwareProfile != BMS_HARDWARE_PROFILE_STM32F303_LTC6812_DUAL_ISOSPI_75S75T) {
		return BMS_CONFIG_V2_RESULT_WRONG_HARDWARE_PROFILE;
	}
	if(config->cellCount != BMS_TOTAL_CELLS) {
		return BMS_CONFIG_V2_RESULT_INVALID_CELL_COUNT;
	}
	if(config->tempCount != BMS_TOTAL_TEMPS) {
		return BMS_CONFIG_V2_RESULT_INVALID_TEMP_COUNT;
	}
	if(config->bodyCrc != expectedBodyCrc) {
		return BMS_CONFIG_V2_RESULT_BAD_CRC;
	}
	if(config->cellOvHardMv <= config->cellOvSoftMv ||
		config->cellUvHardMv >= config->cellUvSoftMv ||
		config->cellUvSoftMv >= config->cellOvSoftMv) {
		return BMS_CONFIG_V2_RESULT_INVALID_THRESHOLD_ORDER;
	}
	if(config->cellUvHardMv < 1500u || config->cellUvSoftMv > 3600u ||
		config->cellOvSoftMv < 3500u || config->cellOvHardMv > 5000u ||
		config->minimalPrechargePermille == 0u || config->minimalPrechargePermille > 1000u ||
		config->lowCurrentPrechargeTimeoutMs < 50u || config->lowCurrentPrechargeTimeoutMs > 10000u ||
		config->chargeTempLimitDeciC < -400 || config->chargeTempLimitDeciC > 900 ||
		config->dischargeTempLimitDeciC < -400 || config->dischargeTempLimitDeciC > 1100 ||
		config->hardTempLimitDeciC < -400 || config->hardTempLimitDeciC > 1200 ||
		config->hardTempLimitDeciC < config->chargeTempLimitDeciC ||
		config->hardTempLimitDeciC < config->dischargeTempLimitDeciC) {
		return BMS_CONFIG_V2_RESULT_INVALID_THRESHOLD_RANGE;
	}
	if(!modCommandsConfigV2MaskValid(config->requiredCellMask) ||
		!modCommandsConfigV2MaskValid(config->requiredTempMask) ||
		!modCommandsConfigV2MaskValid(config->balanceAllowedMask)) {
		return BMS_CONFIG_V2_RESULT_INVALID_MASK;
	}
	if(config->vpackGainMicroPerVolt <= 0 ||
		config->islVbatGainMicroPerVolt <= 0 ||
		config->currentGainMicroPerAmp == 0 ||
		config->currentSign > 1u) {
		return BMS_CONFIG_V2_RESULT_INVALID_CALIBRATION;
	}
	for(uint8_t index = 0u; index < sizeof(config->reserved); index++) {
		if(config->reserved[index] != 0u) {
			return BMS_CONFIG_V2_RESULT_BAD_LENGTH;
		}
	}

	return BMS_CONFIG_V2_RESULT_OK;
}

static void modCommandsSendConfigV2Packet(COMM_PACKET_ID packetId, const bms_config_v2_t *config) {
	int32_t ind = 0;
	const uint8_t *configBytes = (const uint8_t*)config;

	modCommandsSendBuffer[ind++] = packetId;
	memcpy(modCommandsSendBuffer + ind, configBytes, sizeof(*config));
	ind += BMS_CONFIG_V2_WIRE_SIZE;
	modCommandsSendPacket(modCommandsSendBuffer, ind);
}

static void modCommandsSendConfigV2Result(COMM_PACKET_ID packetId, bms_config_v2_result_t result) {
	int32_t ind = 0;

	modCommandsSendBuffer[ind++] = packetId;
	libBufferAppend_uint8(modCommandsSendBuffer, (uint8_t)result, &ind);
	modCommandsSendPacket(modCommandsSendBuffer, ind);
}

static void modCommandsSendLegacyConfigUnsupported(const char *operation) {
	modCommandsPrintf("Legacy config command '%s' is blocked on migrated STM32F303/LTC6812 firmware. Use COMM_BMS_GET_CAPABILITIES and Config V2 only.", operation);
}

static void modCommandsSendCapabilitiesPacket(void) {
	int32_t ind = 0;
	uint32_t featureFlags = BMS_FEATURE_MIGRATED_LTC6812_MODEL |
		BMS_FEATURE_DUAL_ISOSPI |
		BMS_FEATURE_EXP_TEMP |
		BMS_FEATURE_AUX_COUNT_ZERO |
		BMS_FEATURE_CONFIG_V2 |
		BMS_FEATURE_CONFIG_WRITE |
		BMS_FEATURE_BOOTLOADER_UPDATE;

	modCommandsSendBuffer[ind++] = COMM_BMS_GET_CAPABILITIES;
	libBufferAppend_uint32(modCommandsSendBuffer, BMS_CAPABILITIES_MAGIC, &ind);
	libBufferAppend_uint8(modCommandsSendBuffer, BMS_CAPABILITIES_VERSION, &ind);
	libBufferAppend_uint8(modCommandsSendBuffer, BMS_FIRMWARE_TYPE_APPLICATION, &ind);
	libBufferAppend_uint8(modCommandsSendBuffer, FW_VERSION_MAJOR, &ind);
	libBufferAppend_uint8(modCommandsSendBuffer, FW_VERSION_MINOR, &ind);
	libBufferAppend_uint16(modCommandsSendBuffer, 0u, &ind);
	libBufferAppend_uint16(modCommandsSendBuffer, BMS_HARDWARE_PROFILE_STM32F303_LTC6812_DUAL_ISOSPI_75S75T, &ind);
	libBufferAppend_uint8(modCommandsSendBuffer, 1u, &ind);
	libBufferAppend_uint8(modCommandsSendBuffer, BMS_CONFIG_V2_SCHEMA_VERSION, &ind);
	libBufferAppend_uint8(modCommandsSendBuffer, BMS_TOTAL_CELLS, &ind);
	libBufferAppend_uint8(modCommandsSendBuffer, BMS_TOTAL_TEMPS, &ind);
	libBufferAppend_uint8(modCommandsSendBuffer, BMS_LTC6812_DEVICES, &ind);
	libBufferAppend_uint8(modCommandsSendBuffer, BMS_LTC6812_DEVICES, &ind);
	libBufferAppend_uint32(modCommandsSendBuffer, featureFlags, &ind);
	libBufferAppend_uint32(modCommandsSendBuffer, MOD_COMMANDS_APP_START_ADDRESS, &ind);
	libBufferAppend_uint32(modCommandsSendBuffer, MOD_COMMANDS_APP_BODY_START_ADDRESS, &ind);
	libBufferAppend_uint32(modCommandsSendBuffer, MOD_COMMANDS_EEPROM_PAGE0_ADDRESS, &ind);
	libBufferAppend_uint32(modCommandsSendBuffer, MOD_COMMANDS_EEPROM_PAGE1_ADDRESS, &ind);
	libBufferAppend_uint32(modCommandsSendBuffer, MOD_COMMANDS_STAGED_UPDATE_ADDRESS, &ind);
	libBufferAppend_uint32(modCommandsSendBuffer, MOD_COMMANDS_BOOTLOADER_ADDRESS, &ind);
	libBufferAppend_uint32(modCommandsSendBuffer, MOD_COMMANDS_STAGED_IMAGE_MAX_SIZE, &ind);
	libBufferAppend_uint32(modCommandsSendBuffer, MOD_COMMANDS_APPLICATION_MAX_SIZE, &ind);
	libBufferAppend_uint8(modCommandsSendBuffer, BMS_UPDATE_CRC16_CCITT_FALSE, &ind);
	libBufferAppend_uint8(modCommandsSendBuffer, 0u, &ind);
	libBufferAppend_uint8(modCommandsSendBuffer, 0u, &ind);
	libBufferAppend_uint8(modCommandsSendBuffer, 0u, &ind);
	modCommandsSendPacket(modCommandsSendBuffer, ind);
}

void modCommandsInit(modPowerElectricsPackStateTypedef   *generalState,modConfigGeneralConfigStructTypedef *configPointer) {
	modCommandsGeneralConfig = configPointer;
	modCommandsGeneralState  = generalState;
	jumpBootloaderTrue = false;
	modCommandsConfigV2LoadDefaults(&modCommandsActiveConfigV2);
}

void modCommandsSetSendFunction(void(*func)(unsigned char *data, unsigned int len)) {
	modCommandsSendFunction = func;
}

void modCommandsSendPacket(unsigned char *data, unsigned int len) {
	if (modCommandsSendFunction) {
		modCommandsSendFunction(data, len);
	}
}

static uint8_t modCommandsGetUIFaultCode(void) {
	/* Phase 14 keeps the legacy 1-byte UI surface coarse on purpose.
	 * Non-zero means an active fault category exists; detailed fault bits stay in
	 * terminal diagnostics and the internal pack-state mask.
	 */
	return modPowerElectronicsGetUIFaultCode();
}

static uint16_t modCommandsGetEBMSMeasurementFlags(void) {
	uint16_t flags = 0u;

	flags |= ((uint16_t)(modCommandsGeneralState->cellVoltageReadoutValid != 0u) << 0);
	flags |= ((uint16_t)(modCommandsGeneralState->temperatureReadoutValid != 0u) << 1);
	flags |= ((uint16_t)(modCommandsGeneralState->vBatReadoutValid != 0u) << 2);
	flags |= ((uint16_t)(modCommandsGeneralState->currentReadoutValid != 0u) << 3);
	flags |= ((uint16_t)(modCommandsGeneralState->vPackReadoutValid != 0u) << 4);
	flags |= ((uint16_t)(modCommandsGeneralState->powerMonitorReadoutValid != 0u) << 5);
	flags |= ((uint16_t)(modCommandsGeneralState->cellOpenWireValid != 0u) << 6);
	flags |= ((uint16_t)(modCommandsGeneralState->cellBalancingValid != 0u) << 7);
	flags |= ((uint16_t)(modCommandsGeneralState->prechargeMeasurementValid != 0u) << 8);
	flags |= ((uint16_t)(modCommandsGeneralState->prechargeComplete != 0u) << 9);

	return flags;
}

static void modCommandsSendLegacyValuesPacket(void) {
	int32_t ind = 0;

	modCommandsSendBuffer[ind++] = COMM_GET_VALUES;

	libBufferAppend_float32(modCommandsSendBuffer, modCommandsGeneralState->packVoltage, 1e3, &ind);
	libBufferAppend_float32(modCommandsSendBuffer, modCommandsGeneralState->packCurrent, 1e3, &ind);

	libBufferAppend_uint8(modCommandsSendBuffer, (uint8_t)round(modCommandsGeneralState->SoC), &ind);

	libBufferAppend_float32(modCommandsSendBuffer, modCommandsGeneralState->cellVoltageHigh, 1e3, &ind);
	libBufferAppend_float32(modCommandsSendBuffer, modCommandsGeneralState->cellVoltageAverage, 1e3, &ind);
	libBufferAppend_float32(modCommandsSendBuffer, modCommandsGeneralState->cellVoltageLow, 1e3, &ind);
	libBufferAppend_float32(modCommandsSendBuffer, modCommandsGeneralState->cellVoltageMisMatch, 1e3, &ind);

	libBufferAppend_float16(modCommandsSendBuffer, modCommandsGeneralState->loCurrentLoadVoltage, 1e2, &ind);
	libBufferAppend_float16(modCommandsSendBuffer, modCommandsGeneralState->loCurrentLoadCurrent, 1e2, &ind);
	libBufferAppend_float16(modCommandsSendBuffer, modCommandsGeneralState->hiCurrentLoadVoltage, 1e2, &ind);
	libBufferAppend_float16(modCommandsSendBuffer, modCommandsGeneralState->hiCurrentLoadCurrent, 1e2, &ind);
	libBufferAppend_float16(modCommandsSendBuffer, modCommandsGeneralState->auxVoltage, 1e2, &ind);
	libBufferAppend_float16(modCommandsSendBuffer, modCommandsGeneralState->auxCurrent, 1e2, &ind);

	libBufferAppend_float16(modCommandsSendBuffer, modCommandsGeneralState->tempBatteryHigh, 1e1, &ind);
	libBufferAppend_float16(modCommandsSendBuffer, modCommandsGeneralState->tempBatteryAverage, 1e1, &ind);
	libBufferAppend_float16(modCommandsSendBuffer, modCommandsGeneralState->tempBMSHigh, 1e1, &ind);
	libBufferAppend_float16(modCommandsSendBuffer, modCommandsGeneralState->tempBMSAverage, 1e1, &ind);

	libBufferAppend_uint8(modCommandsSendBuffer, (uint8_t)modCommandsGeneralState->operationalState, &ind);
	libBufferAppend_uint8(modCommandsSendBuffer, (uint8_t)modCommandsGeneralState->chargeBalanceActive, &ind);
	libBufferAppend_uint8(modCommandsSendBuffer, 0u, &ind);

	modCommandsSendBuffer[ind++] = modCommandsGeneralConfig->CANID;
	modCommandsSendPacket(modCommandsSendBuffer, ind);
}

static void modCommandsSendEBMSValuesPacket(void) {
	int32_t ind = 0;

	modCommandsSendBuffer[ind++] = COMM_EBMS_GET_VALUES;

	libBufferAppend_float32(modCommandsSendBuffer, modCommandsGeneralState->packVoltage, 1e3, &ind);
	libBufferAppend_float32(modCommandsSendBuffer, modCommandsGeneralState->packCurrent, 1e3, &ind);
	libBufferAppend_uint8(modCommandsSendBuffer, (uint8_t)round(modCommandsGeneralState->SoC), &ind);

	libBufferAppend_float32(modCommandsSendBuffer, modCommandsGeneralState->cellVoltageHigh, 1e3, &ind);
	libBufferAppend_float32(modCommandsSendBuffer, modCommandsGeneralState->cellVoltageAverage, 1e3, &ind);
	libBufferAppend_float32(modCommandsSendBuffer, modCommandsGeneralState->cellVoltageLow, 1e3, &ind);
	libBufferAppend_float32(modCommandsSendBuffer, modCommandsGeneralState->cellVoltageMisMatch, 1e3, &ind);

	libBufferAppend_float16(modCommandsSendBuffer, modCommandsGeneralState->loCurrentLoadVoltage, 1e1, &ind);
	libBufferAppend_float16(modCommandsSendBuffer, modCommandsGeneralState->loCurrentLoadCurrent, 1e1, &ind);
	/* The current pack state does not expose a distinct charger-voltage channel. */
	libBufferAppend_float16(modCommandsSendBuffer, 0.0f, 1e1, &ind);

	libBufferAppend_float16(modCommandsSendBuffer, modCommandsGeneralState->tempBatteryHigh, 1e1, &ind);
	libBufferAppend_float16(modCommandsSendBuffer, modCommandsGeneralState->tempBatteryAverage, 1e1, &ind);
	libBufferAppend_float16(modCommandsSendBuffer, modCommandsGeneralState->tempBatteryLow, 1e1, &ind);
	libBufferAppend_float16(modCommandsSendBuffer, modCommandsGeneralState->tempBMSHigh, 1e1, &ind);
	libBufferAppend_float16(modCommandsSendBuffer, modCommandsGeneralState->tempBMSAverage, 1e1, &ind);
	libBufferAppend_float16(modCommandsSendBuffer, modCommandsGeneralState->tempBMSLow, 1e1, &ind);
	libBufferAppend_float16(modCommandsSendBuffer, modCommandsGeneralState->humidity, 1e1, &ind);

	libBufferAppend_uint8(modCommandsSendBuffer, (uint8_t)modCommandsGeneralState->operationalState, &ind);
	libBufferAppend_uint8(modCommandsSendBuffer, (uint8_t)modCommandsGeneralState->chargeBalanceActive, &ind);
	libBufferAppend_uint8(modCommandsSendBuffer, modCommandsGetUIFaultCode(), &ind);

	/* The UI expects lifetime Ah/Wh counters here. Keep them as explicit zero
	 * placeholders until the firmware exposes true cumulative counters.
	 */
	libBufferAppend_float32(modCommandsSendBuffer, 0.0f, 1e3, &ind);
	libBufferAppend_float32(modCommandsSendBuffer, 0.0f, 1e3, &ind);
	libBufferAppend_float32(modCommandsSendBuffer, 0.0f, 1e3, &ind);
	libBufferAppend_float32(modCommandsSendBuffer, 0.0f, 1e3, &ind);
	libBufferAppend_float32(modCommandsSendBuffer, 0.0f, 1e3, &ind);
	libBufferAppend_float32(modCommandsSendBuffer, 0.0f, 1e3, &ind);

	modCommandsSendPacket(modCommandsSendBuffer, ind);
}

static void modCommandsSendLegacyCellsPacket(void) {
	int32_t ind = 0;

	modCommandsSendBuffer[ind++] = COMM_GET_BMS_CELLS;
	libBufferAppend_uint8(modCommandsSendBuffer, modCommandsGeneralConfig->noOfCells, &ind);

	for(uint8_t cellPointer = 0u; cellPointer < modCommandsGeneralConfig->noOfCells; cellPointer++) {
		/* Phase 13: keep legacy cell packets positive-only. The old negative-voltage
		 * "balancing active" marker was tied to a 16-bit LTC6803-era mask and is not
		 * safe for the migrated 75-cell LTC6812 pack.
		 */
		libBufferAppend_float16(modCommandsSendBuffer,
			modCommandsGeneralState->cellVoltagesIndividual[cellPointer].cellVoltage,
			1e3,
			&ind);
	}

	modCommandsSendBuffer[ind++] = modCommandsGeneralConfig->CANID;
	modCommandsSendPacket(modCommandsSendBuffer, ind);
}

static void modCommandsSendEBMSCellsPacket(void) {
	int32_t ind = 0;

	modCommandsSendBuffer[ind++] = COMM_EBMS_GET_CELLS;
	libBufferAppend_uint8(modCommandsSendBuffer, BMS_TOTAL_CELLS, &ind);

	for(uint8_t cellPointer = 0u; cellPointer < BMS_TOTAL_CELLS; cellPointer++) {
		float cellVoltage = 0.0f;

		if(modCommandsGeneralState->cellVoltageReadoutValid) {
			cellVoltage = modCommandsGeneralState->cellVoltagesLTC6812[cellPointer].cellVoltage;
		}

		libBufferAppend_float16(modCommandsSendBuffer, cellVoltage, 1e3, &ind);
	}

	modCommandsSendPacket(modCommandsSendBuffer, ind);
}

static void modCommandsSendEBMSAuxPacket(void) {
	int32_t ind = 0;

	modCommandsSendBuffer[ind++] = COMM_EBMS_GET_AUX;
	libBufferAppend_uint8(modCommandsSendBuffer, 0u, &ind);
	modCommandsSendPacket(modCommandsSendBuffer, ind);
}

static void modCommandsSendEBMSExpansionTempPacket(void) {
	int32_t ind = 0;

	modCommandsSendBuffer[ind++] = COMM_EBMS_GET_EXP_TEMP;

	if(!modCommandsGeneralState->temperatureReadoutValid) {
		libBufferAppend_uint8(modCommandsSendBuffer, 0u, &ind);
		modCommandsSendPacket(modCommandsSendBuffer, ind);
		return;
	}

	libBufferAppend_uint8(modCommandsSendBuffer, BMS_TOTAL_TEMPS, &ind);

	for(uint8_t tempPointer = 0u; tempPointer < BMS_TOTAL_TEMPS; tempPointer++) {
		float temperatureC = 0.0f;

		if(modCommandsGeneralState->temperaturesLTC6812Valid[tempPointer]) {
			temperatureC = modCommandsGeneralState->temperaturesLTC6812[tempPointer];
		}

		libBufferAppend_float16(modCommandsSendBuffer, temperatureC, 1e1, &ind);
	}

	modCommandsSendPacket(modCommandsSendBuffer, ind);
}

static void modCommandsSendEBMSStatusExtPacket(void) {
	int32_t ind = 0;

	modCommandsSendBuffer[ind++] = COMM_EBMS_GET_BMS_STATUS_EXT;
	libBufferAppend_uint8(modCommandsSendBuffer, FW_VERSION_MAJOR, &ind);
	libBufferAppend_uint8(modCommandsSendBuffer, FW_VERSION_MINOR, &ind);
	libBufferAppend_uint8(modCommandsSendBuffer, BMS_TOTAL_CELLS, &ind);
	libBufferAppend_uint8(modCommandsSendBuffer, BMS_TOTAL_TEMPS, &ind);
	libBufferAppend_uint32(modCommandsSendBuffer, modPowerElectronicsGetActiveFaultMask(), &ind);
	libBufferAppend_uint32(modCommandsSendBuffer, modPowerElectronicsGetLatchedFaultMask(), &ind);
	libBufferAppend_uint16(modCommandsSendBuffer, modCommandsGetEBMSMeasurementFlags(), &ind);
	libBufferAppend_uint8(modCommandsSendBuffer, modCommandsGeneralState->cellBalancingActiveCount, &ind);
	libBufferAppend_uint8(modCommandsSendBuffer, modCommandsGeneralState->cellOpenWireFaultCount, &ind);
	libBufferAppend_uint8(modCommandsSendBuffer, modCommandsGetUIFaultCode(), &ind);
	libBufferAppend_uint8(modCommandsSendBuffer, (uint8_t)modCommandsGeneralState->operationalState, &ind);

	modCommandsSendPacket(modCommandsSendBuffer, ind);
}

void modCommandsProcessPacket(unsigned char *data, unsigned int len) {
	if (!len) {
		return;
	}

	COMM_PACKET_ID packet_id;
	int32_t ind = 0;
	uint16_t flash_res;
	uint32_t new_app_offset;
	uint32_t delayTick;

	packet_id = (COMM_PACKET_ID) data[0];
	data++;
	len--;

	switch (packet_id) {
		case COMM_FW_VERSION:
			ind = 0;
			modCommandsSendBuffer[ind++] = COMM_FW_VERSION;
			modCommandsSendBuffer[ind++] = FW_VERSION_MAJOR;
			modCommandsSendBuffer[ind++] = FW_VERSION_MINOR;
			strcpy((char*)(modCommandsSendBuffer + ind), HW_NAME);
			ind += strlen(HW_NAME) + 1;
			memcpy(modCommandsSendBuffer + ind, STM32_UUID_8, 12);
			ind += 12;

			modCommandsSendPacket(modCommandsSendBuffer, ind);
			break;
		case COMM_JUMP_TO_BOOTLOADER:
			jumpBootloaderTrue = true;
			delayTick = HAL_GetTick();
			break;
		case COMM_ERASE_NEW_APP:
			ind = 0;
			flash_res = modFlashEraseNewAppData(libBufferGet_uint32(data, &ind));

			ind = 0;
			modCommandsSendBuffer[ind++] = COMM_ERASE_NEW_APP;
			modCommandsSendBuffer[ind++] = flash_res == HAL_OK ? true : false;
			modCommandsSendPacket(modCommandsSendBuffer, ind);
			break;
		case COMM_WRITE_NEW_APP_DATA:
			ind = 0;
			new_app_offset = libBufferGet_uint32(data, &ind);
			flash_res = modFlashWriteNewAppData(new_app_offset, data + ind, len - ind);

			ind = 0;
			modCommandsSendBuffer[ind++] = COMM_WRITE_NEW_APP_DATA;
			modCommandsSendBuffer[ind++] = flash_res == HAL_OK ? 1 : 0;
			modCommandsSendPacket(modCommandsSendBuffer, ind);
			break;
		case COMM_GET_VALUES:
			modCommandsSendLegacyValuesPacket();
			break;
		case COMM_EBMS_GET_VALUES:
			modCommandsSendEBMSValuesPacket();
			break;
		case COMM_GET_BMS_CELLS:
			modCommandsSendLegacyCellsPacket();
			break;
		case COMM_EBMS_GET_CELLS:
			modCommandsSendEBMSCellsPacket();
			break;
		case COMM_EBMS_GET_AUX:
			modCommandsSendEBMSAuxPacket();
			break;
		case COMM_EBMS_GET_EXP_TEMP:
			modCommandsSendEBMSExpansionTempPacket();
			break;
		case COMM_EBMS_GET_BMS_STATUS_EXT:
			modCommandsSendEBMSStatusExtPacket();
			break;
		case COMM_BMS_GET_CAPABILITIES:
			modCommandsSendCapabilitiesPacket();
			break;
		case COMM_BMS_GET_CONFIG_V2:
			modCommandsSendConfigV2Packet(packet_id, &modCommandsActiveConfigV2);
			break;
		case COMM_BMS_GET_CONFIG_DEFAULT_V2:
			modCommandsConfigV2LoadDefaults(&modCommandsDefaultConfigV2);
			modCommandsSendConfigV2Packet(packet_id, &modCommandsDefaultConfigV2);
			break;
		case COMM_BMS_GET_CONFIG_SCHEMA_V2:
			ind = 0;
			modCommandsSendBuffer[ind++] = packet_id;
			libBufferAppend_uint32(modCommandsSendBuffer, BMS_CONFIG_V2_MAGIC, &ind);
			libBufferAppend_uint16(modCommandsSendBuffer, BMS_CONFIG_V2_SCHEMA_VERSION, &ind);
			libBufferAppend_uint16(modCommandsSendBuffer, BMS_CONFIG_V2_WIRE_SIZE, &ind);
			libBufferAppend_uint16(modCommandsSendBuffer, BMS_HARDWARE_PROFILE_STM32F303_LTC6812_DUAL_ISOSPI_75S75T, &ind);
			libBufferAppend_uint8(modCommandsSendBuffer, BMS_TOTAL_CELLS, &ind);
			libBufferAppend_uint8(modCommandsSendBuffer, BMS_TOTAL_TEMPS, &ind);
			modCommandsSendPacket(modCommandsSendBuffer, ind);
			break;
		case COMM_BMS_VALIDATE_CONFIG_V2:
		case COMM_BMS_SET_CONFIG_V2: {
			bms_config_v2_t incomingConfig;
			bms_config_v2_result_t validationResult;

			if(len != BMS_CONFIG_V2_WIRE_SIZE) {
				modCommandsSendConfigV2Result(packet_id, BMS_CONFIG_V2_RESULT_BAD_LENGTH);
				break;
			}

			memcpy(&incomingConfig, data, BMS_CONFIG_V2_WIRE_SIZE);
			validationResult = modCommandsValidateConfigV2(&incomingConfig);
			if(packet_id == COMM_BMS_SET_CONFIG_V2 && validationResult == BMS_CONFIG_V2_RESULT_OK) {
				modCommandsActiveConfigV2 = incomingConfig;
			}

			modCommandsSendConfigV2Result(packet_id, validationResult);
			break;
		}
		case COMM_BMS_STORE_CONFIG_V2:
			modCommandsSendConfigV2Result(packet_id, BMS_CONFIG_V2_RESULT_UNSUPPORTED_IN_CURRENT_MODE);
			modCommandsPrintf("Config V2 store is intentionally disabled in this phase. RAM-only validation/apply is supported; persistent store is deferred until validated on hardware.");
			break;
		case COMM_SET_MCCONF:
		case COMM_EBMS_SET_MCCONF:
			modCommandsSendLegacyConfigUnsupported("set");
			break;
		case COMM_GET_MCCONF:
		case COMM_GET_MCCONF_DEFAULT:
		case COMM_EBMS_GET_MCCONF:
		case COMM_EBMS_GET_MCCONF_DEFAULT:
			modCommandsSendLegacyConfigUnsupported("get");
			break;
		case COMM_TERMINAL_CMD:
		  data[len] = '\0';
		  terminal_process_string((char*)data);
			break;
		case COMM_REBOOT:
			modCommandsJumpToMainApplication();
			break;
		case COMM_ALIVE:
			break;
		case COMM_FORWARD_CAN:
			modCANSendBuffer(data[0], data + 1, len - 1, false);
			break;
		case COMM_STORE_BMS_CONF:
		case COMM_EBMS_STORE_CONF:
			modCommandsSendLegacyConfigUnsupported("store");
			break;
		default:
			break;
	}
	
	if(modDelayTick1ms(&delayTick,1000) && jumpBootloaderTrue)
		modFlashJumpToBootloader();
}

void modCommandsPrintf(const char* format, ...) {
	va_list arg;
	va_start (arg, format);
	int len;
	static char print_buffer[255];

	print_buffer[0] = COMM_PRINT;
	len = vsnprintf(print_buffer+1, 254, format, arg);
	va_end (arg);

	if(len > 0) {
		modCommandsSendPacket((unsigned char*)print_buffer, (len<254)? len+1: 255);
	}
}


void modCommandsJumpToMainApplication(void) {
	NVIC_SystemReset();
}
