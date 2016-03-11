#include <LiquidCrystal_I2C.h>
#include <LedControl.h>
/*TEST_BETA3 @Demkors Infinity
  An Arduino-based timer for debate competition
  TEST basic system availability
  USING Ocrobot (Arduino compatible) Mega 2560,
    MAX7219-based 8-digit display & LCD 2004 (via I2C)
*/

//Output pins
#define DIGIT_DIN 5
#define DIGIT_CS 6
#define DIGIT_CLK 7
//Hardware limits
#define MAX_PCS 1
#define MAX_SEGMENT 8
#define MAX_BUTTON 3
//Self-defined limits
#define MAX_SEC 999
#define MAX_DIGIT 3
#define CUSTOM_CHAR 5
#define MAX_STAGE 11
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
//Declaration of SYSTEM STATUS
#define READY 0
#define SPEAKING 1
#define PAUSED 2
#define FINISHED 3
#define IN_SETTING 4
//The period "Welcome" panel should maintain
#define DELAY 3000

// ********* DECLARATION OF CONSTANTS *********
//TITLE OF EVERY STAGE (Line 1,col+1)
const char *TITLE[] = {
  "   1st Statement  ",
  "2nd Attack Defense",
  "3rd Attack Defense",
  "  1st Conclusion  ",
  "    Free Debate   ",
  " Final Conclusion ",
  "   Audience Q&A   "
};
//CURRENT SIDE TO SPEAK/SPEAKING (Line 2,col+7)
const char CURRENT_SIDE[2][9] = {
  "NEGATIVE",
  "POSITIVE"
};
//START-UP TITLE IN All lines(col+2)
const char *START_TITLE[] = {
  "Timer for Debate",
  " --- BETA 3 ---",
  "Demkors Infinity",
  "   9 Mar,2016"
};
//CURRENT STATUS OF THE SYSTEM (Line 3,col+7)
const char SYSTEM_STATUS[4][11] = {
  "  Ready   ",
  "Speaking..",
  "  Paused  ",
  "*Finished*"
};
/*Control panel, consisting of 3 buttons
  & a public pin for detecting if any button is pushed.
  Use pins A0~A3(54~57 on Arduino Mega) to connect buttons.*/
const byte BUTTON_PIN[MAX_BUTTON] = {
  54, 55, 56
};
const byte ANY_BUTTON_PRESSED = 57;

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

// ********* HARDWARE DECLARATION *********
//LCD 2004 screen to show the current state.
LiquidCrystal_I2C lcd(0x27, 20, 4);
/*Control the 8*8 matrix & the 8-digit 7-segment display SEPARATELY
  [WARNING] The matrix module has been removed since BETA3        */
LedControl digit = LedControl(DIGIT_DIN, DIGIT_CLK, DIGIT_CS, MAX_PCS);

// ********* DECLARATION OF PUBLIC VARIABLES *********
//Current side speaking (POSITIVE in default)
byte SIDE = POS;
//Current stage
byte STAGE = 0;
//Current status of system
byte SYSTEM = READY;
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


// ********* CODE SEGMENT *********
//HARDWARE MODULES CONTROL
//The "PRINT" function in LiquidCrystal library is ineffective. Here rewrites it
void print(const char src[]) {
  for (int i = 0; src[i]; i++) lcd.write(src[i]);
}

/*Format required contents & print them on the LCD 2004 screen
  1: TITLE OF EVERY STAGE (Line 1,col+1)
  2: CURRENT SIDE TO SPEAK/SPEAKING (Line 2,col+7)
  3: CURRENT STATUS OF THE SYSTEM (Line 3,col+7)
  4: CURRENT FUNCTION OF BUTTONS (Line 4,col=20/(MAX_BUTTON+1))
  92: THE NUMBER OF CURRENT STAGE (e.g. [01])(Line 2,col+0)
  93: THE AVAILIBLE TIME OF EACH SIDE IN THIS STAGE (e.g. 180s)(Line 3,col+0)
*/
void lcd_control(byte mode) {
  static byte diff = 20 / (MAX_BUTTON + 1);
  switch (mode) {
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
      print(SYSTEM_STATUS[SYSTEM]);
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
    print(START_TITLE[i]);
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

//Control LEDs that point out the time remaining (to be added in Beta 4)

//TIMERS CONTROL
//Initialize both timers (when switching stage)
void init_timer() {
  countMS[POS] = countMS[NEG] = 0;
  if (STAGE == 9) {
    ACTIVATED_TIMER = 2;
    SIDE = NEG;
  }
  else if (STAGE <= 0 || STAGE > MAX_STAGE) {
    ACTIVATED_TIMER = 0;
    count[POS] = count[NEG] = 0;
    return;
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
  lcd_control(2);
  lcd_control(92);
  lcd_control(93);
  digit_control();
  //light_control();
}

//Check the current time & update the present timer
void timer_control() {
  if (SYSTEM == SPEAKING) {
    currentTime = millis();
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
    if (count[SIDE] == 0) {
      //The time of current side has run out
      countMS[SIDE] = 0;
      if (ACTIVATED_TIMER == 2) Exchange();
      //else beep;
      if (count[!SIDE] == 0) {
        SYSTEM = FINISHED;
        button_control(0, 1, 4);
      }
      else {
        SYSTEM = PAUSED;
        button_control(0, 0, 1);
      }
    }
    digit_control();
    //light_control();
    startTime = currentTime;
  }
}

//CONTROL PANEL & BUTTON FUNCTIONS
// [WARNING] Function 0 means "NOT USED"
//Modify the functions of buttons (Update status shown in LCD 2004 as well)
void button_control(byte btn0, byte btn1, byte btn2) {
  button[0] = btn0;
  button[1] = btn1;
  button[2] = btn2;
  lcd_control(4);
  lcd_control(3);
}

//Function 1: PLAY
void Play() {
  if (SYSTEM != SPEAKING) {
    SYSTEM = SPEAKING;
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
  if (SYSTEM == SPEAKING) {
    SYSTEM = PAUSED;
    button_control(0, 1, 4);
  }
  else return;
}
//Function 3/4: SCROLL forwards/backwards
// ONLY supports FORWARD (->) in BETA 3 & 4
void Scroll() {
  STAGE++;
  if (STAGE > MAX_STAGE) STAGE = 1;
  SYSTEM = READY;
  init_timer();
  lcd_control(1);
  lcd_control(3);
  //light_control();
}
//Function 5: EXCHANGE (Switch the current side speaking)
void Exchange() {
  if (count[SIDE] == 0) SIDE = !SIDE;
  else if (count[SIDE] > 1) {
    SIDE = !SIDE;
    lcd_control(2);
  }
  else {
    //[CAUTION] In order to test the system faster, the MS timers"1000"
    while (countMS[SIDE] < 1000) {
      currentTime = millis();
      countMS[SIDE] += (currentTime - startTime);
      startTime = currentTime;
    }
    count[SIDE] = countMS[SIDE] = 0;
    //finishing beep
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
    case 3: case 4:
      Scroll(); break;
    case 5:
      Exchange(); break;
    default: break;
  }
  startTime = millis();
}

//MAIN PROGRAM
//Initialize the system
void setup() {
  digit.shutdown(0, false);
  digit.setIntensity(0, 4);
  digit.clearDisplay(0);
  for (byte i = 0; i < MAX_BUTTON; i++) {
    pinMode(BUTTON_PIN[i], INPUT);
    digitalWrite(BUTTON_PIN[i], HIGH);
  }
  pinMode(ANY_BUTTON_PRESSED, INPUT);
  digitalWrite(ANY_BUTTON_PRESSED, HIGH);
  digit_control();
  Wire.begin();
  lcd.init();
  lcd.backlight();
  showWelcome();
  delay(DELAY);
  STAGE = 1;
  lcd.clear();
  lcd_control(1);
  init_timer();
  button_control(0, 1, 4);
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
