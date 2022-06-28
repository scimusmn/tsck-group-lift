#pragma once

#define ERROR_MUX_FAILURE 100,100

void blinkError(int on, int off) {
	while(1) {
		digitalWrite(13, 1);
		delay(on);
		digitalWrite(13, 0);
	}
}
