#include <Arduino.h>
#include <Wire.h>
#include <SparkFun_I2C_Mux_Arduino_Library.h>
#include "BlinkError.h"
#include "LiftUnit.h"
#include "LedArray.h"

#define LED_DATA 11
#define LED_CLOCK 13
#define LED_LATCH 8

QWIICMUX mux;

LedArray leds(LED_DATA, LED_CLOCK, LED_LATCH);
LiftUnit unit1(mux, 0, 6);
LiftUnit unit2(mux, 1, 5);
LiftUnit unit3(mux, 2, 4);
LiftUnit unit4(mux, 3, 3);
LiftUnit unit5(mux, 4, 2);


void setup() {
	Serial.begin(115200);
	Wire.begin();

	if (!mux.begin()) {
		Serial.println("FATAL: mux failed to initialize!");
		blinkError(ERROR_MUX_FAILURE);
	}
	Serial.println("mux initialized");

	unit1.setup();
	unit2.setup();
	unit3.setup();
	unit4.setup();
	unit5.setup();
	Serial.println("lift units initialized");

	/* blank leds */
	leds.show(0, 0, 0, 0, 0, 0);
}


void loop() {
	unit1.update();
	unit2.update();
	unit3.update();
	unit4.update();
	unit5.update();

	leds.show(
		unit1.isActive(),		
		unit2.isActive(),
		unit3.isActive(),
		unit4.isActive(),
		unit5.isActive(), 
		10
	);
}
