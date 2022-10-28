/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-hc-sr04-ultrasonic-arduino/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

//const int trigPin = 15;
//const int echoPin = 2;
//const int ledPin = 13;
const int trigPin1 = 27;
const int echoPin1 = 34;
const int ledPin1 = 17;
const int trigPin2 = 25;
const int echoPin2 = 35;
const int ledPin2 = 19;

//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

//long duration;
//float distanceCm;
//float distanceInch;
long duration1;
float distanceCm1;
float distanceInch1;
long duration2;
float distanceCm2;
float distanceInch2;

void setup() {
  Serial.begin(115200); // Starts the serial communication
  //pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  //pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  //pinMode(ledPin, OUTPUT); //Sets the LED as an Output
  pinMode(trigPin1, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin1, INPUT); // Sets the echoPin as an Input
  pinMode(ledPin1, OUTPUT); //Sets the LED as an Output
  pinMode(trigPin2, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin2, INPUT); // Sets the echoPin as an Input
  pinMode(ledPin2, OUTPUT); //Sets the LED as an Output
}

void loop() {
  digitalWrite(trigPin1, LOW);// Clears the trigPin
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  duration1 = pulseIn(echoPin1, HIGH);
  distanceCm1 = duration1 * SOUND_SPEED/2;
  
  // Convert to inches
  //distanceInch = distanceCm * CM_TO_INCH;
  distanceInch1 = distanceCm1 * CM_TO_INCH;
  Serial.print("Distance1 (inch): ");
  Serial.println(distanceInch1);
  Serial.print("Distance2 (inch): ");
  Serial.println(distanceInch2);

  if (distanceInch1 < 34 and distanceInch2 > 34) {
    digitalWrite(ledPin1, HIGH); 
    digitalWrite(ledPin2, LOW);
    delay(5000);
  }
  else if (distanceInch2 < 34 and distanceInch1 > 34) {
    digitalWrite(ledPin2, HIGH); 
    digitalWrite(ledPin1, LOW);
    delay(5000);
  }
  else if (distanceInch1 < 34 and distanceInch2 < 34) {
    digitalWrite(ledPin1, HIGH); 
    digitalWrite(ledPin2, HIGH); 
    delay(5000);
  }
  else {
    digitalWrite(ledPin1, LOW);
    digitalWrite(ledPin2, LOW);
  }

  delay(100);
  test();
}

void test()
{
  digitalWrite(trigPin2, LOW);// Clears the trigPin
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration2 = pulseIn(echoPin2, HIGH);
  distanceCm2 = duration2 * SOUND_SPEED/2;
  distanceInch2 = distanceCm2 * CM_TO_INCH;
}