group-lift-arduino
==================


This program reads force input from five load cells (using five proximity sensors to determine which load cells are active) and displays the sum of the forces on both an LED bar graph and a seven-segment display. More information on the physical structure and purpose of the exhibit are available [on the drive](https://drive.google.com/drive/folders/1PwfZByLqBSv9t2nPDFGv4dqbB62W9nZh).


Overview
--------

There are three principal objects: `leds`, which controls the LED bar graph; `lcd`, which controls the seven-segment display; and `units`, which controls the load cells and proximity sensors. These interact within the main `loop()` function.

  * `BlinkError.h`: contains a single function to blink out error codes
  * `Button.h`: [arduino-base] boilerplate code, used to operate both the proximity sensors and the calibration switch
  * `IntervalTimer.h`: [arduino-base] boilerplate code, used to control calibration timing
  * `LedArray.h`: defines the `Leds` class
  * `LiftUnit.h`: defines the various classes incorporated into the `CombinedUnits` class (used for the `units` object)
  * `RedLion.h`: defines the `RedLion` class (of which `lcd` is an instance)
  * `group-lift-arduino.ino`: the main sketch file
