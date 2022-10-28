/******************************************************************************
  Reads the distance something is in front of LIDAR and prints it to the Serial port

  Priyanka Makin @ SparkX Labs
  Original Creation Date: Sept 30, 2019

  This code is Lemonadeware; if you see me (or any other SparkFun employee) at the
  local, and you've found our code helpful, please buy us a round!

  Hardware Connections:
  Plug Qwiic LIDAR into Qwiic RedBoard using Qwiic cable.
  Set serial monitor to 115200 baud.

  Distributed as-is; no warranty is given.
******************************************************************************/
#include "LIDARLite_v4LED.h"

LIDARLite_v4LED myLIDAR; //Click here to get the library: http://librarymanager/All#SparkFun_LIDARLitev4 by SparkFun

const int ledPin = 14;

void setup() {
  Serial.begin(115200);
  Serial.println("Qwiic LIDARLite_v4 examples");
  Wire.begin(); //Join I2C bus
  pinMode(ledPin, OUTPUT); //Sets the LED as an Output

  //check if LIDAR will acknowledge over I2C
  if (myLIDAR.begin() == false) {
    Serial.println("Device did not acknowledge! Freezing.");
    while(1);
  }
  Serial.println("LIDAR acknowledged!");
}

void loop() {
  float newDistance;
  float distance;

  //getDistance() returns the distance reading in cm
  newDistance = myLIDAR.getDistance();

  //Print to Serial port
  Serial.print("New distance: ");
  distance = (newDistance/100) * 3.28084;
  Serial.print(distance);
  //Serial.print((newDistance/100) * 3.28084);
  Serial.println(" feet");

    // Light up LED indicator
  if (distance < 27.5 and distance > 27) {
    digitalWrite(ledPin, HIGH); 
  }
  else {
    digitalWrite(ledPin, LOW);
  }

  delay(20);  //Don't hammer too hard on the I2C bus
}
