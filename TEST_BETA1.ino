#include <LiquidCrystal_I2C.h>
#include <LedControl.h>
//TEST_BETA1 @Demkors Infinity
//Built-in timer test
//USING 8*8 matrix & 8-digit display, both based on MAX7219

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

//*** BETA 1: TIMER TEST ***
#define MAX_SEC 999
unsigned short count[2]={0,0};

//LCD 2004 screen to show the current status.
LiquidCrystal_I2C lcd(0x27,20,4);
//Control the 8*8 matrix & the 8-digit segment display
LedControl digit=LedControl(DIGIT_DIN,DIGIT_CLK,DIGIT_CS,MAX_PCS);
LedControl matrix=LedControl(MATRIX_DIN,MATRIX_CLK,MATRIX_CS,MAX_PCS);
//Current side speaking (POSITIVE default)
byte SIDE=POS;
/*Count milliseconds passed;
  reset to 0 & count other timers when exceeding 1000*/
unsigned short countMS=0;
long startTime;
long currentTime;

const String TITLE[]={
  "  Timer for Debate",
  "   --- BETA 1 ---",
  "  Demkors Infinity",
  "    Dec 27,2015"
};

//"PRINT" is ineffective. Here "WRITE" every char to the screen.
void layout(char src[]){
  for(int i=0;src[i];i++) lcd.write(src[i]);
}
void StrOut(String src){
  int len=src.length();
  for(int i=0;i<len;i++) lcd.write(src.charAt(i));
}

void lcd_title(){
  for(byte i=0;i<4;i++){
    lcd.setCursor(0,i);
    StrOut(TITLE[i]);
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
void showSec(unsigned short sec,byte side){
  byte pos=side?5:0;
  char str[7];
  itoa(sec,str,10);
  byte len;
  for(len=0;str[len];len++);
  len--;
  for(byte i=0;pos+i<MAX_SEGMENT&&len>=0;i++,len--){
    digit.setChar(0,pos+i,str[len],false);
  }
}
void setup() {
  // put your setup code here, to run once:
 digit.shutdown(0,false);
 matrix.shutdown(0,false);
 digit.setIntensity(0,4);
 matrix.setIntensity(0,4);
 digit.clearDisplay(0);
 matrix.clearDisplay(0);
 Wire.begin();
 lcd.init();
 lcd.backlight();
 lcd_title();
 matrix.setLed(0,0,0,true);
 currentTime=millis();
}

void loop() {
  // put your main code here, to run repeatedly:
 startTime=currentTime;
 static byte num=0,row=0,col=0;
 currentTime=millis();
 countMS+=(currentTime-startTime);
 if(countMS>=100){
  countMS-=100;
  count[0]++;
  if(count[0]>9999) count[0]=0;
  matrix.setLed(0,row,col++,false);
  if(col>7){col=0; row++;}
  if(row>7) row=0;
  matrix.setLed(0,row,col,true);
  showSec(count[0],NEG);
 }
}
