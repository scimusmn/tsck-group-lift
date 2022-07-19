#ifndef SMM_BUTTON_H
#define SMM_BUTTON_H

#include <Arduino.h>

namespace smm {
	class Button {
		private:
		unsigned int buttonPin;
		bool inverted;
		bool isPressed;
		unsigned long debounceStartTime;
		unsigned long debounceTime;
		bool debouncing;

		bool getState() {
			bool state = !digitalRead(buttonPin);
			if (inverted)
				return !state;
			return state;
		}

		bool debounceDone() {
			return millis() - debounceStartTime > debounceTime;
		}

		public:

		Button(unsigned int pin, unsigned long debounceTime=5, bool inverted=false) :
			buttonPin(pin), inverted(inverted), debounceTime(debounceTime), debouncing(false)
	       	{
			pinMode(buttonPin, INPUT_PULLUP);
			isPressed = getState();
		}

		virtual void onPress() {}
		virtual void onRelease() {}
		virtual void whilePressed() {}
		virtual void whileReleased() {}

		void update() {
			if (debouncing) {
				if (debounceDone()) {
					isPressed = getState();
					debouncing = false;
					if (isPressed)
						onPress();
					else
						onRelease();
				}
					
			}
			else { /* not debouncing */
				bool state = getState();
				if (state != isPressed) {
					debouncing = true;
					debounceStartTime = millis();
				}
			}

			if (isPressed)
				whilePressed();
			else
				whileReleased();
		}

	};
}

#endif
