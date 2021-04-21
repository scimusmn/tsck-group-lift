# Read-comma-separated-values-from-MicroSD-card-into an Arduino
This example code demonstrates how to read comma separated values into a Arduino from a text file stored on a MicroSD card.  This is useful in stiuations were it is desired to use preloaded configuration data, such as load cell calibration factors, timer presets, etc., and make modifications to those values easily without having to update the code in the IDE. This function uses a "comma" as a dilimeter to parse the characters, then the characters are converted into float values and stored into a user defined array.

The comma stored values in the .txt file need to be in the following format, with no spaces or lines before the first value, or in between values. Be sure to include the comma on the last value, or it won't be parsed. The number of values can be scaled up or down.

0.0,1.1,2.2,4.3,

The card reader being used in this example is an Adafruit 5V ready Micro-SD Breakout Board.  https://www.adafruit.com/product/254.  Follow the hook-up example for Arduino.

Adafruit highly recommeds using the offical SD Association formatting software for formatting SD cards, as a lot of formatting issues can be avoided doing so. The software can be found here: https://www.sdcard.org/downloads/formatter/  
