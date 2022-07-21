#pragma once

#include <math.h>
#include <EEPROM.h>
#include <SparkFun_Qwiic_Scale_NAU7802_Arduino_Library.h>
#include <SparkFun_I2C_Mux_Arduino_Library.h>
#include "IntervalTimer.h"
#include "Button.h"


#define UNIT_0_MUX_PORT 0
#define UNIT_1_MUX_PORT 1
#define UNIT_2_MUX_PORT 2
#define UNIT_3_MUX_PORT 3
#define UNIT_4_MUX_PORT 4

#define UNIT_0_PROXIMITY_PORT 6
#define UNIT_1_PROXIMITY_PORT 5
#define UNIT_2_PROXIMITY_PORT 4
#define UNIT_3_PROXIMITY_PORT 3
#define UNIT_4_PROXIMITY_PORT 2


#define CALIBRATION_FORCE 50.0


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
			Serial.print("initializing load cell on mux port ");
			Serial.print(port);
			Serial.print(" with calibration factor ");
			Serial.println(calibrationFactor);
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

		float calibrate() {
			mux.setPort(port);
			loadCell.calculateCalibrationFactor(CALIBRATION_FORCE, 64);
			return loadCell.getCalibrationFactor();
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
			if (zeroTimer.triggered()) {
				mux.setPort(port);
				loadCell.calculateZeroOffset();
			}

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


class CombinedUnits : public smm::Button {
	protected:
		LiftUnit *units[5];
		IntervalTimer calibrationTimer;
	
	public:
		CombinedUnits(int calibrationButtonPin) 
			: Button(calibrationButtonPin),
			  calibrationTimer(5000)
			{}

		void onPress() {
			Serial.println("hold for calibration...");
			calibrationTimer.start();
		}

		void onRelease() {
			calibrationTimer.stop();
		}

		void setup(QWIICMUX& mux) {
			float calibrationValues[5];
			for (int i=0; i<5; i++) {
				size_t address = i * sizeof(float);
				float f;
				EEPROM.get(address, f);
				if (isnan(f)) f = 0;
				calibrationValues[i] = f == 0 ? 10000 : f;
			}
			units[0] = new LiftUnit(
				mux, 
				calibrationValues[0], 
				UNIT_0_MUX_PORT,
				UNIT_0_PROXIMITY_PORT
			);
			units[0]->setup();
			units[1] = new LiftUnit(
				mux, 
				calibrationValues[1], 
				UNIT_1_MUX_PORT,
				UNIT_1_PROXIMITY_PORT
			);
			units[1]->setup();
			units[2] = new LiftUnit(
				mux, 
				calibrationValues[2], 
				UNIT_2_MUX_PORT,
				UNIT_2_PROXIMITY_PORT
			);
			units[2]->setup();
			units[3] = new LiftUnit(
				mux, 
				calibrationValues[3], 
				UNIT_3_MUX_PORT,
				UNIT_3_PROXIMITY_PORT
			);
			units[3]->setup();
			units[4] = new LiftUnit(
				mux, 
				calibrationValues[4], 
				UNIT_4_MUX_PORT,
				UNIT_4_PROXIMITY_PORT
			);
			units[4]->setup();
		}


		void Update() {
			update();
			if (calibrationTimer.triggered()) {
				bool active[5];
				getActive(active);
				// make sure exactly one load cell is active
				int numActive = 0;
				int activeIndex = 0;
				for (int i=0; i<5; i++) {
					if (active[i]) { 
						numActive += 1;
						activeIndex = i;
					}
				}

				if (numActive != 1) {
					Serial.println("exactly one load cell must be lifted for calibration to succeed!");
					for (int i=0; i<5; i++) Serial.println(active[i]);
				}
				else {
					float factor = units[activeIndex]->calibrate();
					Serial.print("calibrated unit ");
					Serial.print(activeIndex);
					Serial.print(" with factor ");
					Serial.println(factor);
					// save calibration to EEPROM
					size_t address = activeIndex * sizeof(float);
					EEPROM.put(address, factor);
				}
			}
			for (int i=0; i<5; i++) {
				units[i]->update();
			}
		}


		float getForce() {
			float total = 0;
			for (int i=0; i<5; i++) {
				total += units[i]->getForce();
			}
			return total;
		}

	
		void getActive(bool active[5]) {
			for (int i=0; i<5; i++) {
				active[i] = units[i]->isActive();
			}
			return active;
		}
};
