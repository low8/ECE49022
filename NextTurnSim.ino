#include "Timer.h" 
Timer timer; 

long number = 1234; 
int num1 = 0;
int num2 = 0;
int num3 = 0;
int num4 = 0;
int timer_event = 0;

int D1 = 1;
int D2 = 12;
int D3 = 14;
int D4 = 15;

int latchPin = 4;
int clockPin = 2;
int dataPin = 0;

const int button = 16;
int buttonState = 0;

int count = 0;
int numbers[4] ;
int anodePins[] = {1, 12, 14, 15};

//byte table[10] {B00000011, B10011111, B00100101, B00001101, B10011001, B01001001, B01000001, B00011111, B00000001, B00001001};
byte table[10] {B11111100, B01100000, B11011010, B11110010, B01100110, B10110110, B10111110, B11100000, B11111110, B11110110};

void setup() {
  Serial.begin(115200); 
  pinMode(D4, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(button, INPUT);
  digitalWrite(D4, LOW);
  digitalWrite(D3, LOW);
  digitalWrite(D2, LOW);
  digitalWrite(D1, LOW);
}

void loop() {
  timer.update(); 
  timer.stop(timer_event); 
  screenOff(); 
  buttonState = digitalRead(button);
  if (number < 9999) {
    separate(number);
    delay(1);
    Display();
    /*timer_event = timer.every(1, Display); */
  }
  if (buttonState == HIGH) {
      screenOff();
      number += 1;
      delay(1000);
  } 
}

void separate(long num) { 
  num1 = num / 1000;
  numbers[0] = num1;
  int num1_remove = num - (num1 * 1000);
  num2 = num1_remove / 100;
  numbers[1] = num2;
  int num2_remove = num1_remove - (num2 * 100);
  num3 = num2_remove / 10;
  numbers[2] = num3;
  num4 = num2_remove - (num3 * 10);
  numbers[3] = num4;
}

void Display() {
  screenOff();
  digitalWrite(latchPin, LOW); 
  shiftOut(dataPin, clockPin, LSBFIRST, table[numbers[count]]); 
  digitalWrite(anodePins[count], HIGH);
  digitalWrite(latchPin, HIGH); 
  delay(3);
  count++; 
  if (count == 4) { 
    count = 0;
  }
  Serial.println("Here!");
}

void screenOff() { 
  digitalWrite(D4, LOW);
  digitalWrite(D3, LOW);
  digitalWrite(D2, LOW);
  digitalWrite(D1, LOW);
}