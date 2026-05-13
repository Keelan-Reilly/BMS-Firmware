#include "stm32f3xx_hal.h"
#include "stdbool.h"

#define NoOfI2C2Ports				3

typedef struct {
	GPIO_TypeDef* Port;
	uint32_t ClkRegister;
	uint32_t Pin;
	uint32_t Mode;
	uint32_t Pull;
	uint32_t Alternate;
} I2C2PortStruct;

void driverHWI2C2Init(void);
/* Returns true when the HAL I2C transaction succeeds, false on error/timeout. */
bool driverHWI2C2Write(uint16_t DevAddress, bool readWrite, uint8_t *pData, uint16_t Size);
/* Returns true when the HAL I2C transaction succeeds, false on error/timeout. */
bool driverHWI2C2Read(uint16_t DevAddress, uint8_t *pData, uint16_t Size);
