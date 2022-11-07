#include <esp_now.h>
#include <WiFi.h>

// Must be changed to MAC address of other microcontroller
uint8_t broadcastAddress[] = {0xC4, 0xDD, 0x57, 0xCA, 0x4E, 0xE8}; // SENDING TO THIS ADDRESS
// This ESP32's MAC address
// C4:DD:57:C9:8C:B4


float distance;
float red_score;
float blue_score;
int seq = 0;
float incDistance;
float incRed_score;
float incBlue_score;
int incSeq;
String success;

typedef struct packet {
  float dist;
  float red;
  float blue;
  int s;
} packet;

packet send_readings;
packet recv_readings;
esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success";
  }
  else{
    success = "Delivery Fail";
  }
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&recv_readings, incomingData, sizeof(recv_readings));
  Serial.print("Recieved packet size (Bytes): ");
  Serial.println(len);
  incDistance = recv_readings.dist;
  incRed_score = recv_readings.red;
  incBlue_score = recv_readings.blue;
  incSeq = recv_readings.s;
}
 
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
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
 
void loop() {
  getReadings();
  send_readings.dist = distance;
  send_readings.red = red_score;
  send_readings.blue = blue_score;
  send_readings.s = seq;
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &send_readings, sizeof(send_readings));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
    Serial.println("This device's MAC address: ");
    Serial.println(WiFi.macAddress());
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(10000);
  updateDisplay();
}
void getReadings(){
  // TEST VALUES
  distance = 1;
  red_score = 2;
  blue_score = 3;
  seq += 1;
}

void updateDisplay(){
  Serial.println("Incoming Data Packet");
  Serial.print("Distance: ");
  Serial.print(incDistance);
  Serial.println(" Ft");
  Serial.print("Red Score: ");
  Serial.print(incRed_score);
  Serial.println(" pts");
  Serial.print("Blue Score: ");
  Serial.print(incBlue_score);
  Serial.println(" pts");
  Serial.println();
}