#ifndef DRIVER_HWSPI1_H_
#define DRIVER_HWSPI1_H_

#include "stm32f3xx_hal.h"
#include "stdbool.h"
#include "stdlib.h"
#include "string.h"

#define driverHWSPI1DefaultTimeout										100

typedef enum {
	BMS_ISOSPI_CHAIN_CELL = 0,
	BMS_ISOSPI_CHAIN_TEMP,
	BMS_ISOSPI_CHAIN_NONE
} BMS_IsoSpiChain_t;

void driverHWIsoSpiInit(void);
void driverHWIsoSpiDeselectAll(void);
bool driverHWIsoSpiSelect(BMS_IsoSpiChain_t chain);
bool driverHWIsoSpiIsSelected(BMS_IsoSpiChain_t chain);
bool driverHWIsoSpiWrite(BMS_IsoSpiChain_t chain, uint8_t *writeBuffer, uint8_t noOfBytesToWrite);
bool driverHWIsoSpiWriteRead(BMS_IsoSpiChain_t chain, uint8_t *writeBuffer, uint8_t noOfBytesToWrite, uint8_t *readBuffer, uint8_t noOfBytesToRead);

void driverHWSPI1Init(GPIO_TypeDef* GPIOCSPort, uint16_t GPIO_CSPin);
bool driverHWSPI1Write(uint8_t *writeBuffer, uint8_t noOfBytesToWrite,GPIO_TypeDef* GPIOCSPort, uint16_t GPIO_CSPin);
bool driverHWSPI1WriteRead(uint8_t *writeBuffer, uint8_t noOfBytesToWrite, uint8_t *readBuffer, uint8_t noOfBytesToRead,GPIO_TypeDef* GPIOCSPort, uint16_t GPIO_CSPin);

#endif
