// Code adapted and modified from Qwicc Mux Arduino Library example 2
// https://github.com/sparkfun/SparkFun_I2C_Mux_Arduino_Library/blob/master/examples/Example2_DualDistance/Example2_DualDistance.ino
// written by Nathan Seidle @ SparkFun Electronics
// Date: May 17th, 2020
// Modified by David Bailey, Science Museum of MN
// Date: 7/22/2020

#include <Wire.h>
//#include <EEPROM.h> //Needed to record user settings
#include <SparkFun_I2C_Mux_Arduino_Library.h> //Click here to get the library: http://librarymanager/All#SparkFun_I2C_Mux
#include <SparkFun_Qwiic_Scale_NAU7802_Arduino_Library.h> //Click here to get the library: http://librarymanager/All#SparkFun_NAU8702
QWIICMUX myMux;
#define num_of_scales 3
//#define LOCATION_CALIBRATION_FACTOR 0 //Float, requires 4 bytes of EEPROM
NAU7802 **scale; //create pointer to a set of pointers to the NAU7802 class
bool scaleTared [num_of_scales] = {0,0,0}; //number of elements must match number of scales
int pin [num_of_scales] = {2,3,4}; //number of elements must match number of scales
long timeStamp[num_of_scales] = {0,0,0};

float CFactor[num_of_scales] = {4357.9,10899.67,10857.56};
float totalWeight = 0;
//float calFactor = 17203.53; //divisor used convert raw values into kilograms
float settingCalibrationFactor; //Value used to convert the load cell reading to lbs or kg
long tareDelay = 2000;


void setup() {
  Serial.begin(9600);
  Wire.begin();
  //Wire.setClock(10000); //Qwiic Scale is capable of running at 400kHz if desired
  scale = new NAU7802 *[num_of_scales];
  //create instances of NAU7802 class, and set-up input pins for tare hall sensors
  for (int x = 0; x < num_of_scales; x++) {
    scale[x] = new NAU7802;
    pinMode(pin[x], INPUT_PULLUP); //set inputs used for hall sensor taring
  }
  //initialize Mux Shield board
  if (myMux.begin() == false)
  {
    Serial.println("Mux not detected. Program halted...");
    while (1)
      ;
  }
  Serial.println("Mux detected");
  bool initSuccess = true;

  //***configure the scales
  for (byte x = 0; x < num_of_scales; x++)
  {
    myMux.setPort(x);
    if (scale[x]->begin() != 1) //Begin returns 1 on a good init
    {
      Serial.print("Sensor ");
      Serial.print(x);
      Serial.println(" did not begin! Check wiring");
      initSuccess = false;
    }
    else
    {
      //EEPROM.get(LOCATION_CALIBRATION_FACTOR, settingCalibrationFactor);
      scale[x]->setSampleRate(NAU7802_SPS_320);
      scale[x]->calibrateAFE(); //may not be unneccesary
      //scale[x]->setCalibrationFactor(settingCalibrationFactor);
      scale[x]->setCalibrationFactor(CFactor[x]);
      scale[x]->calculateZeroOffset(64);
      scale[x]->setGain(NAU7802_GAIN_64);

 
      Serial.print("Scale ");
      Serial.print(x);
      Serial.println(" configured");
      //Serial.println(settingCalibrationFactor);
      Serial.println(CFactor[x]);
      delay(500);
    }
  }
  if (initSuccess == false)
  {
    Serial.print("Program stopped...");
    while (1)
      ;
  }
  Serial.println("Mux Shield initalized");
}

//*****Main*****
void loop() {
  totalWeight = 0;
  for (byte x = 0; x < num_of_scales; x++) {
    myMux.setPort(x);
    if (!digitalRead(pin[x]) && !scaleTared[x] && millis() > timeStamp[x] + tareDelay) { //Tare only one time while handle is down, and 3 seconds have passed
      scale[x]->calculateZeroOffset();
      scaleTared[x] = 1;
      //Serial.print("Scale: ");
      // Serial.print(x);
      //Serial.println(" tared");
    }
    if (digitalRead(pin[x])) {
      scaleTared[x] = 0; //allow weight to update if handle is lifted
      totalWeight = scale[x]->getWeight() + totalWeight;
      timeStamp[x] = millis();
    }
  }
  Serial.println(totalWeight, 1);
  delay(50); //slow down data to python script a bit, otherwise overfills buffer
}
