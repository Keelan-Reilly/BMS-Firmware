#include "driverHWSPI1.h"
#include "mxconstants.h"

SPI_HandleTypeDef driverHWSPI1Handle;
static BMS_IsoSpiChain_t driverHWIsoSpiSelectedChain = BMS_ISOSPI_CHAIN_NONE;

static void driverHWIsoSpiConfigureChipSelect(GPIO_TypeDef *gpioPort, uint16_t gpioPin) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = gpioPin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(gpioPort, &GPIO_InitStruct);
	HAL_GPIO_WritePin(gpioPort, gpioPin, GPIO_PIN_SET);
}

void driverHWIsoSpiInit(void) {
	driverHWIsoSpiConfigureChipSelect(CS_CELL_GPIO_Port, CS_CELL_Pin);
	driverHWIsoSpiConfigureChipSelect(CS_TEMP_GPIO_Port, CS_TEMP_Pin);
	driverHWIsoSpiDeselectAll();
}

void driverHWIsoSpiDeselectAll(void) {
	HAL_GPIO_WritePin(CS_CELL_GPIO_Port, CS_CELL_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(CS_TEMP_GPIO_Port, CS_TEMP_Pin, GPIO_PIN_SET);
	driverHWIsoSpiSelectedChain = BMS_ISOSPI_CHAIN_NONE;
}

bool driverHWIsoSpiSelect(BMS_IsoSpiChain_t chain) {
	driverHWIsoSpiDeselectAll();

	switch(chain) {
		case BMS_ISOSPI_CHAIN_CELL:
			HAL_GPIO_WritePin(CS_CELL_GPIO_Port, CS_CELL_Pin, GPIO_PIN_RESET);
			driverHWIsoSpiSelectedChain = chain;
			return true;
		case BMS_ISOSPI_CHAIN_TEMP:
			HAL_GPIO_WritePin(CS_TEMP_GPIO_Port, CS_TEMP_Pin, GPIO_PIN_RESET);
			driverHWIsoSpiSelectedChain = chain;
			return true;
		case BMS_ISOSPI_CHAIN_NONE:
		default:
			return false;
	}
}

bool driverHWIsoSpiIsSelected(BMS_IsoSpiChain_t chain) {
	return (driverHWIsoSpiSelectedChain == chain);
}

static bool driverHWSPI1TransferRaw(uint8_t *writeBuffer, uint8_t *readBuffer, uint8_t transferLength) {
	HAL_StatusTypeDef halReturnStatus;

	halReturnStatus = HAL_SPI_TransmitReceive(&driverHWSPI1Handle, writeBuffer, readBuffer, transferLength, driverHWSPI1DefaultTimeout);
	while(driverHWSPI1Handle.State == HAL_SPI_STATE_BUSY);

	return (halReturnStatus == HAL_OK);
}

bool driverHWIsoSpiWrite(BMS_IsoSpiChain_t chain, uint8_t *writeBuffer, uint8_t noOfBytesToWrite) {
	uint8_t *readBuffer;
	bool transactionOK = false;

	if(!driverHWIsoSpiSelect(chain))
		return false;

	readBuffer = malloc(noOfBytesToWrite);
	if(readBuffer != NULL)
		transactionOK = driverHWSPI1TransferRaw(writeBuffer, readBuffer, noOfBytesToWrite);

	driverHWIsoSpiDeselectAll();
	free(readBuffer);

	return transactionOK;
}

bool driverHWIsoSpiWriteRead(BMS_IsoSpiChain_t chain, uint8_t *writeBuffer, uint8_t noOfBytesToWrite, uint8_t *readBuffer, uint8_t noOfBytesToRead) {
	uint8_t *writeArray, *readArray;
	bool transactionOK = false;

	if(!driverHWIsoSpiSelect(chain))
		return false;

	writeArray = malloc(sizeof(uint8_t) * (noOfBytesToWrite + noOfBytesToRead));
	readArray = malloc(sizeof(uint8_t) * (noOfBytesToWrite + noOfBytesToRead));

	if((writeArray != NULL) && (readArray != NULL)) {
		memset(writeArray, 0xFF, noOfBytesToWrite + noOfBytesToRead);
		memcpy(writeArray, writeBuffer, noOfBytesToWrite);
		transactionOK = driverHWSPI1TransferRaw(writeArray, readArray, noOfBytesToWrite + noOfBytesToRead);
		memcpy(readBuffer, readArray + noOfBytesToWrite, noOfBytesToRead);
	}

	driverHWIsoSpiDeselectAll();
	free(writeArray);
	free(readArray);

	return transactionOK;
}

void driverHWSPI1Init(GPIO_TypeDef* GPIOCSPort, uint16_t GPIO_CSPin) {
  driverHWSPI1Handle.Instance = SPI1;
  driverHWSPI1Handle.Init.Mode = SPI_MODE_MASTER;
  driverHWSPI1Handle.Init.Direction = SPI_DIRECTION_2LINES;
  driverHWSPI1Handle.Init.DataSize = SPI_DATASIZE_8BIT;
  /* LTC6820 Rev. C, Table 4 and the page-45 LTC6812 reference schematic tie
   * POL = 1 and PHA = 1, which is SPI mode 3. The STM32F303 SPI1 block supports
   * CPOL/CPHA selection directly (repo datasheet: STM32F303xC, Table 63 / SPI1).
   * TODO: CubeMX-generated SPI1 init paths still need to be reconciled with this
   * runtime contract in a later phase.
   */
  driverHWSPI1Handle.Init.CLKPolarity = SPI_POLARITY_HIGH;
  driverHWSPI1Handle.Init.CLKPhase = SPI_PHASE_2EDGE;
  driverHWSPI1Handle.Init.NSS = SPI_NSS_HARD_OUTPUT;
  driverHWSPI1Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  driverHWSPI1Handle.Init.FirstBit = SPI_FIRSTBIT_MSB;
  driverHWSPI1Handle.Init.TIMode = SPI_TIMODE_DISABLE;
  driverHWSPI1Handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  driverHWSPI1Handle.Init.CRCPolynomial = 7;
  driverHWSPI1Handle.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  driverHWSPI1Handle.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&driverHWSPI1Handle) != HAL_OK)
  {
    while(true);
  }

	driverHWIsoSpiInit();

	if(GPIOCSPort != NULL)
		HAL_GPIO_WritePin(GPIOCSPort,GPIO_CSPin,GPIO_PIN_SET);
};

bool driverHWSPI1Write(uint8_t *writeBuffer, uint8_t noOfBytesToWrite, GPIO_TypeDef* GPIOCSPort, uint16_t GPIO_CSPin) {
	uint8_t *readBuffer;																																					// Make fake buffer holder
	bool transactionOK = false;
	readBuffer = malloc(noOfBytesToWrite);																												// Make fake buffer for

	HAL_GPIO_WritePin(GPIOCSPort,GPIO_CSPin,GPIO_PIN_RESET);																// Make CS low
	if(readBuffer != NULL)
		transactionOK = driverHWSPI1TransferRaw(writeBuffer,readBuffer,noOfBytesToWrite);
	HAL_GPIO_WritePin(GPIOCSPort,GPIO_CSPin,GPIO_PIN_SET);																	// Make CS High

	free(readBuffer);																																							// Dump de fake buffer

	return transactionOK;																																						// Return true if all went OK
};

bool driverHWSPI1WriteRead(uint8_t *writeBuffer, uint8_t noOfBytesToWrite, uint8_t *readBuffer, uint8_t noOfBytesToRead, GPIO_TypeDef* GPIOCSPort, uint16_t GPIO_CSPin) {
	uint8_t *writeArray, *readArray;
	bool transactionOK = false;
	
	writeArray = malloc(sizeof(uint8_t)*(noOfBytesToWrite+noOfBytesToRead));
	readArray = malloc(sizeof(uint8_t)*(noOfBytesToWrite+noOfBytesToRead));	
	
	if((writeArray != NULL) && (readArray != NULL)) {
		memset(writeArray,0xFF,noOfBytesToWrite+noOfBytesToRead);
		memcpy(writeArray,writeBuffer,noOfBytesToWrite);
	
		HAL_GPIO_WritePin(GPIOCSPort,GPIO_CSPin,GPIO_PIN_RESET);
		transactionOK = driverHWSPI1TransferRaw(writeArray,readArray,noOfBytesToWrite+noOfBytesToRead);
		HAL_GPIO_WritePin(GPIOCSPort,GPIO_CSPin,GPIO_PIN_SET);
	
		memcpy(readBuffer,readArray+noOfBytesToWrite,noOfBytesToRead);
	}
		
	free(writeArray);
	free(readArray);
	
	return transactionOK;																																						// Return true if all went OK
};


