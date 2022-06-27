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


	void showFloor(bool s1, bool s2, bool s3, bool s4, bool s5) {
		byte b = 0;
		if (s1 || s2 || s3 || s4 || s5)
			// at least one is on
			b |= 1;

		if (s1) b |= 0x20;
		if (s2) b |= 0x10;
		if (s3) b |= 0x08;
		if (s4) b |= 0x04;
		if (s5) b |= 0x02;

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


	void show(bool s1, bool s2, bool s3, bool s4, bool s5, int barLevel) {
		digitalWrite(latchPin, 0);
		fillBar(barLevel);
		showFloor(s1,s2,s3,s4,s5);
		digitalWrite(latchPin, 1);
	}
};


