#include <Arduino.h>
#include <Wire.h>
#include <SparkFun_I2C_Mux_Arduino_Library.h>
#include "LiftUnit.h"
#include "LedArray.h"

#define LED_DATA 11
#define LED_CLOCK 13
#define LED_LATCH 8

QWIICMUX mux;

LedArray leds(LED_DATA, LED_CLOCK, LED_LATCH);
LiftUnit unit1(mux, 0, 6);


void setup() {
	Serial.begin(115200);
/*	Wire.begin();

	if (!mux.begin()) {
		Serial.println("FATAL: mux failed to initialize!");
		while(1); // freeze arduino
	}
	Serial.println("mux initialized");

	unit1.setup();
	Serial.println("unit 1 initialized");*/
}


void loop() {
	for (int i=0; i<24; i++) {
		Serial.println(i);
		leds.show(0,0,0,0,0, i);
		delay(500);
	}
}
