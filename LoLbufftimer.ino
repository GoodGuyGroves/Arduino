// Import LiquidCrystal library
// See http://arduino.cc/en/Reference/LiquidCrystal
#include "LiquidCrystal.h"

// Tell it the pins we have used on the arduino
LiquidCrystal lcd(12,11,5,4,3,2);

// Assign the used pins to vars
const int btnSelect = 7;
const int btnReset = 8;
const int buzzer = 10;
const int blueRGB = 17;   // Analog 3
const int greenRGB = 18;  // Analog 4
const int redRGB = 19;    // Analog 5

const String objectives[] = {"Baron", "Dragon", "Our Blue", "Our Red", "Their Blue", "Their Red"};
const int lastBuff = 5;
long timers[] = {20, 15, 10, 10, 10, 10};
long timerTemplate[] = {20, 15, 10, 10, 10, 10};
int objSel = 0;
long prevTimerMilli = 0;
long btnSelPrevMilli = 0;
long btnResPrevMilli = 0;
long prevCaseMilli = 0;
long prevUpdateMilli = 0;
long prevLEDOnMilli = 0;
long prevLEDOffMilli = 0;
long timerThreshold = 9;

// Variables for time keeping
int timersOn[] = {0, 0, 0, 0, 0, 0};
const long loopInterval = 1000;
const long btnInterval = 150;
const long LEDInterval = 500;
long loopTimer;

// Variables to hold both buttons states
int selState;
int resState;

void setup() {
  // Set the baud rate for serial port messages
  Serial.begin(300);
  
  // I'm using an LCD that is 2 lines of 16 characters
  lcd.begin(16, 2);
  
  // Clear old text off the LCD
  lcd.clear();
  
  // Write a starting phrase if you'd like
  //lcd.print("Welcome...");
  
  // Make the buttons pins be input
  pinMode(btnSelect, INPUT);
  pinMode(btnReset, INPUT);
  
  // Make the LED's and buzzer be output
  pinMode(blueRGB, OUTPUT);
  pinMode(greenRGB, OUTPUT);
  pinMode(redRGB, OUTPUT);
  pinMode(buzzer, OUTPUT);
  
  // Run this in setup to show the first buff timer on screen
  lcd.setCursor(0,0);
  lcd.print(objectives[objSel]);
  lcd.setCursor(0,1);
  lcd.print(timers[objSel]);
  lcd.print(" seconds");
  objSel++;
  
}

void loop() {
   
  // A single variable to test all loop timers against
  loopTimer = millis();
  
  // Get both buttons states  
  selState = digitalRead(btnSelect);
  //Serial.println(selState);
  resState = digitalRead(btnReset);
  //Serial.println(resState);
  
  buffCycle();
  updateTimers();
  refreshTimer();
  resetTimer();

 ///*
 if (loopTimer - prevLEDOnMilli > LEDInterval) {
   prevLEDOnMilli = loopTimer;
     if (timers[0] < timerThreshold + 1) {
       analogWrite(redRGB, 185);
       analogWrite(greenRGB, 105);
       analogWrite(blueRGB, 155);
     }
     if (timers[1] < timerThreshold + 1) {
       analogWrite(redRGB, 0);
       analogWrite(greenRGB, 255);
       analogWrite(blueRGB, 0);
     }
     if (timers[2] < timerThreshold + 1 || timers[4] < timerThreshold + 1) {
      analogWrite(blueRGB, 255);
     }
     if (timers[3] < timerThreshold + 1 || timers[5] < timerThreshold + 1) {
      analogWrite(redRGB, 255);
     }
 }

//*
 if (loopTimer - prevLEDOffMilli > LEDInterval / 2) {
  prevLEDOffMilli = loopTimer;
  analogWrite(redRGB, 0);
  analogWrite(greenRGB, 0);
  analogWrite(blueRGB, 0);
 }
 //*/

  //debug();
  
}

/* ----- ----- ----- ----- ----- FUNCTIONS ----- ----- ----- ----- ----- */

// Prints stuff out to serial for debugging purposes
void debug() {
  Serial.println();
  Serial.print(objectives[objSel -1]);
  Serial.print(" is being displayed with timer ");
  Serial.println(timers[objSel -1]);
  for (int x = 0; x < lastBuff + 1; x++) {
    Serial.print(objectives[x]);
    Serial.print(": ");
    Serial.println(timers[x]);
  }
}

// This cycles through the buffs on-screen
void buffCycle() {
  if (loopTimer - btnSelPrevMilli > btnInterval) {
    btnSelPrevMilli = loopTimer;
    if (selState == LOW) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(objectives[objSel]);
      lcd.setCursor(0,1);
      lcd.print("             ");
      lcd.setCursor(0,1);
      lcd.print(timers[objSel]);
      lcd.print(" seconds");
      objSel++;
      if (objSel > lastBuff) {
        objSel = 0;
      } 
    }
  }
}


// Updates all timers that are turned on
// Fix to work for when a timer hits zero
// Edit: Negative counting is cool to see how long it's been up
void updateTimers() {
  if (loopTimer - prevTimerMilli > loopInterval) {
    prevTimerMilli = loopTimer;
    for (int i = 0; i < lastBuff + 1; i++) {
      if (timersOn[i] > 0) {
        timers[i] = timers[i] - 1;
      }
    }
  }
}

// Updates timer on the screen for the selected buff
void refreshTimer() {
  if (loopTimer - prevUpdateMilli > loopInterval) {
    prevUpdateMilli = loopTimer;
    if (objSel != 0) {
      if (timersOn[objSel - 1] > 0) {
        clearLine();
          lcd.print(timers[objSel - 1]);
          lcd.print(" seconds");
      }
    }
    else {
      if (timersOn[lastBuff] > 0) {
        clearLine();
        lcd.print(timers[lastBuff]);
        lcd.print(" seconds");
      }
    }
  }
}

// Resets the currently displayed timer
void resetTimer() {
  if (loopTimer - btnResPrevMilli > btnInterval / 10) {
    btnResPrevMilli = loopTimer;
    if (resState == LOW) {
      if (objSel != 0) {
        timers[objSel - 1] = timerTemplate[objSel -1];// + 1;
        timersOn[objSel - 1] = 1;
      }
      else {
        timers[lastBuff] = timerTemplate[lastBuff];// + 1;
        timersOn[lastBuff] = 1;
      }
    }
  }
}

void clearLine() {
  lcd.setCursor(0,1);
  lcd.print("                ");
  lcd.setCursor(0,1);
}
