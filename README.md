# Debate Timer @Arduino
> **FIRST REPOSITORY OF iDEMKORS ON GitHub**  
A timer for debate competition based on Arduino  
基于Arduino的辩论赛计时器 - iDemkors第一个GitHub仓库

## CONCLUSION
+ This is an experimental Arduino project codenamed "Beta". An Ocrobot MEGA 2560 board (an alternative to Arduino Mega 2560) with variants of hardware modules is programmed to make the timer.
+ The system is adaptive for debate contests based on international rules, with 11 sections during a contest; can configure the time limit and (firstly) speaking side of each section; and save the configuration after power off. More details are available on the following.
+ The latest version of .ino source program is Beta 6.
+ The Chinese version of this document will be unveiled after uploading a blog system to my GitHub.
+ 本说明书的[完整中文版本](http://idemkors.github.io/2016/06/08/DebateTimer/)已在自建博客中提供。
+ If any mistake in this document or in the code is committed, please contact me.
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
  + A PCD8544 (Nokia 5110) screen (to be used in Beta 7);
  + A DS3231-based real-time clock module. (Or may never be added)

* LIBRARIES
  + [LedControl](http://wayoda.github.io/LedControl/) (by Eberhard Fahle)
  + [LiquidCrystal_I2C](https://github.com/marcoschwartz/LiquidCrystal_I2C) (by Marco Schwartz)
  + [EEPROM](https://www.arduino.cc/en/Reference/EEPROM) (Built-in library, used in Beta 6)

### Default Settings 
The system supports debate contests with 11 sections, the number and names of which can be only modified through source program; while the time limit and (firstly) speaking side of each side can be configured through "Settings" and saved, and kept after power off. The option "Reset settings" is also available.  
**On the following shows default settings of each section:**

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
> 12. Audience Q&A - (Additional section, entrance to "Settings")  
> POS = Positive ;  NEG = Negative

The sections: "Attack Defense" and "Free debate" requires both sides to speak, in which the side that should start the speech will be shown in the screen (when switching to these sections).   
In the "Attack Defense" sections, the side being asked will be given 20 seconds if it's turn to speak, which can only be modified in the source program. When answering, both timers of each side work simultaneously; and when not answering, the timer of that side will be disabled. Furthermore, the time limit of "Attack Defense" is how long the whole section will last.  
In the "Free debate" sections, both sides are given the time set in the configuration; only the timer for the currently speaking side will work.  
The Positive "sit" on the left **initially**. If not, find the following 2 lines in the program:

```C
#define POS 1
#define NEG 0
```

Then exchange the two numbers like this:

```C
#define POS 0
#define NEG 1
```

### How to use the system (Beta 6 for example)
1. Connect the necessary hardware to your Arduino (compatible) board.
  + The right RGB LED (for Negative) takes Pin 3 (Green) and 4 (Red).
  + Pin 5~7 is arranged for MAX7219-based display; Pin 5, 6, and 7 correspond to DIN, LOAD(CS), and CLK on MAX7219.
  + The left RGB LED (for Positive) takes Pin 8 (Red) and 9 (Green).
  + The buzzer is connected to Pin 11. be cautious using it if not having an 100μF capacitor connected between it and GND.
  + 3 buttons make use of Pin A0~A2 (the left one uses A0).
  + If having diodes, connect A3 to all 3 buttons through separate diodes, from the pin to buttons. (IM DYING)
  + Prepare a 20x4 LCD screen **with I2C support**, then connect Pin "SDA" "SCL" on both Arduino and the screen.
2. Power on the system, upload the source program in Arduino IDE, and wait until the screen shows "1st Statement".
3. **Button functions (icons are shown on the bottom of LCD screen):**
  + **"Left" and "Right" narrow** : Switch forwards/backwards; change value  
  + **"Play"** : Start/Continue the speech  
  + **"Pause"** : Pause the speech  
  + **"Spanner"** : Enter Settings/Confirm  
  + **"Two narrows"** : Switch the currently speaking side

4. Be the best time keeper!
 
## FOR THOSE WHO WANT TO IMPORT
### If you are using a UNO...
The available I/O pins on a UNO board may not be enough for the system. Here are the solutions:

  1. Use a 20x4  LCD screen via I2C serial bus, that requires only 2 I/O pins. 
  2. Remove the button on the left, that would remain the system able to use.
  3. Modify the code in `loop()` to avoid using the pin `ANY_BUTTON_PRESSED` that check whether one of the buttons is being pressed.
  4. Switch to a Mega board!

### If you DON'T have a 20x4 LCD screen...
I have no plans for special versions for other screens (except PCD8544). QwQ

### If you DON'T have a diode (not LED) or a capacitor...
Don't worry, the use of diodes aims for a faster recognition of pressing buttons. You can do Step 4 in "using a UNO" above to avoid using diodes.  
And a capacitor can improve the stability when checking whether a button is being pressed. Removing it may result in LITTLE difference, buttons can still work properly (the contents on the screen may "flash" for a moment).  
Taking advantage of them is RECOMMENDED, though.

## HISTORY
### Beta 6 (18 May, 2016)
+ Supports configuration of time limit and (firstly) speaking side of each section
+ Stores configuration after power off
+ Supports "20 seconds" for the answering side in sections "Attack Defense"

See [Change Log](https://github.com/iDemkors/DebateTimer-Arduino/blob/master/Changelog.md) for more details.

##LICENSE
[MIT](https://github.com/iDemkors/DebateTimer-Arduino/blob/master/LICENSE.md)

![Its me](http://ww1.sinaimg.cn/large/62fb934ajw1f42vsx2wm6j20j60f8wha.jpg)
