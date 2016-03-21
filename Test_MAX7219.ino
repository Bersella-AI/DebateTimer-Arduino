#include <LiquidCrystal_I2C.h>
#include <LedControl.h>
//Test MAX7219 @Demkors Infinity
//ATTENTION: LCD 2004(I2C) is required to show status

#define MATRIX_DIN 5
#define MATRIX_CS 6
#define MATRIX_CLK 7
#define DIGIT_DIN 8
#define DIGIT_CS 9
#define DIGIT_CLK 10
#define MAX_PCS 1
LedControl digit=LedControl(DIGIT_DIN,DIGIT_CLK,DIGIT_CS,MAX_PCS);
LedControl matrix=LedControl(MATRIX_DIN,MATRIX_CLK,MATRIX_CS,MAX_PCS);
LiquidCrystal_I2C lcd(0x27,20,4);
const unsigned long DELAY=200;

//"PRINT" is ineffective. Here "WRITE" every char to the screen.
void layout(char src[]){
  for(int i=0;src[i];i++) lcd.write(src[i]);
}
void StrOut(String src){
  for(int i=0;i<src.length();i++) lcd.write(src.charAt(i));
}

void lcd_title(){
  lcd.setCursor(4,0);
  layout("Test MAX7219");
  lcd.setCursor(3,1);
  layout("--- BETA 0 ---");
  lcd.setCursor(2,2);
  layout("Demkors Infinity");
  lcd.setCursor(4,3);
  layout("Dec 26,2015");
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
}

void loop() {
  // put your main code here, to run repeatedly:
  static byte num=0;
  for (byte row=0;row<8;row++){
    for (byte col=0;col<8;col++){
      digit.setDigit(0,col,num,true);
      matrix.setLed(0,row,col,true);
      delay(DELAY);
      digit.setChar(0,col,' ',false);
      matrix.setLed(0,row,col,false);
    }
    num++;
    if(num==10) num=0;
  }
}
