#include <LiquidCrystal_I2C.h>
#include <LedControl.h>
//TEST_BETA2 @Demkors Infinity
//TEST built-in timer, special symbols(LCD), buzzer
//USING 8*8 matrix & 8-digit display, both based on MAX7219
//LCD 2004 uses HD44780U(ROM A00)

//Output pins
#define MATRIX_DIN 5
#define MATRIX_CS 6
#define MATRIX_CLK 7
#define DIGIT_DIN 8
#define DIGIT_CS 9
#define DIGIT_CLK 10
//Hardware limits
#define MAX_PCS 1
#define MAX_SEGMENT 8
//Control panel, consisting of 3 buttons.
#define PLAY_BUTTON 3
#define LEFT_BUTTON 4
#define RIGHT_BUTTON 2
//Time required for each side in every stage.(Measured in S)
#define FIRST_STATEMENT 180
#define SECOND_STATEMENT 120
#define FREE_DEBATE 300
#define FINAL_CONCLUSION 180
//Numbers representing each side
#define POS 1
#define NEG 0

//*** BETA 2: TIMER & BUTTON TEST ***
#define MAX_SEC 999
#define MAX_DIGIT 3
unsigned short count[2]={0,0};

//LCD 2004 screen to show the current status.
LiquidCrystal_I2C lcd(0x27,20,4);
//Control the 8*8 matrix & the 8-digit segment display
LedControl digit=LedControl(DIGIT_DIN,DIGIT_CLK,DIGIT_CS,MAX_PCS);
LedControl matrix=LedControl(MATRIX_DIN,MATRIX_CLK,MATRIX_CS,MAX_PCS);
//Current side speaking (POSITIVE default)
byte SIDE=POS;
byte STATE=0;
/*Count milliseconds passed;
  reset to 0 & count other timers when exceeding 1000*/
unsigned short countMS[2]={0,0};
long startTime;
long currentTime;

const char *TITLE[]={
  "  Timer for Debate",
  "   --- BETA 2 ---",
  "  Demkors Infinity",
  "    Dec 27,2015"
};

//*** BETA 2: TEST SPECIAL SYMBOLS ON LCD 2004 ***
// Introductions must be "defined" in near future
byte SYMBOL[4][8]={
  {    //PAUSE
    B11011,
    B11011,
    B11011,
    B11011,
    B11011,
    B11011,
    B11011,
    B11011
  },{  //PLAY
    B10000,
    B11000,
    B11100,
    B11110,
    B11100,
    B11000,
    B10000,
    B00000
  },{  //LEFT
    B00010,
    B00100,
    B01000,
    B11111,
    B11111,
    B01000,
    B00100,
    B00010
  },{  //RIGHT
    B01000,
    B00100,
    B00010,
    B11111,
    B11111,
    B00010,
    B00100,
    B01000
  }
};

//"PRINT" is ineffective. Here "WRITE" every char to the screen.
void layout(const char src[]){
  for(int i=0;src[i];i++) lcd.write(src[i]);
}

void lcd_title(){
  for(byte i=0;i<4;i++){
    lcd.setCursor(0,i);
    layout(TITLE[i]);
    //*** BETA 2: Set custom symbols into LCD
    lcd.createChar(i,SYMBOL[i]);
  }
}
void paint(byte side){
  matrix.clearDisplay(0);
  //POSITIVE sit on the left
  if(side){
    matrix.setColumn(0,0,B00001000);
    matrix.setColumn(0,1,B00011100);
    matrix.setColumn(0,2,B00111110);
  }else{
    matrix.setColumn(0,7,B00001000);
    matrix.setColumn(0,6,B00011100);
    matrix.setColumn(0,5,B00111110);
  }
}
void connectNum(char *target,byte pos,char num[]){
  switch(strlen(num)){
    case 0: strcat(target,"   ");break; //3 spacer
    case 1: strcat(target,"  ");break;  //2 spacer
    case 2: strcat(target," ");break;   //1 spacer
    default: break;  //do nothing
  }
  strcat(target,num);
}
void showSec(){
  //POSITIVE timer shown in 6~8th digit, NEGATIVE shown in 1~3
  char str[9]="",trans[7];
  byte i=0,j;
  itoa(count[POS],trans,10);
  connectNum(str,i,trans);
  for(i=MAX_DIGIT;i<MAX_SEGMENT-MAX_DIGIT;i++) str[i]=' ';
  str[i]='\0';
  itoa(count[NEG],trans,10);
  connectNum(str,i,trans);
  for(i=strlen(str)-1,j=0;j<MAX_SEGMENT&&i>=0;j++,i--)
   digit.setChar(0,j,str[i],false);
}

//*** BETA 2: TEST SPECIAL SYMBOL ON LCD 2004 ***
void lcd_test(){
  lcd.setCursor(18,3);
  lcd.write(STATE++);
  if(STATE>3) STATE=0;
}

void setup() {
  // put your setup code here, to run once:
 pinMode(PLAY_BUTTON,INPUT);
 digit.shutdown(0,false);
 matrix.shutdown(0,false);
 digit.setIntensity(0,4);
 matrix.setIntensity(0,4);
 digit.clearDisplay(0);
 paint(SIDE);
 showSec();
 Wire.begin();
 lcd.init();
 lcd.backlight();
 lcd_title();
 digitalWrite(PLAY_BUTTON,HIGH);
 currentTime=millis();
}

void loop() {
  // put your main code here, to run repeatedly:
 startTime=currentTime;
 static byte num=0,row=0,col=0;
 if(digitalRead(PLAY_BUTTON)==LOW){
  while(digitalRead(PLAY_BUTTON)==LOW);
  if(SIDE==POS) SIDE=NEG;
  else SIDE=POS;
  paint(SIDE);
  showSec();
  lcd_test();
  startTime=millis();
 }
 currentTime=millis();
 countMS[SIDE]+=(currentTime-startTime);
 if(countMS[SIDE]>=100){
  countMS[SIDE]-=100;
  count[SIDE]++;
  if(count[SIDE]>999) count[SIDE]=0;
  showSec();
 }
}
