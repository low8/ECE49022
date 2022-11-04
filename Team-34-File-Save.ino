#include "SPIFFS.h"

void setup( ) {
	Serial.begin(115200);
	
	if (!SPIFFS.begin(115200)) {
		Serial.println("An error has occured whi9le mounting SPIFFS");
		return;
	}
	
	File fp = SPIFFS.open("test.txt", FILE_WRITE);
	
	if (!file) {
		Serial.println("There was an errror opening the file for writing");
		return;
	}
	
	if (file.print("TEST)) {
		Serial.println("File was written\n");
	}
	else {
		Serial.println("File write failed\n");
	}
	file.close();
}

void loop() {}