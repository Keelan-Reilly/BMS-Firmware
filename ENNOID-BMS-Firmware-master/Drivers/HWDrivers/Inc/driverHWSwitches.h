#include "stm32f3xx_hal.h"
#include "stdbool.h"

#define NoOfSwitches				4

typedef struct {
	GPIO_TypeDef* Port;
	uint32_t ClkRegister;
	uint32_t Pin;
	uint32_t Mode;
	uint32_t Pull;
} driverHWSwitchesPortStruct;

extern const driverHWSwitchesPortStruct driverHWSwitchesPorts[NoOfSwitches];

typedef enum {
	SWITCH_CHARGER_SAFETY = 0,
	SWITCH_CHARGE_ENABLE,
	/* Active-low downstream permission into shutdown logic. */
	SWITCH_MULTIPURPOSE_ENABLE,
	/* Active-low downstream permission into shutdown logic. */
	SWITCH_DISCHARGE_ENABLE
} driverHWSwitchesIDTypedef;

/* TODO(migration): Remove legacy aliases after call sites are updated to the
 * hardware contract names.
 */
#define SWITCH_DRIVER SWITCH_CHARGER_SAFETY
#define SWITCH_CHARGE SWITCH_CHARGE_ENABLE
#define SWITCH_PRECHARGE SWITCH_MULTIPURPOSE_ENABLE
#define SWITCH_DISCHARGE SWITCH_DISCHARGE_ENABLE

typedef enum {
	SWITCH_RESET = 0,
	SWITCH_SET,
} driverHWSwitchesStateTypedef;

void driverHWSwitchesInit(void);
void driverHWSwitchesSetSwitchState(driverHWSwitchesIDTypedef switchID, driverHWSwitchesStateTypedef newState);
void driverHWSwitchesDisableAll(void);
bool driverHWSwitchesGetMonitorEnabledState(void);
bool driverHWSwitchesGetSwitchState(driverHWSwitchesIDTypedef switchID);
