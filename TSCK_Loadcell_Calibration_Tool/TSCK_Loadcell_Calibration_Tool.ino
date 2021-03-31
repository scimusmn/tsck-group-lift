//Load cell calibration tool for TSCK Group Lift component
//D.Bailey
//Science Museum of MN
//3/17/20
//This tool is used to determine the calibration value for the 500kg load cells
//used in the TSCK Group Lift component.
//Calibration weight is set at 50kg
//Instructions are provided on a 20x4 LCD screen.

//defines
#define pushButton 12

//libraries
#include <Wire.h>
#include "Adafruit_LiquidCrystal.h"
Adafruit_LiquidCrystal lcd(3, 2, 4); //clock, data, latch pins for Adafruit LCD backpack, in SPI mode
#include "SparkFun_Qwiic_Scale_NAU7802_Arduino_Library.h" // Click here to get the library: http://librarymanager/All#SparkFun_NAU8702
NAU7802 myScale; //Create instance of the NAU7802 class

//variables
float calibrationWeight = 50.0;
uint8_t debounce=20;

void setup()
{
  pinMode(pushButton, INPUT_PULLUP);
  lcd.begin(20, 4);
  Wire.begin();
  myScale.begin();
  lcd.setCursor(0, 0);
  lcd.print("Loadcell Calibration");
  lcd.setCursor(0, 1);
  lcd.print("  Value Calculator");
  lcd.setCursor(0, 3);
  lcd.print("   Starting");
  for (byte i = 0; i < 6; i++) {
    delay(300);
    lcd.print(".");
  }
}

void calibrateScale() {
  lcd.clear();
  lcd.print ("SETUP LOAD CELL WITH");
  lcd.setCursor(0, 1);
  lcd.print ("NO WEIGHT ON IT.");
  lcd.setCursor(0, 2);
  lcd.print ("PRESS BUTTON WHEN");
  lcd.setCursor (0, 3);
  lcd.print ("READY.");
  while (digitalRead (pushButton));
  lcd.clear();
  delay(debounce); //button debounce
  while (!digitalRead (pushButton));
  myScale.calculateZeroOffset(64); //Zero or Tare the scale. Average over 64 readings.
}

void addWeight() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ADD 50kg. MAKE");
  lcd.setCursor(0, 1);
  lcd.print("SURE THE WEIGHT");
  lcd.setCursor(0, 2);
  lcd.print("IS STILL! PUSH");
  lcd.setCursor(0, 3);
  lcd.print("BUTTON WHEN READY.");
  while (digitalRead (pushButton));
  delay(debounce); //button debounce
  while (!digitalRead (pushButton));
}

void getCalFactor() {
  lcd.clear();
  myScale.calculateCalibrationFactor(calibrationWeight, 64); //Tell the library how much weight is currently on it
  lcd.print("CAL. = ");
  lcd.print(myScale.getCalibrationFactor(), 2);
  lcd.setCursor(0, 1);
  lcd.print("WEIGHT = ");
  lcd.print(myScale.getWeight(), 1);
  lcd.setCursor(0, 2);
  lcd.print("PRESS BUTTON TO");
  lcd.setCursor(0, 3);
  lcd.print("START OVER.");
  while (digitalRead (pushButton));
  lcd.clear();
  delay(debounce); //button debounce
  while (!digitalRead (pushButton));
}

void loop() {
  calibrateScale();
  addWeight();
  getCalFactor();
}
