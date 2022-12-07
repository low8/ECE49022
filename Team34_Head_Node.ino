#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include "HX711.h"
#include "Timer.h"
#include <SPI.h>//https://www.arduino.cc/en/reference/SPI
#include <MFRC522.h>//https://github.com/miguelbalboa/rfid
#define SS_PIN 5
#define RST_PIN 22


// Communication Variables

uint8_t broadcastAddress[] = {0xC4, 0xDD, 0x57, 0xC9, 0x8C, 0xB4};

float red_score;
float blue_score;
float seq;
float incomingRed;
float incomingBlue;
float incomingSeq;

typedef struct struct_message {
    float red;
    float blue;
    float seq;
} struct_message;

struct_message Cur;
struct_message incomingReadings;
String success;

esp_now_peer_info_t peerInfo;

// Scoring Variables

byte nuidPICC[4] = {0, 0, 0, 0};
MFRC522::MIFARE_Key key;
MFRC522 rfid = MFRC522(SS_PIN, RST_PIN);
HX711 scale;

uint8_t dataPin = 13;
uint8_t clockPin = 33;
float w1, w2, previous = 0;
int numInt = 0;
float numDec = 0;
int bags = 0;
int redPt = 0;
int bluePt = 0;

Timer timer; 

char number[] = "0000"; 
char roundScore[] = "0000";
int bagPts = 0;
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
int cPin = 2;
int dPin = 0;

const int button = 16;
int buttonState = 0;

int count = 0;
int numbers[4] ;
int anodePins[] = {1, 12, 14, 15};

byte table[10] {B11111100, B01100000, B11011010, B11110010, B01100110, B10110110, B10111110, B11100000, B11111110, B11110110};
byte redWin[10] {B0001010, B10011110, B01111010, B00000000};
byte blueWin[10] {B00111110, B00011100, B00111000, B10011110};

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  Serial.print("Bytes received: ");
  Serial.println(len);
  incomingRed = incomingReadings.red;
  incomingBlue = incomingReadings.blue;
  incomingSeq = incomingReadings.seq;
}

void sendAndRecieve_setup() {
  WiFi.mode(WIFI_STA);

  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
       
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
}

void sendAndRecieve_loop() {
  getReadings();

  Cur.red = red_score;
  Cur.blue = blue_score;
  Cur.seq = seq;

  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &Cur, sizeof(Cur));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  updateDisplay();
  delay(10000);

}

void getReadings(){
  red_score -= 26;
  blue_score -= 26;
  seq += 1;
}

void updateDisplay(){

  if (incomingSeq > seq) {
    red_score = incomingRed;
    blue_score = incomingBlue;
  }

  Serial.println("INCOMING READINGS");
  Serial.print("red_score: ");
  Serial.print(incomingRed);
  Serial.println();
  Serial.print("blue_score: ");
  Serial.print(incomingBlue);
  Serial.println();
  Serial.print("seq: ");
  Serial.print(incomingSeq);
  Serial.println();
}

//separates the digits of the score for the displays
void separate(char *num) { 
  num1 = int(num[0]) - '0';
  numbers[0] = num1;
  num2 = int(num[1]) - '0';
  numbers[1] = num2;
  num3 = int(num[2]) - '0';
  numbers[2] = num3;
  num4 = int(num[3]) - '0';
  numbers[3] = num4;
}

//displays the score to the 7-seg
void Display() {
  screenOff();
  digitalWrite(latchPin, LOW); 
  shiftOut(dPin, cPin, LSBFIRST, table[numbers[count]]); 
  digitalWrite(anodePins[count], HIGH);
  digitalWrite(latchPin, HIGH); 
  delay(3);
  count++; 
  if (count == 4) { 
    count = 0;
  }
}

//turns the screen off
void screenOff() { 
  digitalWrite(D4, LOW);
  digitalWrite(D3, LOW);
  digitalWrite(D2, LOW);
  digitalWrite(D1, LOW);
}

//Displays winner color name
void DisplayWinner() {
  screenOff();
  digitalWrite(latchPin, LOW); 
  if ((numbers[2] * 10 + numbers[3]) >= 21)
  {
    shiftOut(dPin, cPin, LSBFIRST, redWin[count]); 
  }
  else
  {
    shiftOut(dPin, cPin, LSBFIRST, blueWin[count]); 
  }
  digitalWrite(anodePins[count], HIGH);
  digitalWrite(latchPin, HIGH); 
  delay(3);
  count++; 
  if (count == 4) { 
    count = 0;
  }
}


void readRFID(void ) { /* function readRFID */
  ////Read RFID card

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  // Look for new 1 cards
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if (  !rfid.PICC_ReadCardSerial())
    return;

  // Store NUID into nuidPICC array
  for (byte i = 0; i < 4; i++) {
    nuidPICC[i] = rfid.uid.uidByte[i];
  }

  printDec(rfid.uid.uidByte, rfid.uid.size); //detect which team the tag belongs to
  Serial.println();

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();

}

//This originally would print the buffer to the serial monitor, but now it checks which team's tag is detected.
void printDec(byte *buffer, byte bufferSize) {
  if (int((unsigned char)(buffer[0])) == 70) //would normally check all of tag ID, but since we can only use 2 for demo, we'll only check between the two tags.
  {
    redPt++;//red
  }
  else
  {
    bluePt++; //blue
  }
}

void scoring_loop() {
    if (((numbers[0] * 10 + numbers[1]) < 21) && ((numbers[2] * 10 + numbers[3]) < 21))
  {
    //set point values to zero for start of each round of play. These are the points to be added to the total scores
    redPt = 0;
    bluePt = 0;
    //display current scores and check if next turn button pressed.
    timer.update(); 
    timer.stop(timer_event); 
    screenOff(); 
    buttonState = digitalRead(button);
    separate(number);
    delay(1);
    Display();

    //If next round button pressed
    if (buttonState == HIGH) {
      //Check weight sensor values
      screenOff();
      delay(5);
      w1 = scale.get_units(10);

      //determines number of bags
      numInt = int(w1 / 2);
      numDec = (w1 / 2) - numInt;
      if ((w1 / 2) < 0.5)
      {
        bags = 0;
      }
      else if((numDec >= 0) && (numDec <= 0.5))
      {
        bags = numInt;
      }
      else//check num of bags in hole
      {
        bags = numInt + 1;
      }
      
      readRFID(); //this updates points for RFID as well. For note when testing in person, you'll have to move tag away and put bag for each turn check.
      //since we can't detect difference on the bags in hole with RFID, any additional bags over 4 are counted as blue.
      if (bags > 4)
      {
        redPt = 12 + redPt;
        bluePt = ((bags - 4) * 3) + bluePt;
      }
      else
      {
        redPt = (bags * 3) + redPt;
      }

      //determine each team's score for the round
      if (redPt >= bluePt)
      {
        redPt = redPt - bluePt;
        bluePt = 0;
      }
      else
      {
        bluePt = bluePt - redPt;
        redPt = 0;
      }

      //put point values in appropriate spots with new scores
      bagPts = ((int(roundScore[2])- '0') * 10) + (int(roundScore[3])- '0') + redPt;
      roundScore[2] = (bagPts / 10) + '0'; //adds hole points to red team since we cannot currently check bag color in hole. Might add that 1st 4 bas are red, any additional are blue
      roundScore[3] = (bagPts % 10) + '0';
      bagPts = ((int(roundScore[0])- '0') * 10) + (int(roundScore[1])- '0') + bluePt;
      roundScore[0] = (bagPts / 10) + '0'; //adds hole points to red team since we cannot currently check bag color in hole. Might add that 1st 4 bas are red, any additional are blue
      roundScore[1] = (bagPts % 10) + '0';
      separate(roundScore);

      //put new score in total score variable
      for (int i = 0; i < 4; i++)
      {
        number[i] = roundScore[i];
      }
    }
  }
  else //if win condition is met
  {
    //display winner
    if ((numbers[2] * 10 + numbers[3]) >= 21)
    {
      DisplayWinner(); //red wins
    }
    else
    {
      DisplayWinner(); //blue wins
    }
  }
}

void scoring_setup() {
  scale.begin(dataPin, clockPin);

  // load cell factor 20 KG
  // scale.set_scale(127.15);
  // load cell factor 5 KG
  scale.set_scale(-8000);       // TODO you need to calibrate this yourself.
  scale.tare();

  //button and displays
  pinMode(D4, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(cPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(dPin, OUTPUT);
  pinMode(button, INPUT);
  digitalWrite(D4, LOW);
  digitalWrite(D3, LOW);
  digitalWrite(D2, LOW);
  digitalWrite(D1, LOW);

  //RFID
  //init rfid D8,D5,D6,D7
  SPI.begin();
  rfid.PCD_Init();
}
 
void setup() {
  Serial.begin(115200);
  sendAndRecieve_setup();
}
 
void loop() {
  sendAndRecieve_loop();
  Serial.println("EXIT LOOP");
}

