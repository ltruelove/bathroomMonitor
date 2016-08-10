#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "...";
const char* password = "...";
const char* host = "ipsaveurl";
IPAddress ip(192, 168, 201, 5);
IPAddress gateway(192, 168, 201, 5);
IPAddress subnet(192, 168, 201, 5);

ESP8266WebServer server(80);

const int led = 13;
const int pin = 2;
unsigned long previousMillis = 0;
const long interval = 3000;

//volatile int state = false;
//volatile int flag = false;
const char* door_state = "closed";
const char* message = "{\"doorStatus\" : \"";

void handleRoot() {
  getDoorStatus();
  digitalWrite(led, 1);
  String response = String(message);
  response.concat(door_state);
  response.concat("\"}");
  
  server.send(200, "text/plain", response);
  digitalWrite(led, 0);
}

void handleNotFound(){
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void){
  pinMode(pin, INPUT);
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);

  Serial.begin(115200);
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  String url = String(host);
  String ip = WiFi.localIP().toString();
  url += ip;
  Serial.println(ip);

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
    //attachInterrupt(digitalPinToInterrupt(pin), changeDoorStatus, CHANGE);

  WiFiClient wClient;
  char buf[128];
  url.toCharArray(buf, 128);
  wClient.connect(buf, 80);
    
  server.on("/", handleRoot);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}
void getDoorStatus(){
  int switchVal = digitalRead(pin);

  if(switchVal == HIGH){
    door_state = "closed";
  }else{
    door_state = "opened";
  }
}
/*
void changeDoorStatus() {

    unsigned long currentMillis = millis();
 
    if(currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;   
    
        state = !state;
        if(state) {
            door_state = "opened";
        }
        else{
            door_state = "closed";
        }
        flag = true;
        Serial.println(state);
        Serial.println(door_state);
    }
    
}
*/
