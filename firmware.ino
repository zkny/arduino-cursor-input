/*
  Parts used:
  -- Arduino Micro (Pro Miucro, arduino copy with ATmega32U4)
  -- Digital Rotary Encoder (GND VCC SW DT CLK)
  -- LCD Display (2x16, i2c interface mounted, GND VCC SDA SCL)
 */


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

void setup() {
  Serial.begin(115200);

  // rotary encoder setup
  pinMode(Button, INPUT_PULLUP);
  pinMode(InA, INPUT);
  pinMode(InB, INPUT);
  ALastState = digitalRead(InA);

  // LCD setup
  Wire.begin();
  Wire.beginTransmission(0x27);
  lcd.setBacklight(true);
  lcd.clear();

  // keyboard setup
  Keyboard.begin();
  lcd.print("Cursor mode");
}

void SendKeyboardInstruction(int Direction) {
  if (Direction == 1) {
    /* Encoder turning right */
    switch (OperatingMode) {
      case true:
        Keyboard.write(KEY_RIGHT_ARROW);
        break;
      case false:
        Keyboard.write(KEY_TAB);
        break;
    }
  } else {
    /* Encoder turning Left */
    switch (OperatingMode) {
      case true:
        Keyboard.write(KEY_LEFT_ARROW);
        break;
      case false:
        Keyboard.press(KEY_LEFT_SHIFT);
        Keyboard.write(KEY_TAB);
        delay(20);
        Keyboard.releaseAll();
        break;
    }
  }
}

void ChangeOperatingMode() {
  lcd.clear();
  /* Could be more then true or false, implement non binary method */
  OperatingMode = !OperatingMode;

  /* make cases for non binary states */
  switch (OperatingMode) {
    case true:
      lcd.print("Cursor mode");
      break;
    case false:
      lcd.print("Tab mode");
      break;
  }
}

void loop() {
  // detect rotary turning
  ACurrentState = digitalRead(InA);
  if (ACurrentState != ALastState) {
    if (digitalRead(InB) != ACurrentState) {
      if (smallCounter == -1) {
        SendKeyboardInstruction(0);
        smallCounter = 0;
      } else {
        smallCounter--;
      }
    } else {
      if (smallCounter == 1) {
        SendKeyboardInstruction(1);
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
      ChangeOperatingMode();
      ButtonLastState = 0;
    } else {
      ButtonLastState = 1;
    }
    delay(100); // prevent accidental double click
  }
}
