#include <Servo.h>
#include <pt.h>
#include "pitches.h"

struct pt motorThread;
struct pt songThread;
struct pt buttonThread;

Servo s1;
Servo s2;

int s1Pin = 9;
int s2Pin = 10;
int pPin = 2;
int bInPin = 4;

bool powerState = false;

int melody[] = {
  NOTE_E2,
  NOTE_E2, NOTE_F2, 0, 0, 0,
  NOTE_E2, NOTE_F2, NOTE_E2, 0, 0,
  NOTE_E2, NOTE_F2, 0, NOTE_E2, NOTE_F2, 0,
  NOTE_E2, NOTE_F2, 0, NOTE_E2, NOTE_F2, 0,
  NOTE_E2, NOTE_F2, 0, NOTE_E2, NOTE_F2, 0,
  NOTE_E2, NOTE_F2, 0, NOTE_E2, NOTE_F2, 0,
  NOTE_E2, NOTE_F2, 0, NOTE_E2, NOTE_F2, 0,
  NOTE_E2, NOTE_F2, 0, NOTE_E2, NOTE_F2, 0,
  0, 0, 0, 0,
};

int durations[] = {
  1,
  4, 8, 8, 4, 4,
  4, 4, 8, 8, 4,
  4, 8, 8, 4, 8, 8,
  4, 8, 8, 4, 8, 8,
  4, 8, 8, 4, 8, 8,
  4, 8, 8, 4, 8, 8,
  4, 8, 8, 4, 8, 8,
  4, 8, 8, 4, 8, 8,
  1, 1, 1, 1,
};

PT_THREAD(motorLoop()) {
  static unsigned long time = 0;
  PT_BEGIN(&motorThread);
  while(1) {
    if(powerState) { 
      time = millis();
      PT_WAIT_UNTIL(&motorThread, millis() - time > 2000);
      s1.write(90);
      s2.write(45);
      time = millis();
      PT_WAIT_UNTIL(&motorThread, millis() - time > 2000);
      s1.write(30);
      s2.write(135);
    }
    else {
      time = millis();
      PT_WAIT_UNTIL(&motorThread, millis() - time > 100);
    }
  }
  PT_END(&motorThread);
}

PT_THREAD(songLoop()) {
  static unsigned long time = 0;
  static int pauseBetweenNotes = 0;
  static int thisNote = 0;
  static int dur = 0;
  PT_BEGIN(&songThread);
  while(1) {
      if(powerState) {
        Serial.println("SONG RUN");
        for(thisNote = 0; thisNote < sizeof(melody) / sizeof(int); thisNote++) {    
          dur = 1000 / durations[thisNote];
          tone(2, melody[thisNote], dur);
          pauseBetweenNotes = dur * 1.30;
          time = millis();
          PT_WAIT_UNTIL(&songThread, millis() - time > pauseBetweenNotes);
          noTone(2);
        }
      }
      else {
        time = millis();
        PT_WAIT_UNTIL(&songThread, millis() - time > 100);
      }
  }
  PT_END(&songThread);
}

PT_THREAD(buttonLoop()) {
  static int buttonState = 0;
  static bool pressed = false;
  static unsigned long time = 0;
  PT_BEGIN(&buttonThread);
  while(1) {
    buttonState = digitalRead(bInPin);
    if(!pressed && buttonState == HIGH) {
      powerState = !powerState;
      Serial.print("Power State: ");
      Serial.println(powerState);
      pressed = true;
      Serial.print("Pressed State: ");
      Serial.println(pressed);   
    }
    else if(pressed && buttonState == LOW) {
      pressed = false;
      Serial.print("Pressed State: ");
      Serial.println(pressed);   
    }
    time = millis();
    PT_WAIT_UNTIL(&buttonThread, millis() - time > 100);
  }
  PT_END(&buttonThread);
}

void setup() {
  pinMode(pPin, OUTPUT);
  pinMode(bInPin, INPUT);
  Serial.begin(9600);
  s1.attach(s1Pin);
  s2.attach(s2Pin);
  s1.write(30);
  s2.write(135);
  
  PT_INIT(&motorThread);
  PT_INIT(&songThread);
  PT_INIT(&buttonThread);
}

void loop() {
   
  motorLoop();
  songLoop();
  buttonLoop(); 
}
