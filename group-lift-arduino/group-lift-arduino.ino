#include <Arduino.h>
#include <Wire.h>
#include <SparkFun_I2C_Mux_Arduino_Library.h>
#include "BlinkError.h"
#include "LiftUnit.h"
#include "LedArray.h"
#include "RedLion.h"

#define LED_DATA 11
#define LED_CLOCK 13
#define LED_LATCH 8

#define CALIBRATION_BUTTON_PIN 7

QWIICMUX mux;

LedArray leds(LED_DATA, LED_CLOCK, LED_LATCH);
RedLion lcd(9);

CombinedUnits units(CALIBRATION_BUTTON_PIN);


void setup() {
	Serial.begin(115200);
	Wire.begin();

	if (!mux.begin()) {
		Serial.println("FATAL: mux failed to initialize!");
		blinkError(ERROR_MUX_FAILURE);
	}
	Serial.println("mux initialized");

	units.setup(mux);
	Serial.println("lift units initialized");

	/* blank leds */
	bool allOff[] = { false, false, false, false, false };
	leds.show(allOff, 0);

	lcd.showValue(0);
}


void loop() {
	units.update();

	float force = units.getForce();
	int barLevel = 24 * (force/500);

	bool active[5];
	units.getActive(active);
	leds.show(active, barLevel);
}
