#include "driverHWSwitches.h"

static GPIO_PinState driverHWSwitchesGetActiveLowDownstreamPinState(bool allowed) {
	/* For PB11/PB10 the downstream shutdown inputs are active-low, but the MCU drives
	 * a MOSFET stage. `allowed=true` therefore maps to MCU GPIO high, which pulls the
	 * downstream signal low/asserted.
	 */
	return allowed ? GPIO_PIN_SET : GPIO_PIN_RESET;
}

static GPIO_PinState driverHWSwitchesGetDirectPermissionPinState(bool allowed) {
	/* PB0/PB2 are kept on the existing direct GPIO polarity in this phase:
	 * `allowed=true` maps to MCU GPIO high. Default/fallback is still inactive low.
	 * TODO(phase6): confirm charger-side polarity against the final schematic.
	 */
	return allowed ? GPIO_PIN_SET : GPIO_PIN_RESET;
}

const driverHWSwitchesPortStruct driverHWSwitchesPorts[NoOfSwitches] =			// Hold all status configuration data
{
	{GPIOB,RCC_AHBENR_GPIOAEN,GPIO_PIN_2,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL},		// CHARGER_SAFETY
	{GPIOB,RCC_AHBENR_GPIOCEN,GPIO_PIN_0,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL},		// CHARGE_ENABLE
	{GPIOB,RCC_AHBENR_GPIOCEN,GPIO_PIN_11,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL},		// MULTIPURPOSE_ENABLE (active-low downstream)
	{GPIOB,RCC_AHBENR_GPIOCEN,GPIO_PIN_10,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL}		// DISCHARGE_ENABLE (active-low downstream)
};

void driverHWSwitchesInit(void) {
	GPIO_InitTypeDef switchPortHolder;
	uint8_t SwitchPointer;
	
	for(SwitchPointer = 0; SwitchPointer < NoOfSwitches; SwitchPointer++) {
		RCC->AHBENR |= driverHWSwitchesPorts[SwitchPointer].ClkRegister;				// Enable clock de desired port
		switchPortHolder.Mode = driverHWSwitchesPorts[SwitchPointer].Mode;			// Push pull output
		switchPortHolder.Pin = driverHWSwitchesPorts[SwitchPointer].Pin;				// Points to status pin
		switchPortHolder.Pull = driverHWSwitchesPorts[SwitchPointer].Pull;			// Pullup
		switchPortHolder.Speed = GPIO_SPEED_HIGH;																// GPIO clock speed
			HAL_GPIO_Init(driverHWSwitchesPorts[SwitchPointer].Port,&switchPortHolder);// Perform the IO init 
		};

	/* Safe default: no permission is asserted merely because the MCU booted. */
	driverHWSwitchesDisableAll();
};

void driverHWSwitchesSetSwitchState(driverHWSwitchesIDTypedef switchID, driverHWSwitchesStateTypedef newState) {
	HAL_GPIO_WritePin(driverHWSwitchesPorts[switchID].Port,driverHWSwitchesPorts[switchID].Pin,(GPIO_PinState)newState); // Set desired pin to desired state 
};

void driverHWSwitchesSetMasterOkPermission(bool allowed) {
	HAL_GPIO_WritePin(driverHWSwitchesPorts[SWITCH_MULTIPURPOSE_ENABLE].Port,
		driverHWSwitchesPorts[SWITCH_MULTIPURPOSE_ENABLE].Pin,
		driverHWSwitchesGetActiveLowDownstreamPinState(allowed));
}

void driverHWSwitchesSetDischargePermission(bool allowed) {
	HAL_GPIO_WritePin(driverHWSwitchesPorts[SWITCH_DISCHARGE_ENABLE].Port,
		driverHWSwitchesPorts[SWITCH_DISCHARGE_ENABLE].Pin,
		driverHWSwitchesGetActiveLowDownstreamPinState(allowed));
}

void driverHWSwitchesSetChargePermission(bool allowed) {
	HAL_GPIO_WritePin(driverHWSwitchesPorts[SWITCH_CHARGE_ENABLE].Port,
		driverHWSwitchesPorts[SWITCH_CHARGE_ENABLE].Pin,
		driverHWSwitchesGetDirectPermissionPinState(allowed));
}

void driverHWSwitchesSetChargerSafetyPermission(bool allowed) {
	HAL_GPIO_WritePin(driverHWSwitchesPorts[SWITCH_CHARGER_SAFETY].Port,
		driverHWSwitchesPorts[SWITCH_CHARGER_SAFETY].Pin,
		driverHWSwitchesGetDirectPermissionPinState(allowed));
}

void driverHWSwitchesDisableAll(void) {
	driverHWSwitchesSetChargerSafetyPermission(false);
	driverHWSwitchesSetChargePermission(false);
	driverHWSwitchesSetMasterOkPermission(false);
	driverHWSwitchesSetDischargePermission(false);
};

bool driverHWSwitchesGetMonitorEnabledState(void) {
	return (bool) HAL_GPIO_ReadPin(driverHWSwitchesPorts[SWITCH_DRIVER].Port,driverHWSwitchesPorts[SWITCH_DRIVER].Pin);
};

bool driverHWSwitchesGetSwitchState(driverHWSwitchesIDTypedef switchID) {
	return (bool) HAL_GPIO_ReadPin(driverHWSwitchesPorts[switchID].Port,driverHWSwitchesPorts[switchID].Pin); // Set desired pin to desired state 
};
