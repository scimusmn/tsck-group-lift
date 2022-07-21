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


/** defines a single load cell unit package */
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
				// load cell initialized correctly so we can configure it
				loadCell.setSampleRate(NAU7802_SPS_320);
				loadCell.setCalibrationFactor(calibrationFactor);
				loadCell.calculateZeroOffset();
				loadCell.setGain(NAU7802_GAIN_64);
			}
			else {
				// load cell did not initialize correctly!
				Serial.print("WARNING: load cell on mux port ");
				Serial.print(port);
				Serial.println(" failed to initialized!");
			}
		}

		/** Calibrate the load cell
		 *
		 * This function should be called when there is a force equivalent to
		 * CALIBRATION_FORCE on the load cell. It returns the computed
		 * calibration factor.
		 */
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


/** combines multiple load cell units into a single object */
class CombinedUnits : public smm::Button {
	protected:
		LiftUnit *units[5];
		IntervalTimer calibrationTimer;
	
	public:
		CombinedUnits(int calibrationButtonPin) 
			: Button(calibrationButtonPin),
			  calibrationTimer(5000)
			{}

		/* start the calibration timer when holding the button */
		void onPress() {
			Serial.println("hold for calibration...");
			calibrationTimer.start();
		}

		/* stop the calibration timer when the button is released */
		void onRelease() {
			calibrationTimer.stop();
		}

		void setup(QWIICMUX& mux) {
			// load the calibration values from EEPROM
			float calibrationValues[5];
			for (int i=0; i<5; i++) {
				size_t address = i * sizeof(float);
				float f;
				EEPROM.get(address, f);
				if (isnan(f)) f = 0; // bad values are loaded as 0 instead
				// zeroes are treated as 10,000 so that bad data
				// or a fresh arduino has reasonable defaults for calibration factors
				calibrationValues[i] = f == 0 ? 10000 : f;
			}

			// set up the units
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
			// update the button-checking code (i know this is ugly -- i'm sorry, i was out of time)
			update();

			// check if we should calibrate
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
					// we should not calibrate
					Serial.println("exactly one load cell must be lifted for calibration to succeed!");
					for (int i=0; i<5; i++) Serial.println(active[i]);
				}
				else {
					// we should calibrate!
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

			// update the units
			for (int i=0; i<5; i++) {
				units[i]->update();
			}
		}


		// get the total force on all load cells
		float getForce() {
			float total = 0;
			for (int i=0; i<5; i++) {
				total += units[i]->getForce();
			}
			return total;
		}


		// populate the array with which load cells are presently active
		void getActive(bool active[5]) {
			for (int i=0; i<5; i++) {
				active[i] = units[i]->isActive();
			}
			return active;
		}
};
