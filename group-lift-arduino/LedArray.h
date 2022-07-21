#pragma once

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * LedArray controls the power shift registers that are chained to
 * control all of the LED strips in the exhibit.
 *
 * There are two sets of LED strips: the floor strips and the bar
 * graph. There are six floor strips, arranged in a tree. The "trunk"
 * strip emerges from the base of the bar graph, and then splits
 * into the five "branch" strips, each of which connects to one of
 * the load cell units. Whenever force is applied to one of the
 * units, the the corresponding "branch" should light up, and if any
 * branch is lit the "trunk" should be lit as well.
 *
 * The floor strips are controlled by the first shift register, and
 * the bar graph is controlled by the subsequent three shift registers.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

class LedArray {
	protected:
	/* shift register communication pins */
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
		// check if we should light the trunk strip
		if (active[0] || active[1] || active[2] || active[3] || active[4])
			// at least one is on
			b |= 1;

		// check the branch strips
		if (active[0]) b |= 0x20;
		if (active[1]) b |= 0x10;
		if (active[2]) b |= 0x08;
		if (active[3]) b |= 0x04;
		if (active[4]) b |= 0x02;

		// send byte to the shift register
		shiftOut(dataPin, clockPin, LSBFIRST, b);
	}

	void fillBar(int level)
	{
		// step over the three shift registers
		for (int i=0; i<3; i++) {
			byte b = 0; // each bit is 0 by default!
			// step over each bit
			for (int j=0; j<8; j++) {
				int index = 8*(2-i) + j;
				// check if the bit should changed to 1
				if (index < level) b |= (1<<j);
			}
			// send the byte to the shift register
			shiftOut(dataPin, clockPin, MSBFIRST, b);
		}
	}


	void show(bool active[5], int barLevel) {
		digitalWrite(latchPin, 0);
		// bar first (because the bar registers are further down the chain)
		fillBar(barLevel);
		showFloor(active);
		digitalWrite(latchPin, 1);
	}
};


