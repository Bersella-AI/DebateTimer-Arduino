# Debate Timer @Arduino
> **FIRST REPOSITORY OF iDEMKORS ON GitHub**  
A timer for debate competition based on Arduino  
Arduino上的辩论赛计时器 - iDemkors第一个GitHub仓库

## CONCLUSION
+ This is an experimental Arduino project codenamed "Beta". An Ocrobot MEGA 2560 board (an alternative to Arduino Mega 2560) with variants of hardware modules is programmed to make the timer.
+ The latest version of .ino source program is Beta 6.
+ The Chinese version of this document will be unveiled after uploading a blog system to my GitHub.
+ 本说明书的完整中文版本将会在博客系统上传之后另行发布。
+ More details will be announced gradually. If any mistake in this document or in the code is committed, please contact me.
+ A new document *"Change log"* has been created for more details about all versions of the project.

## SYSTEM SUMMARY

### SYSTEM REQUIREMENT (by default)
* ESSENTIAL HARDWARE (for Beta 3)
  + An Arduino (or compatible) board;  
(The project uses Mega 2560, it's fine if using a UNO)
  + A 20x4 LCD screen **with I2C support**;
  + A MAX7219-based 8-digit LED display module;
  + 2 or 3 groups of a button, an 1N4148 diode, and an 100nF capacitor;
  + A breadboard and wires.

* ADDITIONAL HARDWARE (Beta 4 or later)
  + 2 RGB LEDs (along with resistors);
  + A **source-less** buzzer (along with an 100μF capacitor);
  + (Former) 16 bytes of internal EEPROM.

* (HARDWARE SCHEDULED IN PROGRESS)
  + A Nokia 5110 screen (to be used in Beta 7);
  + A DS3231-based real-time clock module. (Or may never be added)

* LIBRARIES
  + [LedControl](http://wayoda.github.io/LedControl/) (by Eberhard Fahle)
  + [LiquidCrystal_I2C](https://github.com/marcoschwartz/LiquidCrystal_I2C) (by Marco Schwartz)
  + [EEPROM](https://www.arduino.cc/en/Reference/EEPROM) (Built-in library) (Beta 6)

### Default Settings 
POS = Positive  
NEG = Negative  
When stages following requires both sides to speak, the system will indicate which side should start the speech. 

> 01. 1st Statement - POS 150s
> 02. 1st Statement - NEG 150s
> 03. 2nd Attack Defense - POS 90s
> 04. 2nd Attack Defense - NEG 90s
> 05. 3rd Attack Defense - POS 90s
> 06. 3rd Attack Defense - NEG 90s
> 07. 1st Conclusion - POS 90s
> 08. 1st Conclusion - NEG 90s
> 09. Free Debate - NEG  240s
> 10. Final Conclusion - NEG 180s
> 11. Final Conclusion - POS 180s


### How to use the system (Beta 6 for example)
1. Connect the necessary hardware to your Arduino (compatible) board.
  + The right RGB LED (for Negative) takes Pin 3 (Green) and 4 (Red).
  + Pin 5~7 is arranged for MAX7219-based display; Pin 5, 6, and 7 correspond to DIN, LOAD(CS), and CLK on MAX7219.
  + The left RGB LED (for Positive) takes Pin 8 (Red) and 9 (Green).
  + The buzzer is connected to Pin 11. be cautious using it if not having an 100μF capacitor connected between it and GND.
  + 3 buttons make use of Pin A0~A2 (the left one uses A0).
  + If having diodes, connect A3 to all 3 buttons through separate diodes, from the pin to buttons. (IM DYING)
  + Prepare a 20x4 LCD screen **with I2C support**, then connect Pin "SDA" "SCL" on both Arduino and the screen.
2. Power on the system, and wait until the screen shows "1st Statement".
3. **Button functions:**
  + **"Left" and "Right" narrow** : Switch forwards/backwards; change value  
  + **"Play"** : Start/Continue the speech  
  + **"Pause"** : Pause the speech  
  + **"Screwdriver"** : Enter Settings/Confirm  
  + **"Two narrows"** : Switch the currently speaking side

4. Be the best time keeper!
 
## FOR THOSE WHO WANT TO IMPORT
### If you are using a UNO...
The available I/O pins on a UNO board may not be enough for the system. Here are the solutions:

  1. Use 2 RGB LEDs instead of 6 separate LEDs. 4 pins should be enough to generate red, green, and yellow lights for both sides.
  2. Use a 20x4/16x2  LCD screen via I2C serial bus, that requires only 2 I/O pins. 
  3. Versions requiring 2 buttons (Beta 3 & 4) can be considered. (**Note:** New features in subsequent versions are NOT included)
  4. Modify the code in `loop()` to avoid using the pin `ANY_BUTTON_PRESSED` that check whether one of the buttons is being pressed.
  5. Switch to a Mega board -- the easiest way!

### If you are using a 16x2 LCD...
I have no plans for special versions for the screen. QwQ

### If you DON'T have a diode (not LED) or a capacitor...
Don't worry, the use of diodes aims for a faster recognition of pressing buttons. You can do Step 4 in "using a UNO" above to avoid using diodes.  
And a capacitor can improve the stability when checking whether a button is being pressed. Removing it may result in LITTLE difference, buttons can still work properly (the contents on the screen may "flash" for a moment).  
Taking advantage of them is RECOMMENDED, though.

## HISTORY
### Beta 6 (18 May, 2016)
+ Supports configuration of time limit and (firstly) speaking side of each stage
+ Stores configuration after power off
+ Supports "20 seconds" for the answering side in stages "Attack Defense"

See [Change Log](https://github.com/iDemkors/DebateTimer-Arduino/blob/master/Changelog.md) for more details.

##LICENSE
[MIT](https://github.com/iDemkors/DebateTimer-Arduino/blob/master/LICENSE.md)

![Its me](http://ww1.sinaimg.cn/large/62fb934ajw1f42vsx2wm6j20j60f8wha.jpg)






















