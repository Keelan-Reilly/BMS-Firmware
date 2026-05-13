#ifndef __DRIVERHWPOWERSTATE_H
#define __DRIVERHWPOWERSTATE_H

#include "generalDefines.h"
#include "stm32f3xx_hal.h"
#include "stdbool.h"

#define NoOfPowersSTATs				3

typedef struct {
	GPIO_TypeDef* Port;
	uint32_t ClkRegister;
	uint32_t Pin;
	uint32_t Mode;
	uint32_t Pull;
} PowerStatePortStruct;

extern const PowerStatePortStruct driverHWPorts[NoOfPowersSTATs];

typedef enum {
	POWER_STATE_OUTPUT_ENABLE = 0,
	POWER_STATE_INPUT_BUTTON,
	POWER_STATE_INPUT_CHARGE_DETECT
} PowerStateIDTypedef;

/* TODO(migration): Remove legacy aliases after call sites are updated. */
#define P_STAT_POWER_ENABLE POWER_STATE_OUTPUT_ENABLE
#define P_STAT_BUTTON_INPUT POWER_STATE_INPUT_BUTTON
#define P_STAT_CHARGE_DETECT POWER_STATE_INPUT_CHARGE_DETECT

typedef enum {
	P_STAT_RESET = 0,
	P_STAT_SET
} PowerStateStateTypedef;

void driverHWPowerStateInit(void);
void driverHWPowerStateSetOutput(PowerStateIDTypedef inputPort, PowerStateStateTypedef newState);
bool driverHWPowerStateReadInput(PowerStateIDTypedef inputPort);

#endif
