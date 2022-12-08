#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <esp_now.h>

uint8_t serverAddress[] = {0xC4, 0xDD, 0x57, 0xC9, 0x8C, 0xB4};


const char* ssid = "Galaxy S6 active 7656";
const char* password = "vypu5262";

WebServer server(80);   //instantiate server at port 80 (http port)

String page = "";
String text = "";

float red_score;
float blue_score;

typedef struct struct_message {
    float red;
    float blue;
    float seq;
} struct_message;

struct_message myData;

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  red_score = myData.red;
  blue_score = myData.blue;
}
   

void setup(void) {
  Serial.begin(115200);
  Serial.println(WiFi.macAddress());
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.on("/red.txt", []() {
    text = (String)red_score;
    server.send(200, "text/html", text);
  });
   server.on("/blue.txt", []() {
    text = (String)blue_score;
    server.send(200, "text/html", text);
  });
  server.on("/", []() {
    page = "<body align=center style=\"background-color:#F9E79F;\"><p style=\"font-size:70px; color:red;\"align=center>Team 34 Scoreboard</p>\r\n";
    page += "<h1 align=center>Red Team Score: <span id=\"red\"></span></h1>\r\n";
    page += "<h1 align=center>Blue Team Score: <span id=\"blue\"></span></h1>\r\n";
    page += "<script>\r\n";
    page += "var x = setInterval(function() {loadData(\"red.txt\",updateData)}, 1000);\r\n";
    page += "var y = setInterval(function() {loadData(\"blue.txt\",updateData1)}, 1000);\r\n";
    page += "function loadData(url, callback){\r\n";
    page += "var xhttp = new XMLHttpRequest();\r\n";
    page += "xhttp.onreadystatechange = function(){\r\n";
    page += " if(this.readyState == 4 && this.status == 200){\r\n";
    page += " callback.apply(xhttp);\r\n";
    page += " }\r\n";
    page += "};\r\n";
    page += "xhttp.open(\"GET\", url, true);\r\n";
    page += "xhttp.send();\r\n";
    page += "}\r\n";
    page += "function updateData(){\r\n";
    page += " document.getElementById(\"red\").innerHTML = this.responseText;\r\n";
    page += "}\r\n";
    page += "function updateData1(){\r\n";
    page += " document.getElementById(\"blue\").innerHTML = this.responseText;\r\n";
    page += "}\r\n";
    page += "</script><body>\r\n";
    server.send(200, "text/html", page);
  });

  server.begin();
}

void loop(void) {
 

  delay(3000);
  red_score += 1;
  blue_score += 1;
  server.handleClient();
}