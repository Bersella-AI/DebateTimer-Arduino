# Debate Timer @Arduino
**FIRST REPOSITORY OF iDEMKORS**
A timer for debate competition based on Arduino

Arduino上的辩论赛计时器 - iDemkors第一个Git项目

### CONCLUSION
+ This is an experimental Arduino project "Beta". An Ocrobot MEGA 2560 board (an alternative to Arduino MEGA 2560) with variants of hardware modules is programmed to make the timer.
+ The current developed version of .ino source program is Beta 3.
+ More details will be announced after the release of final version.

### HARDWARE REQUIREMENT (by default)
+ Arduino (Genuino or other compatible vendors) MEGA 2560 board;
+ LCD 2004 screen (those with I2C support is RECOMMENDED);
+ 8-digit LED module (based on MAX7219);
+ 6 individual LEDs (2*red/2*yellow/2*green) OR 2 RGB LEDs;
+ 2 or 3 groups of a button, a 1N4148 diode, and a 100nF capacitor;
+ A breadboard and wires.
(Nokia 5110 screen may be added in the next versions...)

### LIBRARIES REQUIREMENT (by default)
+ LedControl (by Eberhard Fahle)
+ LiquidCrystal_I2C (by Marco Schwartz)

### Update Beta 1 & 2 (10 Mar, 2016)
Beta 1 is written for the test of an 8-digit LED module and an 8*8 LED matrix, both based on MAX7219 chip. It will light all the LEDs availible.
Beta 2, adding a button on the base of Beta 1, is designed to test 2 timers using built-in functions ("millis"), with a button to switch the timer to run.

### Beta 3 has passed the test (10 Mar, 2016)
