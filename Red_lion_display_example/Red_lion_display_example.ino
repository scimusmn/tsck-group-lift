//TSCK component #411 How much can you lift
//D.Bailey, 4/6/21
//Example code for sending data to Red Lion LED 4" display.
//The last serial data to be sent must always be a println 
//to include the <CR> delimiter. Max characters to diplay is 6.
//A pot is connected to A0 to simulate analog values from a load cell.
//Sending a value of "0" will always display it at the rightmost digit.
//If it is neccessary to have the zero be anywhere other than the rightmost digit
//send a letter "O" instead, then send additional space characters as needed.  
//The letter "O" and "0" will look the same.

#include <SoftwareSerial.h>

SoftwareSerial LEDdisplay(10, 11); // RX, TX
int reading = 0;

void setup() {
  LEDdisplay.begin(9600);
}
void loop() {
  reading = analogRead(A0);
  if (reading > 0) {
    LEDdisplay.print(reading); //display intger value
    LEDdisplay.println(" "); //shift value over one space to be more centered
  }
  else {
    LEDdisplay.print("O"); //send letter "O", the value "0" can't be shifted to the left
    LEDdisplay.println(" "); //move the letter over 1 space
  }
  delay(100); //Some delay is needed to keep display from displaying garbage, 100ms min.
}
