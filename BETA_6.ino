#include <LiquidCrystal_I2C.h>
#include <LedControl.h>
#include <avr/pgmspace.h>
#include <EEPROM.h>
/*Beta_6 @Demkors Infinity
  An Arduino-based timer for debate competition
  TEST Settings (secondary menu), saving data into/read from EEPROM,
    & multi-functional Direction keys
  USING Ocrobot (Arduino compatible) Mega 2560 board,
    and other modules

  col = Column
*/

// ********* DEFINITION OF SOFTWARE LAYER *********
//Time required for each side in every stage.(Measured in S)
//This declaration is no longer required since Beta 6

//Numbers representing each side
//The POSitive sit on the left. If not, exchange the number of both sides
#define POS 1
#define NEG 0

//Declaration of SPECIAL SYMBOLS (Never used yet)

//Declaration of SYSTEM MODE
#define READY 0
#define SPEAKING 1
#define PAUSED 2
#define FINISHED 3
#define IN_SETTING 4
//The period "light_test" function should maintain (*8)
#define DELAY 400
//Detecting whether the STAGE is "Attack Defense" (3~6)
#define isAttackDefense (STAGE<7&&STAGE>2)
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
#define CUSTOM_CHAR 6
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
  A0, A1, A2
};
const byte ANY_BUTTON_PRESSED = A3;
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

// ********* DEFAULT SETTINGS *********
const byte DEFAULT_SETTING[] PROGMEM = {
  0x14, 0x77, 12, 11, 0x15, 0x05, 0x33, 0x23, 0x33, 0x23, 0x13, 0x03, 0x28, 0x06, 0x16, 0
  //Checksum: 0x1477 = 5239
};

// ********* CONTENTS TO SHOWN ON LCD SCREEN *********

//Title of every stage (Line 1,col+1)  (Line 2,col+1 in Settings)
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
//(Line 3,col+6 in Settings)
const char CURRENT_SIDE[2][9] = {
  "NEGATIVE",
  "POSITIVE"
};
//Start-up title in All lines(col+2)
const char START_TITLE[] PROGMEM =
  "Timer for Debate --- BETA 6 --- Demkors Infinity  16 May,2016   "
/* row=
   0               1               2               3
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
  "\"Screw\":  ",
  "Setup time"
};
//Titles shown in Settings (Line 1,col+0)
const char HINT_SETTING[] PROGMEM =
  "Choose & edit stage"
  ;
const char HINT_EDIT_SIDE[] PROGMEM =
  " Edit Side of Stage"
  ;
const char HINT_EDIT_TIME[] PROGMEM =
  " Edit Time of Stage"
  ;
//Function of extra stages in Settings (Line 2,col+1)
const char HINT_SAVE[] PROGMEM =
  "  Save and Exit!  "
  ;
const char HINT_RESET[] PROGMEM =
  "Reset all Setting "
  ;
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
  }, { //5.EXCHANGE
    B10111,
    B11001,
    B11101,
    B10001,
    B10001,
    B10111,
    B10011,
    B11101
  }, { //6.SETTINGS
    B01001,
    B01111,
    B00110,
    B00110,
    B01100,
    B01100,
    B11110,
    B10010
  }
};

// ********* DECLARATION OF PUBLIC VARIABLES *********
//Record of time and (firstly) speaking side of each stage
typedef struct {
  byte *sides;
  byte *second30;
} Stages;
Stages stg;
//Numbers of essential stages and all availible stages
byte EXPECTED_STAGE;
byte MAX_STAGE;
byte MAX_SETTING;
//Current side speaking (POSITIVE in default)
byte SIDE = POS;
//Works for stages requiring 2 timers working in the meanwhile
byte SIDE_LOCK;
//Current stage
byte STAGE = 0;
//Current mode of the system
byte MODE = READY;
//Numbers of timers in use
byte ACTIVATED_TIMER = 0;
//Depth (or Mode) in Setting
byte SETTING_DEPTH = 0;
//Stage to be configured
byte SETTING_STAGE = 0;
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
//The moment of time remaining when the light should turn to "Warning"
short WARNING_MOMENT = 30;
/*The states of indicator lights of both sides
  0: (turned off)  1: green  2: yellow  3: red*/
byte light[2] = {0, 0};
//Timer for the buzzer and its mode
short buzzer_count = 0;
boolean BUZZER_PLAYING = false;

// ********* CODE SEGMENT *********

//GENERAL METHODS
//Adjust the value of "src" (+1/-1) between Min and Max
void Switch(byte &src, byte Min, byte Max, byte fromKey) {
  if (fromKey == 4) {
    if (src == Max) src = Min;
    else src++;
  }
  else if (fromKey == 3) {
    if (src == Min) src = Max;
    else src--;
  }
  else return;
}
//Update flags timers according to specified stage "fromStg"
void DealwithStage(byte fromStg) {
  byte i = fromStg - 1;
  if (fromStg < 0 || fromStg > EXPECTED_STAGE) {
    ACTIVATED_TIMER = 0;
    if (fromStg < 0 || fromStg > MAX_STAGE) return;
  }
  else ACTIVATED_TIMER = (stg.sides[i] > 1) ? 2 : 1;
  SIDE = stg.sides[i] % 2;
  count[SIDE] = stg.second30[i] * 30;
  count[!SIDE] = (isAttackDefense) ? 20 : count[SIDE];
  WARNING_MOMENT = (count[SIDE] < 120) ? 15 : 30;
}

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
  93: THE TIME LIMIT OF EACH SIDE IN THIS STAGE (e.g. 180s)(Line 3,col+0)
  102: SELECTED STAGE in Setting (Line 2,col+1)
  113: THE NUMBER OF SELECTED STAGE in Setting (Line 3,col+1)
  123: SIDE TO SPEAK IN THIS STAGE in Setting (Line 3,col+6)
  133: TIME LIMIT OF THIS STAGE in Setting (Line 3,col+15)
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
    case 102:
      lcd.setCursor(0, 1);
      lcd.write('[');
      print(TITLE[SETTING_STAGE <= 9 ? (SETTING_STAGE - 1) / 2 : SETTING_STAGE / 2]);
      lcd.write(']');
      break;
    case 113:
      lcd.setCursor(1, 2);
      lcd.write('<');
      lcd.write(SETTING_STAGE / 10 + 0x30);
      lcd.write(SETTING_STAGE % 10 + 0x30);
      lcd.write('>');
      break;
    case 123:
      lcd.setCursor(6, 2);
      print(CURRENT_SIDE[SIDE]);
      break;
    case 133:
      lcd.setCursor(16, 2);
      lcd.write(count[SIDE] / 100 + 0x30);
      lcd.write(count[SIDE] % 100 / 10 + 0x30);
      lcd.write('0');
      lcd.write('s');
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
      byte buffer = pgm_read_byte(&START_TITLE[i * 16 + j]);
      lcd.write(buffer);
    }
  }
  for (i = 0; i < CUSTOM_CHAR; i++) lcd.createChar(i, SYMBOL[i]);
}

//Save stages' settings to EEPROM
void SaveSetting() {
  byte buffer;
  int addr = 0;
  EEPROM.update(addr++, 0x14);
  EEPROM.update(addr++, 0x77);
  EEPROM.update(addr++, MAX_STAGE);
  EEPROM.update(addr++, EXPECTED_STAGE);
  for (byte i = 0; i < MAX_STAGE; i++, addr++) {
    buffer = stg.sides[i] * 16;
    buffer += stg.second30[i];
    EEPROM.update(addr, buffer);
  }
}

//Read stages' settings from EEPROM; if the checksum failed, load default settings
void ReadSetting() {
  byte buffer;
  int addr = 0;
  light[0] = EEPROM.read(addr++);
  light[1] = EEPROM.read(addr++);
  free(stg.sides);
  free(stg.second30);
  if (word(light[0], light[1]) != 0x1477) {
    byte limit = 99;
    addr = 0;
    for (byte i = 0; i < limit; i++, addr++) {
      buffer = pgm_read_byte(&DEFAULT_SETTING[addr]);
      EEPROM.update(addr, buffer);
      if (addr == 2) limit = buffer + 4;
    }
    addr = 2;
  }
  MAX_STAGE = EEPROM.read(addr++);
  EXPECTED_STAGE = EEPROM.read(addr++);
  //SETTINGS has one more extra stage
  MAX_SETTING = MAX_STAGE + 1;
  stg.sides = (byte *)malloc(MAX_STAGE * sizeof(byte));
  stg.second30 = (byte *)malloc(MAX_STAGE * sizeof(byte));
  for (byte i = 0; i < MAX_STAGE; i++, addr++) {
    buffer = EEPROM.read(addr);
    stg.sides[i] = buffer / 16;
    stg.second30[i] = buffer % 16;
  }
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

//Show selected stage and the setting(SIDE / TIME) on the 8-digit display
void setting_digit_control() {
  char str[9] = "", trans[7];
  byte i = 0, j;
  if (ACTIVATED_TIMER == 0) {
    digit.clearDisplay(0);
    return;
  }
  itoa(SETTING_STAGE, trans, 10);
  str[i++] = (SETTING_STAGE > 9) ? '1' : '0';
  str[i++] = SETTING_STAGE % 10 + 48;
  str[i++] = ' ';
  if (SIDE) str[i++] = 'P';
  else str[i++] = 'n';
  str[i++] = ' ';
  str[i++] = (count[SIDE] >= 100) ? count[SIDE] / 100 + 48 : ' ';
  str[i++] = count[SIDE] ? count[SIDE] % 100 / 10 + 48 : ' ';
  str[i++] = '0';
  str[i] = '\0';
  for (i--, j = 0; j < MAX_SEGMENT && i >= 0; j++, i--)
    digit.setChar(0, j, str[i], (i == 1) ? true : false);
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
//Turn off the indicator LEDs of specified SIDE
void init_light(byte theSide) {
  for (byte j = 0; j < LEDS_EACH_SIDE; j++)
    digitalWrite(LIGHT_PIN[theSide][j], HIGH);
  light[theSide] = OFF;
}
//Control hardware indicator LEDs & the buzzer IF NECESSARY
void light_buzzer_control() {
  if (count[SIDE] > WARNING_MOMENT) {
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
    digitalWrite(LIGHT_PIN[SIDE][LED_GREEN], LOW);
    //digitalWrite(LIGHT_PIN[SIDE][LED_YELLOW],HIGH);
    light[SIDE] = YELLOW;
    buzzer_play(WARNING_FREQ, WARN_DURATION);
  }
  UPDATE_LIGHTS = false;
}
//Check indicator LEDs on the software layer. If necessary, process the hardware part
void light_check() {
  if (count[SIDE] > WARNING_MOMENT) {
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
  DealwithStage(STAGE);
  if (STAGE <= EXPECTED_STAGE) lcd_control(2);
  lcd_control(92);
  lcd_control(93);
  digit_control();
  init_light(0);
  init_light(1);
}

//Check the current time & update the present timer
void timer_control() {
  unsigned long interval = currentTime - startTime;
  if (BUZZER_PLAYING) {
    buzzer_count -= interval;
    if (buzzer_count <= 0) close_buzzer();
  }
  if (MODE == SPEAKING) {
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
      if (isAttackDefense) {
        if (count[SIDE_LOCK] == 0 && ACTIVATED_TIMER == 2) {
          SIDE = !SIDE_LOCK;
          count[SIDE] = 0;
          digit_control();
          light_check();
        } else if (count[SIDE_LOCK] == 0) {
          //do nothing
        } else return;
      }
      mode_control(FINISHED);
      if (ACTIVATED_TIMER==2&&!isAttackDefense) Exchange();
      if (count[SIDE] == 0) button_control(3, 1, 4);
      else button_control(0, 0, 1);
    }
  }
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
    if (isAttackDefense) {
      ACTIVATED_TIMER = 1;
      SIDE_LOCK = SIDE;
      button_control(0, 2, 5);
    }
    else if (ACTIVATED_TIMER == 2 && count[!SIDE] != 0) {
      button_control(0, 2, 5);
      SIDE_LOCK = !SIDE;
    }
    else {
      SIDE_LOCK = SIDE;
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
void Scroll(byte fromKey) {
  //function: 0=MAIN_UI  1=SETTING_CHOOSING  2=SETTING_MODIFING
  if (MODE != IN_SETTING) {
    Switch(STAGE, 1, MAX_STAGE, fromKey);
    lcd_control(1);
    if (STAGE > EXPECTED_STAGE) {
      //Read 2 lines of contents from ADDIT_STAGE and print in Line 2 & 3(col+7) on the screen
      //The contents depend on current STAGE (If it's No.12, Line 0 and 1 will be printed)
      //And clear the 8-digit display and reset LEDs
      button_control(3, 6, 4);
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
  } else if (SETTING_DEPTH == 1) {
    //Choosing stage to edit
    Switch(SETTING_STAGE, 1, MAX_SETTING, fromKey);
    if (SETTING_STAGE > EXPECTED_STAGE) {
      byte i;
      ACTIVATED_TIMER = 0;
      lcd.setCursor(1, 1);
      for (i = 0; i < 19; i++)
        if (SETTING_STAGE == EXPECTED_STAGE + 1) lcd.write(pgm_read_byte(&HINT_SAVE[i]));
        else lcd.write(pgm_read_byte(&HINT_RESET[i]));
      lcd.setCursor(1, 2);
      for (i = 0; i < 19; i++) lcd.write(' ');
      lcd.setCursor(18, 1);
    } else {
      DealwithStage(SETTING_STAGE);
      lcd_control(102);
      lcd_control(113);
      lcd_control(123);
      lcd_control(133);
      lcd.setCursor(3, 2);
    }
    setting_digit_control();
  } else if (SETTING_DEPTH == 2) {
    //Adjusting SIDE of current Stage
    //Save after pressing "SET"
    Switch(SIDE, NEG, POS, fromKey);
    lcd_control(123);
    lcd.setCursor(14, 2);
    setting_digit_control();
  } else if (SETTING_DEPTH == 3) {
    //Adjusting SECONDS of current Stage (i*30)
    //Save immidiately
    Switch(stg.second30[SETTING_STAGE - 1], 0, 15, fromKey);
    count[SIDE] = stg.second30[SETTING_STAGE - 1] * 30;
    lcd_control(133);
    lcd.setCursor(19, 2);
    setting_digit_control();
  } else return;
}
//Function 5: EXCHANGE (Switch the current side speaking)
void Exchange() {
  if (isAttackDefense) {
    switch (ACTIVATED_TIMER) {
      case 1:
        SIDE_LOCK = SIDE;
        count[!SIDE] = (count[SIDE] > 20) ? 20 : count[SIDE];
        countMS[!SIDE] = (count[SIDE] > 20) ? 0 : countMS[SIDE];
        ACTIVATED_TIMER = 2;
        break;
      case 2:
        SIDE = SIDE_LOCK;
        ACTIVATED_TIMER = 1;
        init_light(SIDE_LOCK ? NEG : POS);
        break;
      default: break;
    }
    return;
  }
  SIDE_LOCK = SIDE;
  SIDE = !SIDE;
  if (count[SIDE_LOCK] > 0) {
    lcd_control(2);
  }
}
//Function 6: SETTINGS (Enter/Confirm)
void Setting() {
  if (MODE == READY) {
    lcd.clear();
    //Title of Setting (Line 1,col+0)
    for (byte i = 0; i < 19; i++) lcd.write(pgm_read_byte(&HINT_SETTING[i]));
    lcd.setCursor(0, 1);
    lcd.write('[');
    lcd.setCursor(19, 1);
    lcd.write(']');
    MODE = IN_SETTING;
    SETTING_DEPTH++;
    SETTING_STAGE = 0;
    button_control(3, 6, 4);
    Scroll(4);
    lcd.blink();
  }
  else if (SETTING_DEPTH == 1 && SETTING_STAGE > EXPECTED_STAGE) {
    //SETTINGS has 2 extra stages -- "Save" and "Reset"
    lcd.noBlink();
    lcd.clear();
    if (SETTING_STAGE == EXPECTED_STAGE + 1) SaveSetting();
    else {
      //RESET SETTINGS
      EEPROM.write(0, 39);
      ReadSetting();
    }
    SETTING_DEPTH = 0;
    lcd_control(1);
    init_timer();
    mode_control(READY);
    button_control(3, 6, 4);
    startTime = millis();
  }
  else if (SETTING_DEPTH == 1) {
    lcd.home();
    for (byte i = 0; i < 19; i++) lcd.write(pgm_read_byte(&HINT_EDIT_SIDE[i]));
    lcd.setCursor(14, 2);
    SETTING_DEPTH++;
  }
  else if (SETTING_DEPTH == 2) {
    byte buffer = stg.sides[SETTING_STAGE - 1];
    stg.sides[SETTING_STAGE - 1] = (buffer >= 2) ? SIDE + 2 : SIDE;
    lcd.home();
    for (byte i = 0; i < 19; i++) lcd.write(pgm_read_byte(&HINT_EDIT_TIME[i]));
    lcd.setCursor(19, 2);
    SETTING_DEPTH++;
  }
  else if (SETTING_DEPTH == 3) {
    lcd.home();
    for (byte i = 0; i < 19; i++) lcd.write(pgm_read_byte(&HINT_SETTING[i]));
    SETTING_DEPTH = 1;
    lcd.setCursor(4, 2);
  }
  else return;
}
/*Guide to corresponding function of the button pressed
  FUNCTION 0: (Empty)
          1: Play/Start speaking
          2: Pause
          3: Scroll backwards
          4: Scroll forwards
          5: Switch current side speaking
          6: Settings
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
    case 6:
      Setting();
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
  init_light(0);
  init_light(1);
  Wire.begin();
  lcd.init();
  lcd.backlight();
  showWelcome();
  buzzer_play(START_FREQ, SHORT_DURATION);
  delay(SHORT_DURATION);
  close_buzzer();
  ReadSetting();
  light_test();
  STAGE = 1;
  SETTING_DEPTH = 0;
  SETTING_STAGE = 0;
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
      currentTime = millis();
      timer_control();
      if (ACTIVATED_TIMER == 2 && isAttackDefense) {
        SIDE = !SIDE;
        timer_control();
      }
      startTime = currentTime;
      if (ButtonPressed == -1) continue;
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
  currentTime = millis();
  timer_control();
  if (ACTIVATED_TIMER == 2 && isAttackDefense) {
    SIDE = !SIDE;
    timer_control();
  }
  startTime = currentTime;
}
