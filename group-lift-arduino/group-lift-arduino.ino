#include <Arduino.h>

#define LED_DATA 4
#define LED_CLOCK 6
#define LED_LATCH 5


void shiftArray(int data, int clock, int order, byte *b, int n)
{
	for (int i=0; i<n; i++) {
		Serial.print(b[i], HEX);
		Serial.print(' ');
		shiftOut(data, clock, order, b[i]);
	}
}


class leds {
	protected:
	int dataPin, clockPin, latchPin;

	public:
	leds(int dataPin, int clockPin, int latchPin) :
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

	void fillBar(int n)
	{
		byte array[3];
		for (int i=0; i<24; i++) {
			int byteIndex = 2 - (i/8);
			int bitIndex = i % 8;
			if (i <= n) {
				array[byteIndex] |= (1 << bitIndex);
			}
			else {
				array[byteIndex] &= ~(1 << bitIndex);
			}
		}
		
		shiftArray(dataPin, clockPin, MSBFIRST, array, 3);
	}


	void show(bool s1, bool s2, bool s3, bool s4, bool s5, int barLevel) {
		digitalWrite(latchPin, 0);
		fillBar(barLevel);
		showFloor(s1,s2,s3,s4,s5);
		digitalWrite(latchPin, 1);
	}
} l(LED_DATA, LED_CLOCK, LED_LATCH);


void transmitBit(bool bit) {
	digitalWrite(LED_CLOCK, 0);
	digitalWrite(LED_DATA, bit);
	digitalWrite(LED_CLOCK, 1);
}


void setup() {
	Serial.begin(115200);
}


void loop() {
	for (int i=0; i<24; i++) {
		Serial.println(i);
		l.show(i%2, i%3, i%4, i%5, i%6, i);
		delay(500);
	}
}
