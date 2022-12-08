#include "LIDARLite_v4LED.h"

LIDARLite_v4LED myLIDAR; //Click here to get the library: http://librarymanager/All#SparkFun_LIDARLitev4 by SparkFun

const int ledPin = 18;

const int trigPin1 = 27;
const int echoPin1 = 34;
const int ledPin1 = 17;
const int trigPin2 = 25;
const int echoPin2 = 35;
const int ledPin2 = 19;
int inRange = 0;

//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

//long duration;
long duration1;
float distanceCm1;
float distanceInch1;
long duration2;
float distanceCm2;
float distanceInch2;

void setup() {
  Serial.begin(115200);
  Serial.println("Qwiic LIDARLite_v4 examples");
  Wire.begin(); //Join I2C bus
  pinMode(ledPin, OUTPUT); //Sets the LED as an Output
  digitalWrite(ledPin, LOW);

  //check if LIDAR will acknowledge over I2C
  if (myLIDAR.begin() == false) {
    Serial.println("Device did not acknowledge! Freezing.");
    while(1);
  }
  Serial.println("LIDAR acknowledged!");

  pinMode(trigPin1, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin1, INPUT); // Sets the echoPin as an Input
  pinMode(ledPin1, OUTPUT); //Sets the LED as an Output
  pinMode(trigPin2, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin2, INPUT); // Sets the echoPin as an Input
  pinMode(ledPin2, OUTPUT); //Sets the LED as an Output
}

void loop() {
  float newDistance;
  float distance;

  newDistance = myLIDAR.getDistance();

  distance = (newDistance/100) * 3.28084;
  if (inRange == 0)
  {
    if (distance < 31 and distance > 30)
    {
      inRange = 1;
    }
  }
  // Light up LED indicator
  if (inRange == 1) {
    digitalWrite(ledPin, HIGH); 

    digitalWrite(trigPin1, LOW);// Clears the trigPin
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin1, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin1, LOW);
    duration1 = pulseIn(echoPin1, HIGH);
    distanceCm1 = duration1 * SOUND_SPEED/2;
  
    // Convert to inches
    distanceInch1 = distanceCm1 * CM_TO_INCH;

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
  else {
      digitalWrite(ledPin, LOW);
  }
  delay(20);  //Don't hammer too hard on the I2C bus
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