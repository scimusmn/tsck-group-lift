#pragma once

#include <Arduino.h>
#include <SoftwareSerial.h>

#ifndef SERIAL_DUMMY
#define SERIAL_DUMMY 12
#endif

class RedLion {
	protected:
	SoftwareSerial serial;

	public:
	RedLion(int txPin) : serial(SERIAL_DUMMY, txPin) {
		serial.begin(9600);
	}

	showValue(int value) {
		if (value == 0) serial.print("O"); // 0 can't be moved :c
		else serial.print(value);
		serial.println(" ");
	}
};
