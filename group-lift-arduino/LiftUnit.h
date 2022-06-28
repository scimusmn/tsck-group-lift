#pragma once

#include <SparkFun_Qwiic_Scale_NAU7802_Arduino_Library.h>
#include <SparkFun_I2C_Mux_Arduino_Library.h>
#include "IntervalTimer.h"

class LiftUnit {
	protected:
		QWIICMUX& mux;
		NAU7802 loadCell;
		IntervalTimer zeroTimer;
		float calibrationFactor;
		int port;
		int proximityPin;
		bool active;
		bool initialized;
		float force;


	public:
		LiftUnit(QWIICMUX& mux, float calibration, int port, int proxPin) 
			: mux(mux), calibrationFactor(calibration),
			  port(port), proximityPin(proxPin), 
			  active(false), initialized(false),
			  zeroTimer(3000)
		{}


		void setup() {
			/* prox sensor */
			pinMode(proximityPin, INPUT_PULLUP);

			/* load cell */
			mux.setPort(port);
			if (loadCell.begin()) {
				initialized = true;
				loadCell.setSampleRate(NAU7802_SPS_320);
				loadCell.setCalibrationFactor(calibrationFactor);
				loadCell.calculateZeroOffset();
				loadCell.setGain(NAU7802_GAIN_64);
			}
			else {
				Serial.print("WARNING: load cell on mux port ");
				Serial.print(port);
				Serial.println(" failed to initialized!");
			}
		}
	
		
		void update() {
			if (!initialized) return;

			/* read proximity sensor */
			bool proximity = digitalRead(proximityPin);

			/* check if handle was just dropped */
			if (active && !proximity) zeroTimer.start(); // set 3-second timeout for zeroing

			/* update active from proximity */
			active = proximity;

			/* check if we should zero the scale yet */
			if (zeroTimer.triggered()) loadCell.calculateZeroOffset();

			/* get load cell value */
			if (active) {
				zeroTimer.stop();
				mux.setPort(port);
				force = loadCell.getWeight();
			}
		}


		bool isActive() {
			return active;
		}


		float getForce() {
			if (!active) return 0;
			return force;
		}
};
