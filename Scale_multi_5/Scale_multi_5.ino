// Code adapted and modified from Qwicc Mux Arduino Library example 2
// https://github.com/sparkfun/SparkFun_I2C_Mux_Arduino_Library/blob/master/examples/Example2_DualDistance/Example2_DualDistance.ino
// written by Nathan Seidle @ SparkFun Electronics
// Date: May 17th, 2020
// Modified by David Bailey, Science Museum of MN
// Date: 4/20/2021

#define num_of_scales 5

#include <Wire.h>
#include <SparkFun_I2C_Mux_Arduino_Library.h> //Click here to get the library: http://librarymanager/All#SparkFun_I2C_Mux
#include <SparkFun_Qwiic_Scale_NAU7802_Arduino_Library.h> //Click here to get the library: http://librarymanager/All#SparkFun_NAU8702
QWIICMUX myMux; //instance of Qwiic Mux class
NAU7802 **scale; //create pointer to a set of pointers to the NAU7802 class

//Variables

bool scaleTared [num_of_scales] = {0, 0, 0, 0, 0}; //number of elements must match number of scales
int proxSensePin [num_of_scales] = {2, 3, 4, 5, 6}; //lift handle postion proximity sensor pins
long timeStamp[num_of_scales] = {0, 0, 0, 0, 0}; //number of elements must match number of scales
float CFactor[num_of_scales] = {4357.9, 10899.67, 10857.56, 10899.67, 10899.67}; //array that holds the individual load cell calibration factors
float totalWeight = 0;
long tareDelay = 2000;

//Setup
void setup() {
  Serial.begin(9600);
  Wire.begin();
  scale = new NAU7802 *[num_of_scales];
  //create instances of NAU7802 class, and set-up input pins for tare hall sensors
  for (int x = 0; x < num_of_scales; x++) {
    scale[x] = new NAU7802;
    pinMode(proxSensePin[x], INPUT_PULLUP); //set inputs used for hall sensor taring
  }
  //initialize Mux Shield board
  if (myMux.begin() == false)
  {
    //Serial.println("Mux not detected. Program halted...");
    while (1);
  }
  //Serial.println("Mux detected");
  bool initSuccess = true;

  //Configure the scales
  for (byte x = 0; x < num_of_scales; x++)
  {
    myMux.setPort(x);
    if (scale[x]->begin() != 1) //Begin returns 1 on a good init
    {
      //Serial.print("Sensor ");
      //Serial.print(x);
      //Serial.println(" did not begin! Check wiring");
      initSuccess = false;
    }
    else
    {
      scale[x]->setSampleRate(NAU7802_SPS_320);
      scale[x]->calibrateAFE(); //this line may not be neccesary
      scale[x]->setCalibrationFactor(CFactor[x]); //This is where the calibration factors are read from the array
      scale[x]->calculateZeroOffset(64);
      scale[x]->setGain(NAU7802_GAIN_64);
      //Serial.print("Scale ");
      //Serial.print(x);
      //Serial.println(" configured");
      //Serial.println(CFactor[x]);
      delay(500);
    }
  }
  if (initSuccess == false)
  {
    //Serial.print("Program stopped...");
    while (1);
  }
  //Serial.println("Mux Shield initalized");
}

//Main
void loop() {
  totalWeight = 0;
  for (byte x = 0; x < num_of_scales; x++) {
    myMux.setPort(x);
    if (!digitalRead(proxSensePin[x]) && !scaleTared[x] && millis() > timeStamp[x] + tareDelay) { //Tare only one time while handle is down, and 3 seconds have passed
      scale[x]->calculateZeroOffset();
      scaleTared[x] = 1;
      //Serial.print("Scale: ");
      //Serial.print(x);
      //Serial.println(" tared");
    }
    if (digitalRead(proxSensePin[x])) {
      scaleTared[x] = 0; //allow weight to update if handle is lifted
      totalWeight = scale[x]->getWeight() + totalWeight;
      timeStamp[x] = millis();
    }
  }
  Serial.println(totalWeight, 1); //will already be scaled to kg based on calibration factor
  delay(100); //delay needed to allow Red Lion LED display to update
}
