# Debate Timer @Arduino
> **FIRST REPOSITORY OF iDEMKORS ON GitHub**  
A timer for debate competition based on Arduino  
Arduino上的辩论赛计时器 - iDemkors第一个GitHub仓库

### CONCLUSION
+ This is an experimental Arduino project "Beta". An Ocrobot MEGA 2560 board (an alternative to Arduino Mega 2560) with variants of hardware modules is programmed to make the timer.
+ The currently developed version of .ino source program is Beta 5.
+ The Chinese version of this document will be unveiled after uploading a blog system to my GitHub.
+ More details will be announced gradually. If any mistake in this document or in the code is committed, please contact me.

### SYSTEM REQUIREMENT (by default)
* ESSENTIAL HARDWARE (for Beta 3)
  + An Arduino (Genuino or other compatible vendors) Mega 2560 board;
  + An LCD 2004 screen (those with I2C support is RECOMMENDED);
  + A MAX7219-based 8-digit LED module;
  + 2 or 3 groups of a button, a 1N4148 diode, and a 100nF capacitor;
  + A breadboard and wires.

* ADDITIONAL HARDWARE (Beta 4 or later)
  + 6 individual LEDs (2\*red/2\*yellow/2\*green) OR 2 RGB LEDs;
  + A source-less buzzer;
  + A DS3231-based real-time clock module. (Maybe Beta 6?)
  +  (Nokia 5110 screen and EEPROM support may be added in the future versions...)

* LIBRARIES
  + [LedControl](http://wayoda.github.io/LedControl/) (by Eberhard Fahle)
  + [LiquidCrystal_I2C](https://github.com/marcoschwartz/LiquidCrystal_I2C) (by Marco Schwartz)
 
## FOR THOSE WHO WANT TO IMPORT
### If you are using a UNO...
The available I/O pins on a UNO board may not be enough for the system. Here are the solutions:

  1. Use 2 RGB LEDs instead of 6 separate LEDs. 4 pins should be enough to generate red, green, and yellow lights for both sides.
  2. Use an LCD 2004/1602 screen via I2C bus, that requires only 2 I/O pins. 
  3. Versions requiring 2 buttons (Beta 3 & 4) can be considered.
  4. Modify the code to save a public pin `ANY_BUTTON_PRESSED` that check whether one of the buttons is being pressed.
  5. Switch to a Mega board -- the easiest way!

### If you are using an LCD 1602...
(The reformation guide will be available after the release of Beta 4/5)  
\*smile\*

### If you DON'T have a diode (not LED) or a capacitor...
Don't worry, the use of diodes aims for a faster recognition of pressing buttons. You can do Step 4 in "using a UNO" above to avoid using diodes.  
And a capacitor can improve the stability when checking whether a button is being pressed. Removing it may result in LITTLE difference, buttons can still work properly (the contents on the screen may "flash" for a moment).  
Taking advantage of them is RECOMMENDED, though.

## HISTORY
### Beta 0 (Test_MAX7219) (26 Dec, 2015)
Just test an 8-digit display and an 8*8 LED matrix (both based on MAX7219).

### Update Beta 1 & 2 (27 Dec & 29 Dec, 2015)
Beta 1 aims to test the built-in timer and show a timer on the 8-digit display (reset to 0 when exceeding 999.9s).  
Beta 2, adding a button on the base of Beta 1, is designed to test 2 timers using built-in functions `millis`, with a button to switch the timer to run.

*(then 3 months passed unconsciously QAQ)*
### Update Beta 3, the first available version (11 Mar, 2016)
![Beta 3](http://ww3.sinaimg.cn/large/62fb934ajw1f1ru985pt8j21kw0w07pp.jpg)
I wrote the whole basic structure of the system on a piece of paper directly. And "translated" it into code once. And slightly adjusted the code 5 times. And it works!  
In the interest of checking the availability of the basic structure, I minimized the hardware requirement to only 4 parts:

  + LCD 2004 screen via I2C bus
  + MAX7219-based 8-digit LED display
  + 2 buttons
  + A breadboard and wires

The 8*8 LED matrix will no longer be used since this version. Other "required" hardware modules would be added in the next versions.  
But, why not have a try?(⊙▽⊙)

### Beta 4, with enhanced experience (20 Mar, 2016)
> Space occupation: 10,294 bytes  
Memory allocation: 757 bytes  
Digital pins required: 14 (8 PWM, 4 Analog In, a pair of SPI connectors)

![Beta 4](http://ww1.sinaimg.cn/large/62fb934ajw1f24anqstlxj21kw0w01f6.jpg)
***New features:***

  + 2 RGB LEDs (Common-Anode type)
  + A source-less buzzer module
  + The third button (yet to be used), paired with a diode & a capacitor
  + ...and *MORE* wires

The LEDs and the buzzer provide a better experience to prompt the time keeper more intuitively: 

  * Once starting the speech, the light corresponding to current side will turn *green*;
  * When the time remains just below 30 seconds, it turns *yellow*;
  * When the time runs out, it turns *red*.
  * The buzzer will *beep* in different frequency or period following the light turning.

And *an "extra" stage* (Audience Q&A) is added, which don't need timers. Lights and the 8-digit display will be cleared when entering "extra" stages.

***Known issues:***

Through the test, I found the supply current via USB port increased to 240~260mA, double to Beta 3... And **when using USB power supply, the buzzer did not "stop"** -- playing a noise louder than "stopped" properly. I measured the buzzer's supply current when it's "stopped" -- 130mA! Probably there's problem with my buzzer...  
To "stop" the buzzer properly and ensure the stability of the system, **a 9/12V DC adaptor** is recommended to power the system.

*(Too busy to optimize...)*

### Beta 5 has been released (27 Apr, 2016)
This is a minor update. ONLY requires an extra button.
> Space occupation: 10,424 bytes  
Memory allocation: 685 bytes

  * Enabled the LEFT button, to switch *backwards*.
  * Adjusted the position of starting "beep"; the buzzer will beep after the welcome titles are shown. *But the issue in Beta 4 remains unsolved...*
  * Moved the contents of welcome titles `START_TITLE` , only shown once in overall execution, from RAM to Flash; and read the contents from Flash. This reduced the usage of RAM by 72 bytes (compared to Beta 4). A good experiment...

### Beta 6 won't be unveiled until my blog system is uploaded (maybe 1 month later) :P
