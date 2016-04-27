#include <LiquidCrystal_I2C.h>
#include <LedControl.h>
#include <avr/pgmspace.h>
/*Beta_5 @Demkors Infinity
  An Arduino-based timer for debate competition
  TEST switching both forwards and BACKWARDS,
    and storing char arrays into Program Memory
  USING Ocrobot (Arduino compatible) Mega 2560 board,
    and other modules
*/

// ********* DEFINITION OF SOFTWARE LAYER *********
//Time required for each side in every stage.(Measured in S)
//If "Settings" is added into the system, these must switch to variables.
#define FIRST_STATEMENT 150
#define SECOND_STATEMENT 90
#define FREE_DEBATE 240
#define FINAL_CONCLUSION 180
//Numbers representing each side
#define POS 1
#define NEG 0
//Declaration of SPECIAL SYMBOLS
#define ICON_PAUSE 0
#define ICON_PLAY 1
#define ICON_LEFT 2
#define ICON_RIGHT 3
#define ICON_EXC 4
//Declaration of SYSTEM MODE
#define READY 0
#define SPEAKING 1
#define PAUSED 2
#define FINISHED 3
#define IN_SETTING 4
//The period "light_test" function should maintain (*8)
#define DELAY 400
//Numbers of essential stages and all availible stages
#define EXPECTED_STAGE 11
#define MAX_STAGE 12

// ********* HARDWARE DECLARATION *********
//Declaration of INDICATOR LIGHTS
//Numbers of pins
#define LEDS_EACH_SIDE 2
//Corresponding light of each pin
#define LED_GREEN 0
#define LED_RED 1
#define LED_YELLOW 2
//Colors
#define OFF 0
#define GREEN 1
#define YELLOW 2
#define RED 3
//Output pins of MAX7219-based 8-digit display
#define DIGIT_DIN 5
#define DIGIT_CS 6
#define DIGIT_CLK 7
//A source-less buzzer output, using Pin 11
#define BUZZER_PIN 11
//Hardware limits
#define MAX_PCS 1
#define MAX_SEGMENT 8
#define MAX_BUTTON 3
//Self-defined limits
#define MAX_SEC 999
#define MAX_DIGIT 3
#define CUSTOM_CHAR 5
#define SHORT_DURATION 150
#define WARN_DURATION 300
#define START_FREQ 896
#define FINISH_FREQ 896
#define WARNING_FREQ 440
//LCD 2004 screen to show the current state.
LiquidCrystal_I2C lcd(0x27, 20, 4);
/*Control the 8-digit 7-segment display SEPARATELY
  [WARNING] The matrix module has been removed since BETA3        */
LedControl digit = LedControl(DIGIT_DIN, DIGIT_CLK, DIGIT_CS, MAX_PCS);
/*Control panel, consisting of 3 buttons
  & a public pin for detecting if any button is pushed.
  Use pins A0~A3(54~57 on Arduino Mega) to connect buttons.*/
const byte BUTTON_PIN[MAX_BUTTON] = {
  54, 55, 56
};
const byte ANY_BUTTON_PRESSED = 57;
/*Indicator LED module, consisting of 2 RGB LEDs, represents the time remaining
  of each side.
  Use pins 3,4 for the right side, and 8,9 for the left side.
  [CAUTION] Pin 2 & 10 are reserved for configuration of 6 individual LEDs
*/
const byte LIGHT_PIN[2][LEDS_EACH_SIDE] = {
  //G, R,(Y)
  {3, 4},
  {9, 8}
};

// ********* CONTENTS TO SHOWN ON LCD SCREEN *********

//Title of every stage (Line 1,col+1)
const char *TITLE[] = {
  "   1st Statement  ",
  "2nd Attack Defense",
  "3rd Attack Defense",
  "  1st Conclusion  ",
  "    Free Debate   ",
  " Final Conclusion ",
  "   Audience Q&A   "
};
//Current side speaking/to speak (Line 2,col+7)
const char CURRENT_SIDE[2][9] = {
  "NEGATIVE",
  "POSITIVE"
};
//Start-up title in All lines(col+2)
const char START_TITLE[] PROGMEM = 
  "Timer for Debate --- BETA 5 --- Demkors Infinity  27 Apr,2016   "
/* row=
 * 0               1               2               3
 */
;
//Current mode of the system (Line 3,col+7)
const char SYSTEM_MODE[4][11] = {
  "  Ready   ",
  "Speaking..",
  "  Paused  ",
  "*Finished*"
};
//Contents in Line 2 & 3(col+7) for additional Stage 12
const char ADDIT_STAGE[2][11] = {
  "Wait for  ",
  "the result"
};
//SPECIAL SYMBOLS ON LCD 2004 (Line 4), used for buttons' functions
// [WARNING] Function 0 means "NOT USED"
byte SYMBOL[CUSTOM_CHAR][8] = {
  { //1.PLAY
    B10000,
    B11000,
    B11100,
    B11110,
    B11100,
    B11000,
    B10000,
    B00000
  }, { //2.PAUSE
    B11011,
    B11011,
    B11011,
    B11011,
    B11011,
    B11011,
    B11011,
    B11011
  }, { //3.LEFT
    B00010,
    B00100,
    B01000,
    B11111,
    B11111,
    B01000,
    B00100,
    B00010
  }, { //4.RIGHT
    B01000,
    B00100,
    B00010,
    B11111,
    B11111,
    B00010,
    B00100,
    B01000
  }, { //5.EXCHANGE -- Yet to be finalized
    B10111,
    B11001,
    B11101,
    B10001,
    B10001,
    B10111,
    B10011,
    B11101
  }
};

// ********* DECLARATION OF PUBLIC VARIABLES *********

//Current side speaking (POSITIVE in default)
byte SIDE = POS;
//Current stage
byte STAGE = 0;
//Current mode of the system
byte MODE = READY;
//Numbers of timers in use
byte ACTIVATED_TIMER = 0;
/*MAIN & SECONDARY TIMERS (in seconds & milliseconds)
  SECONDARY timers reset to 0 & count MAIN timers when exceeding 1000*/
unsigned short count[2] = {0, 0};     //MAIN timers
unsigned short countMS[2] = {0, 0};   //SECONDARY timers
//Read current time from the built-in "millis" timer
unsigned long startTime;
unsigned long currentTime;
/*Current function of buttons
  [CAUTION] Button 0 remains undefined in BETA3 & 4
  [WARNING] To make sure that each funtion of buttons has a customized icon,
            the number of functions may NOT exceed 8 */
byte button[MAX_BUTTON] = {0, 0, 0};
//A switch that determines if it's necessary to update the indicator lights
boolean UPDATE_LIGHTS = false;
/*The states of indicator lights of both sides
  0: (turned off)  1: green  2: yellow  3: red*/
byte light[2] = {0, 0};
//Timer for the buzzer and its mode
short buzzer_count = 0;
boolean BUZZER_PLAYING = false;

// ********* CODE SEGMENT *********

//HARDWARE OUTPUT MODULES CONTROL

//The "PRINT" function in LiquidCrystal library is ineffective. Here rewrites it
void print(const char src[]) {
  for (byte i = 0; src[i]; i++) lcd.write(src[i]);
}

/*Format required contents & print them on the LCD 2004 screen
  1: TITLE OF EVERY STAGE (Line 1,col+1)
  2: CURRENT SIDE TO SPEAK/SPEAKING (Line 2,col+7)
  3: CURRENT MODE OF THE SYSTEM (Line 3,col+7)
  4: CURRENT FUNCTION OF BUTTONS (Line 4,col=20/(MAX_BUTTON+1))
  92: THE NUMBER OF CURRENT STAGE (e.g. [01])(Line 2,col+0)
  93: THE AVAILIBLE TIME OF EACH SIDE IN THIS STAGE (e.g. 180s)(Line 3,col+0)
*/
void lcd_control(byte func) {
  static byte diff = 20 / (MAX_BUTTON + 1);
  switch (func) {
    case 1:
      lcd.setCursor(1, 0);
      print(TITLE[STAGE <= 9 ? (STAGE - 1) / 2 : STAGE / 2]);
      break;
    case 2:
      lcd.setCursor(7, 1);
      print(CURRENT_SIDE[SIDE]);
      break;
    case 3:
      lcd.setCursor(7, 2);
      print(SYSTEM_MODE[MODE]);
      break;
    case 4:
      for (byte i = 0; i < MAX_BUTTON; i++) {
        lcd.setCursor(i * diff + 4, 3);
        // [WARNING] Function 0 means "NOT USED"
        if (button[i]) lcd.write(button[i] - 1);
        else lcd.write(' ');
      }
      break;
    case 92:
      lcd.setCursor(0, 1);
      lcd.write('[');
      lcd.write(STAGE > 9 ? '1' : '0');
      lcd.write(STAGE % 10 + 48);
      lcd.write(']');
      break;
    case 93:
      lcd.setCursor(0, 2);
      lcd.write(count[SIDE] / 100 + 48);
      lcd.write((count[SIDE] % 100) / 10 + 48);
      lcd.write('0');
      lcd.write('s');
      break;
    default:
      break;
  }
}

//Show the Welcome title on LCD 2004 & load customized symbols to it
void showWelcome() {
  byte i;
  for (i = 0; i < 4; i++) {
    lcd.setCursor(2, i);
    for (int j = 0; j < 16; j++) {
      byte buffer = pgm_read_byte(&START_TITLE[i*16+j]);
      lcd.write(buffer);
    }
  }
  for (i = 0; i < CUSTOM_CHAR; i++) lcd.createChar(i, SYMBOL[i]);
}

//Format both the timers to show on the 8-digit display properly
void connectNum(char *target, byte pos, char src[]) {
  switch (strlen(src)) {
    case 0: strcat(target, "   "); break; //fill 3 blanks
    case 1: strcat(target, "  "); break; //fill 2 blanks
    case 2: strcat(target, " "); break; //fill 1 blank
    default: break;  //do nothing
  }
  strcat(target, src);  //Then put the required number into the string to be shown
}

//Show seconds remaining on the 8-digit display
void digit_control() {
  //POSITIVE timer shown in 6~8th digits(on the LEFT), NEGATIVE shown in 1~3
  char str[9] = "", trans[7];
  byte i = 0, j;
  if (ACTIVATED_TIMER == 0) {
    digit.clearDisplay(0);
    return;
  }
  itoa(count[POS], trans, 10);
  if (ACTIVATED_TIMER == 2 || SIDE == POS) connectNum(str, i, trans);
  else strcat(str, "   ");  //If POSITIVE timers is disactivated, fill 3 blanks
  for (i = MAX_DIGIT; i < MAX_SEGMENT - MAX_DIGIT; i++) str[i] = ' ';
  str[i] = '\0';
  itoa(count[NEG], trans, 10);
  if (ACTIVATED_TIMER == 2 || SIDE == NEG) connectNum(str, i, trans);
  else strcat(str, "   ");  //And so for NEGATIVE
  for (i = strlen(str) - 1, j = 0; j < MAX_SEGMENT && i >= 0; j++, i--)
    digit.setChar(0, j, str[i], false);
}

//Make the buzzer to play a specified tone for some time
void buzzer_play(unsigned short frequency, short duration) {
  buzzer_count = duration;
  BUZZER_PLAYING = true;
  tone(BUZZER_PIN, frequency, duration);
}
void close_buzzer() {
  BUZZER_PLAYING = false;
  noTone(BUZZER_PIN);
}

// [WARNING] The RGB LEDs are Common-Anode type
//Turn off all the indicator LEDs
void init_light() {
  for (byte i = 0; i < 2; i++) {
    for (byte j = 0; j < LEDS_EACH_SIDE; j++)
      digitalWrite(LIGHT_PIN[i][j], HIGH);
    light[i] = OFF;
  }
}
//Control hardware indicator LEDs & the buzzer IF NECESSARY
void light_buzzer_control() {
  if (count[SIDE] > 30) {
    digitalWrite(LIGHT_PIN[SIDE][LED_GREEN], LOW);
    light[SIDE] = GREEN;
    if (ACTIVATED_TIMER == 2) {
      digitalWrite(LIGHT_PIN[!SIDE][LED_GREEN], LOW);
      light[!SIDE] = GREEN;
    }
    buzzer_play(START_FREQ, SHORT_DURATION);
  }
  else if (count[SIDE] == 0) {
    digitalWrite(LIGHT_PIN[SIDE][LED_RED], LOW);
    digitalWrite(LIGHT_PIN[SIDE][LED_GREEN], HIGH);
    //digitalWrite(LIGHT_PIN[SIDE][LED_YELLOW],HIGH);
    light[SIDE] = RED;
    buzzer_play(FINISH_FREQ, WARN_DURATION);
  }
  else {
    digitalWrite(LIGHT_PIN[SIDE][LED_RED], LOW);
    //digitalWrite(LIGHT_PIN[SIDE][LED_YELLOW],HIGH);
    light[SIDE] = YELLOW;
    buzzer_play(WARNING_FREQ, WARN_DURATION);
  }
  UPDATE_LIGHTS = false;
}
//Check indicator LEDs on the software layer. If necessary, process the hardware part
void light_check() {
  if (count[SIDE] > 30) {
    if (light[SIDE] != GREEN) UPDATE_LIGHTS = true;
  }
  else {
    if (count[SIDE] == 0 && light[SIDE] != RED) UPDATE_LIGHTS = true;
    else if (light[SIDE] != YELLOW) UPDATE_LIGHTS = true;
    else return;
  }
  if (UPDATE_LIGHTS) light_buzzer_control();
}

//SYSTEM MODE CONTROL
//Set a new mode of the system and print on LCD screen
void mode_control(byte new_status) {
  MODE = new_status;
  lcd_control(3);
}

//TIMERS CONTROL

//Initialize both timers (when switching stage)
void init_timer() {
  countMS[POS] = countMS[NEG] = 0;
  if (STAGE == 9) {
    ACTIVATED_TIMER = 2;
    SIDE = NEG;
  }
  else if (STAGE <= 0 || STAGE > EXPECTED_STAGE) {
    ACTIVATED_TIMER = 0;
    count[POS] = count[NEG] = 0;
  }
  else if (STAGE % 2 == 1) {
    ACTIVATED_TIMER = 1;
    SIDE = POS;
  }
  else {
    ACTIVATED_TIMER = 1;
    SIDE = NEG;
  }
  switch (STAGE) {
    case 3: case 4: case 5: case 6: case 7: case 8:
      count[SIDE] = SECOND_STATEMENT;
      count[!SIDE] = 0;
      break;
    case 1: case 2:
      count[SIDE] = FIRST_STATEMENT;
      count[!SIDE] = 0;
      break;
    case 10: case 11:
      count[SIDE] = FINAL_CONCLUSION;
      count[!SIDE] = 0;
      break;
    case 9:
      count[POS] = count[NEG] = FREE_DEBATE;
      break;
    default: break;
  }
  if (STAGE <= EXPECTED_STAGE) lcd_control(2);
  lcd_control(92);
  lcd_control(93);
  digit_control();
  init_light();
}

//Check the current time & update the present timer
void timer_control() {
  currentTime = millis();
  if (MODE == SPEAKING) {
    unsigned long interval = currentTime - startTime;
    while (interval >= 1000 && count[SIDE] > 0) {
      interval -= 1000;
      count[SIDE]--;
    }
    countMS[SIDE] += (short)interval;
    if (countMS[SIDE] >= 1000) {
      countMS[SIDE] -= 1000;
      if (count[SIDE] > 0) count[SIDE]--;
    }
    digit_control();
    light_check();
    if (count[SIDE] == 0) {
      //The time of current side has run out
      countMS[SIDE] = 0;
      if (ACTIVATED_TIMER == 2) Exchange();
      mode_control(FINISHED);
      if (count[SIDE] == 0) button_control(3, 1, 4);
      else button_control(0, 0, 1);
    }
  }
  if (BUZZER_PLAYING) {
    buzzer_count -= (currentTime - startTime);
    if (buzzer_count <= 0) close_buzzer();
  }
  startTime = currentTime;
}

//INPUT PANEL & BUTTON FUNCTIONS

//Modify the functions of buttons, and show the functions on LCD screen
// [CAUTION] The system supports UP TO 3 buttons by default
// [WARNING] Function 0 means "NOT USED"
void button_control(byte btn0, byte btn1, byte btn2) {
  button[0] = btn0;
  button[1] = btn1;
  button[2] = btn2;
  lcd_control(4);
}

//Function 1: PLAY
void Play() {
  if (MODE != SPEAKING) {
    mode_control(SPEAKING);
    if (ACTIVATED_TIMER == 2 && count[!SIDE] != 0) button_control(0, 2, 5);
    else {
      button_control(0, 2, 0);
      lcd_control(2);
    }
    startTime = millis();
  }
  else return;
}
//Function 2: PAUSE
void Pause() {
  if (MODE == SPEAKING) {
    mode_control(PAUSED);
    button_control(3, 1, 4);
  }
  else return;
}
//Function 3/4: SCROLL forwards/backwards
// ONLY supports FORWARD (->) in BETA 3 & 4
// Added support for BACKWARD (<-) in BETA 5
void Scroll(byte function) {
  if (function == 4) {
    STAGE++;
    if (STAGE > MAX_STAGE) STAGE = 1;
  }
  else if (function == 3) {
    STAGE--;
    if (STAGE < 1) STAGE = MAX_STAGE;
  }
  else return;
  lcd_control(1);
  if (STAGE > EXPECTED_STAGE) {
    //Read 2 lines of contents from ADDIT_STAGE and print in Line 2 & 3(col+7) on the screen
    //The contents depend on current STAGE (If it's No.12, Line 0 and 1 will be printed)
    //And clear the 8-digit display and reset LEDs
    button_control(3, 0, 4);
    for (byte i = (STAGE - EXPECTED_STAGE - 1) * 2; i < (STAGE - EXPECTED_STAGE) * 2; i++) {
      lcd.setCursor(7, i % 2 + 1);
      print(ADDIT_STAGE[i]);
    }
  }
  else {
    mode_control(READY);
    button_control(3, 1, 4);
  }
  init_timer();
}
//Function 5: EXCHANGE (Switch the current side speaking)
void Exchange() {
  if (count[SIDE] == 0) SIDE = !SIDE;
  else if (count[SIDE] > 1) {
    SIDE = !SIDE;
    lcd_control(2);
  }
  else {
    //When the time of last speaking side remains below 1 second, run out the time automatically,
    //then continue counting time of another side
    while (countMS[SIDE] < 1000) {
      currentTime = millis();
      countMS[SIDE] += (currentTime - startTime);
      startTime = currentTime;
    }
    count[SIDE] = countMS[SIDE] = 0;
    buzzer_play(FINISH_FREQ, WARN_DURATION);
    SIDE = !SIDE;
  }
}
/*Guide to corresponding function of the button pressed
  FUNCTION 0: (Empty)
          1: Play/Start speaking
          2: Pause
          3: Scroll backwards (to be added in Beta 5)
          4: Scroll forwards
          5: Switch current side speaking
*/
void button_select(byte ButtonDown) {
  switch (button[ButtonDown]) {
    case 0: break;
    case 1:
      Play(); break;
    case 2:
      Pause(); break;
    case 3:
      Scroll(3); break;
    case 4:
      Scroll(4); break;
    case 5:
      Exchange(); break;
    default: break;
  }
  startTime = millis();
}

//MAIN PROGRAM
//Test lights when initializing the system
void light_test() {
  for (byte i = 0; i < 8; i++) {
    digit.setDigit(0, i, 8, true);
    switch (i / 2) {
      case 1:
        digitalWrite(LIGHT_PIN[POS][LED_RED], LOW);
        digitalWrite(LIGHT_PIN[NEG][LED_RED], LOW);
        break;
      case 2:
        digitalWrite(LIGHT_PIN[POS][LED_GREEN], LOW);
        digitalWrite(LIGHT_PIN[NEG][LED_GREEN], LOW);
        break;
      case 3:
        digitalWrite(LIGHT_PIN[POS][LED_RED], HIGH);
        digitalWrite(LIGHT_PIN[NEG][LED_RED], HIGH);
        break;
      default: break;
    }
    delay(DELAY);
    digit.setChar(0, i, ' ', false);
  }
}
//Initialize the system
void setup() {
  digit.shutdown(0, false);
  digit.setIntensity(0, 4);
  digit.clearDisplay(0);
  for (byte i = 0; i < MAX_BUTTON; i++) {
    pinMode(BUTTON_PIN[i], INPUT);
    digitalWrite(BUTTON_PIN[i], HIGH);
  }
  for (byte i = 0; i < 2; i++)
    for (byte j = 0; j < LEDS_EACH_SIDE; j++)
      pinMode(LIGHT_PIN[i][j], OUTPUT);
  pinMode(ANY_BUTTON_PRESSED, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(ANY_BUTTON_PRESSED, HIGH);
  digit_control();
  init_light();
  Wire.begin();
  lcd.init();
  lcd.backlight();
  showWelcome();
  buzzer_play(START_FREQ, SHORT_DURATION);
  delay(SHORT_DURATION);
  close_buzzer();
  light_test();
  STAGE = 1;
  lcd.clear();
  lcd_control(1);
  init_timer();
  mode_control(READY);
  button_control(3, 1, 4);
  startTime = millis();
}

//The MAIN loop will check buttons repeatedly
void loop() {
  static byte ButtonPressed = -1;
  //Record the button pressed, value -1 means none or over 2 of buttons are being pressed
  if (digitalRead(ANY_BUTTON_PRESSED) == LOW) {
    for (byte i = 0; i < MAX_BUTTON; i++)
      if (digitalRead(BUTTON_PIN[i]) == LOW) {
        //Recognize the first button being pressed
        ButtonPressed = i;
        break;
      }
    while (digitalRead(ANY_BUTTON_PRESSED) == LOW) {
      /*Once a second button is pressed, the system will recognize, and keep updating timers
        without anything else done, until all the buttons are released. */
      if (ButtonPressed == -1) {
        timer_control();
        continue;
      }
      for (byte i = 0; i < MAX_BUTTON && i != ButtonPressed; i++)
        if (digitalRead(BUTTON_PIN[i]) == LOW) {
          ButtonPressed = -1;
          break;
        }
    }
    //If none of other buttons is pressed, process corresponding function of the first button
    if (ButtonPressed != -1) button_select(ButtonPressed);
  }
  //Every time after checking buttons (and processing), update timers
  timer_control();
}
