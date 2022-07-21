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
	// initialize buses
	Serial.begin(115200);
	Wire.begin();

	// initialize multiplexer
	if (!mux.begin()) {
		Serial.println("FATAL: mux failed to initialize!");
		blinkError(ERROR_MUX_FAILURE);
	}
	Serial.println("mux initialized");

	// initialize lift units
	units.setup(mux);
	Serial.println("lift units initialized");

	// blank leds
	bool allOff[] = { false, false, false, false, false };
	leds.show(allOff, 0);
	lcd.showValue(100);
}


void loop() {
	// update lift units
	units.Update();

	float force = units.getForce();
	int barLevel = 24 * (force/500);

	// determine which units are being used
	bool active[5];
	units.getActive(active);
	// update the LEDs
	leds.show(active, barLevel);
	// update the lcd
	lcd.showValue(force);
	delay(100);
}
