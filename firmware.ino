#include <LiquidCrystal_PCF8574.h>
#include <Wire.h>
#include "Keyboard.h"

#define Button 9 // rotary encoder SW
#define InA 7 // rotary encoder CLK
#define InB 8 // rotary encode DT

int smallCounter = 0; // Prevent double steps
int ACurrentState;
int ALastState;
int ButtonLastState = 1;

bool OperatingMode = true; // false is cursor, true is tabs, there could be more than 2.

LiquidCrystal_PCF8574 lcd(0x27); // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup()
{
  Serial.begin(115200);

  // rotary encoder setup
  pinMode(Button, INPUT_PULLUP);
  pinMode(InA, INPUT);
  pinMode(InB, INPUT);
  ALastState = digitalRead(InA);

  // LCD setup
  Wire.begin();
  Wire.beginTransmission(0x27);

  if (Wire.endTransmission() == 0) {
    lcd.begin(16, 2);
    lcd.setBacklight(true);
    lcd.setCursor(0, 0);
    lcd.clear();
  } else {
    lcd.setBacklight(false);
  }
    

  

  // keyboard setup
  Keyboard.begin();
  lcd.print("Cursor mode");
}


void loop()
{
// detect rotary turning      
  ACurrentState = digitalRead(InA);
  if (ACurrentState != ALastState) {
    if (digitalRead(InB) != ACurrentState) {
      if (smallCounter == -1) {  
       if (OperatingMode) {
         Keyboard.write(KEY_LEFT_ARROW);
        } else {     
         Keyboard.press(KEY_LEFT_SHIFT);
         Keyboard.write(KEY_TAB);
         delay(20);
         Keyboard.releaseAll();
       }
       smallCounter = 0;
     } else {
      smallCounter--;
     }
     
    } else {
      if (smallCounter == 1) {
       if (OperatingMode) {
         Keyboard.write(KEY_RIGHT_ARROW);
       } else {
         Keyboard.write(KEY_TAB);
       }
        smallCounter = 0;
      } else {
       smallCounter++; 
      }
} 
    ALastState = ACurrentState;
  }

  // detect button presses
  int ButtonState = digitalRead(Button);
  if (ButtonLastState != ButtonState) {
    if (ButtonState == 0) {
      lcd.clear();  
      ButtonLastState = 0;
      OperatingMode = !OperatingMode;
      if (OperatingMode) {
        lcd.print("Cursor mode");
      } else {
        lcd.print("Tab mode");
      }
    } else {
      ButtonLastState = 1;
    }
   delay(100); // prevent accidental double click 
  }     
}
