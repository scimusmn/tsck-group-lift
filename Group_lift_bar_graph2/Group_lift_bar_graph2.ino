//Example code to map a Dotstar LEd strip to values from 0-1023 from a potentiometer

#include <Adafruit_DotStar.h>
#include <SPI.h>

#define NUMPIXELS 60 // Number of LEDs in strip
#define pot 0 //analog input pin where potentiometer is connected to

// Here's how to control the LEDs from any two pins:
#define DATAPIN    4
#define CLOCKPIN   5

Adafruit_DotStar strip(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);
// The last parameter is optional -- this is the color data order of the
// DotStar strip, which has changed over time in different production runs.
// Your code just uses R,G,B colors, the library then reassigns as needed.
// Default is DOTSTAR_BRG, so change this if you have an earlier strip.
// Hardware SPI is a little faster, but must be wired to specific pins
// (Arduino Uno = pin 11 for data, 13 for clock, other boards are different).
//  Adafruit_DotStar strip(NUMPIXELS, DOTSTAR_BRG);

long timenow = 0;
int newpeak = 0;
int currentled = 0;

void setup() {
  //Serial.begin(9600);
  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP
}

void loop() {
  currentled = map (analogRead(pot), 0, 1023, 0, 59);
  if (currentled > newpeak) {
    newpeak = currentled;
    timenow = millis();
  }
  for (int i = 0; i < strip.numPixels(); i++) {
    if (i < currentled) {
      strip.setPixelColor(i, strip.Color(100,  0,  0));
    }
    else strip.setPixelColor(i, 0);
  }
  strip.setPixelColor(newpeak, strip.Color(0,  200,  0));

  if (millis() >= timenow + 2000) {
    strip.setPixelColor(newpeak, 0);
    newpeak = 0;
  }
  strip.show(); //turn on pixels
  //Serial.println(newpeak);
}
