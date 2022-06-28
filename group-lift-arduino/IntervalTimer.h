#pragma once

#include <Arduino.h>

class IntervalTimer {
	public:
	IntervalTimer(unsigned long milliseconds) {
		interval = milliseconds;
		active = false;
	}

	void start() {
		active = true;
		startTime = millis();
	}

	void stop() {
		active = false;
	}

	bool triggered() {
		if (!active) return false;

		if (millis() - startTime > interval) {
			active = false;
			return true;
		}
		return false;
	}

	protected:
		unsigned long startTime, interval;
		bool active;
};
