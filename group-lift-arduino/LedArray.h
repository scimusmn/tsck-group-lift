#pragma once

class LedArray {
	protected:
	int dataPin, clockPin, latchPin;

	public:
	LedArray(int dataPin, int clockPin, int latchPin) :
		dataPin(dataPin), clockPin(clockPin), latchPin(latchPin)
	{
		pinMode(dataPin, OUTPUT);
		pinMode(clockPin, OUTPUT);
		pinMode(latchPin, OUTPUT);
	}


	void showFloor(bool active[5]) {
		byte b = 0;
		if (active[0] || active[1] || active[2] || active[3] || active[4])
			// at least one is on
			b |= 1;

		if (active[0]) b |= 0x20;
		if (active[1]) b |= 0x10;
		if (active[2]) b |= 0x08;
		if (active[3]) b |= 0x04;
		if (active[4]) b |= 0x02;

		shiftOut(dataPin, clockPin, LSBFIRST, b);
	}

	void fillBar(int level)
	{
		for (int i=0; i<3; i++) {
			byte b = 0;
			for (int j=0; j<8; j++) {
				int index = 8*(2-i) + j;
				if (index < level) b |= (1<<j);
			}
			shiftOut(dataPin, clockPin, MSBFIRST, b);
		}
	}


	void show(bool active[5], int barLevel) {
		digitalWrite(latchPin, 0);
		fillBar(barLevel);
		showFloor(active);
		digitalWrite(latchPin, 1);
	}
};


