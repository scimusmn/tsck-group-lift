#pragma once

#include <SparkFun_Qwiic_Scale_NAU7802_Arduino_Library.h>
#include <SparkFun_I2C_Mux_Arduino_Library.h>

class LiftUnit {
	protected:
		QWIICMUX& mux;
		NAU7802 loadCell;
		int port;
		int proximityPin;
		bool active;
		bool initialized;
		float force;


	public:
		LiftUnit(QWIICMUX& mux, int port, int proxPin) 
			: mux(mux), port(port), proximityPin(proxPin), 
			  active(false), initialized(false)
		{}


		void setup() {
			/* prox sensor */
			pinMode(proximityPin, INPUT_PULLUP);

			/* load cell */
			mux.setPort(port);
			if (loadCell.begin())
				initialized = true;
			else {
				Serial.print("WARNING: load cell on mux port ");
				Serial.print(port);
				Serial.println(" failed to initialized!");
			}
		}
	
		
		void update() {
			if (!initialized) return;

			bool active = !digitalRead(proximityPin);
			if (active) {
				mux.setPort(port);
				force = loadCell.getWeight();
			}
		}


		bool isActive() {
			return active;
		}


		float getForce() {
			return force;
		}
};
