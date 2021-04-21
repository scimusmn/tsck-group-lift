//This is a boiler plate for reading comma separated values from a MicroSD card
//using the Adafruit 5V ready MicroSD breakout board.
//Values in *.txt file must be in this format below with a comma after each floating point
//and no spaces or lines before first value. Spaces 
//0.0,0.0,0.0,0.0,0.0,
//Use the numChars variable to set max number of characters for each value to limit the buffer size
//Card reader to Arduino connections: CLK=pin 13, DO=pin 12, DI=pin 11, CS=pin 10 
//3/7/2021
//Dave Bailey

//included libraries
#include <SD.h>
#include <SPI.h>

//variables
float myArray[5] = {0.0, 0.0, 0.0, 0.0, 0.0}; //array to hold parsed values, scale up or down as needed
int x = 0; //myArray array index

void setup() {
  Serial.begin(9600);
  readMicroSDcard();
  for (x = 0; x < 5; x++) { //loop to print out all the float values stored in myArray
    Serial.println(myArray[x]);
  }
}

void readMicroSDcard() { //read values from SD card, and parse into myArray array
  const byte numChars = 10; //max number of characters that comprise each of the comma separated values
  const int chipSelect = 10; //connected to pin 10 Arduino Uno/Metro to detect card presence
  char receivedChars[numChars]; //array to receive characters for each comma separated value
  char tempChars[numChars];  //array used to store temporary characters during parsing function
  if (!SD.begin(chipSelect))  //see if the card is present
  {
    Serial.println("No card found or failed to initialize");
    return;
  }
  Serial.println("Card found and initialized");
  File myFile = SD.open("test.txt"); //open .txt file on card
  if (myFile) {
    static byte index = 0;
    char incomingChar;
    while (myFile.available() > 0) {
      incomingChar = myFile.read();
      if (incomingChar != ',') { //keep receiving characters until next comma
        receivedChars[index] = incomingChar;
        index++;
      }
      else {
        receivedChars[index] = '\0'; // terminate the string
        index = 0;//reset receivedChars array index
        strcpy(tempChars, receivedChars); // this temporary copy is necessary to protect the original data
                                          // because strtok() used in parseData() replaces the commas with \0
        char * strtokIndex; // this is used by strtok() as an index
        strtokIndex = strtok(tempChars, ","); // this continues where the previous call left off at the comma
        myArray[x] = atof(strtokIndex); //convert characters to a float value, add it to current index myArray
        x++; //move to next element in myArray
      }
    }
  }
}

void loop() {
}
