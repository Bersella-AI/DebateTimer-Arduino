# Debate Timer @Arduino
**FIRST REPOSITORY OF iDEMKORS**

A timer for debate competition based on Arduino

Arduino上的辩论赛计时器 - iDemkors第一个Git仓库

### CONCLUSION
+ This is an experimental Arduino project "Beta". An Ocrobot MEGA 2560 board (an alternative to Arduino Mega 2560) with variants of hardware modules is programmed to make the timer.
+ The current developed version of .ino source program is Beta 3.
+ More details will be announced guadually. If any mistake in this document or in the code is committed, please contact me.

### SYSTEM REQUIREMENT (by default)
* HARDWARE
  + Arduino (Genuino or other compatible vendors) MEGA 2560 board;
  + LCD 2004 screen (those with I2C support is RECOMMENDED);
  + 8-digit LED module (based on MAX7219);
  + 6 individual LEDs (2*red/2*yellow/2*green) OR 2 RGB LEDs;
  + 2 or 3 groups of a button, a 1N4148 diode, and a 100nF capacitor;
  + A breadboard and wires.
  (Nokia 5110 screen may be added in the next versions...)

* LIBRARIES
  + [LedControl](http://wayoda.github.io/LedControl/) (by Eberhard Fahle)
  + [LiquidCrystal_I2C](https://github.com/marcoschwartz/LiquidCrystal_I2C) (by Marco Schwartz)
 
## FOR THOSE WHO WANT TO IMPORT
### If you are using a UNO...
The availible I/O pins on a UNO board may not be enough for the system. Here are the solutions:
  1. Use 2 RGB LEDs instead of 6 separate LEDs. 4 pins should be enough to generate red, green, and yellow lights for both sides.
  2. Use a LCD 2004/1602 screen via I2C bus, that requires only 2 I/O pins. **Remember to load "Software I2C" library.**
  3. Versions requiring 2 buttons (Beta 3 & 4) can be considered.
  4. Modify the code to save a public pin (ANY_BUTTON_PRESSED) that check whether one of the buttons is being pressed.
  5. Switch to a Mega board -- the easiest way!

### If you DON'T have a diode (not LED) or a capacitor...
Don't worry, the use of diodes aims for a faster recognition of pressing buttons. You can do Step 4 in "using a UNO" above to avoid using diodes.
And a capacitor can improve the stability when checking whether a button is being pressed. Removing it may result in LITTLE difference, buttons can still work properly (the contents on the screen may "flash" for a moment).
Taking advantage of them is RECOMMENDED, though.

## HISTORY
### Update Beta 1 & 2 (10 Mar, 2016)
Beta 1 is written for the test of an 8-digit LED module and an 8*8 LED matrix, both based on MAX7219 chip. It will light all the LEDs availible.
Beta 2, adding a button on the base of Beta 1, is designed to test 2 timers using built-in functions ("millis"), with a button to switch the timer to run.

### Update Beta 3, the first availible version (11 Mar, 2016)
I wrote the whole basic structure of the system on a piece of paper directly. And "translated" it into code once. And slightly adjusted the code 5 times. And it works!
In the interest of checking the availability of the basic structure, I minimized the hardware requirement to only 4 parts:
  + LCD 2004 screen via I2C bus
  + MAX7219-based 8-digit LED display
  + 2 buttons
  + A breadboard and wires
The 8*8 LED matrix will no longer be used since this version. Other "required" hardware modules would be added in the next versions.
But, why not have a try?
