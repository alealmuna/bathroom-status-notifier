#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

//#define ENDPOINT_ADDRESS "http://maker.ifttt.com/trigger/magnet_bathroom_status/with/key/dROe3LFhK_oMPEmEpvujQ2"
//#define open_photo_url "http://i.imgur.com/8zmd9wR.jpg"
//#define close_photo_url "http://i.imgur.com/WnONWb1.jpg"

#define ENDPOINT_ADDRESS "ENDPOINT_ADDRESS"
#define API_TOKEN "API_TOKEN"

#define SSID "SSID"
#define PASSWORD "PASSWORD"

const byte unavailableLedPin = D1;
const byte availableLedPin = D2;
const byte interruptPin = D4;
const byte confirmPin = D3;

volatile byte state = LOW;
volatile bool toggle = false;


void setup() {
  
  Serial.begin(57600);  
  pinMode(availableLedPin, OUTPUT);
  pinMode(unavailableLedPin, OUTPUT);
  pinMode(interruptPin, INPUT_PULLUP);
  pinMode(confirmPin, INPUT_PULLUP);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  
  while(!Serial){}
  
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, CHANGE);
}

void loop() {
  if(toggle){
    if(digitalRead(confirmPin)){
      toggle = false;
      Serial.println("Toggle without confirm pin");
      return;
    }
    detachInterrupt(digitalPinToInterrupt(interruptPin));
    String payload = "{\"value\":\"" + String(state) + "\"}";
    /*String payload = "";
    if(state){
      payload = "value1=1&value2=Baño ocupado&value3=" + String(close_photo_url);
    }else{
      payload = "value1=0&value2=Baño desocupado&value3=" + String(open_photo_url);
    }*/
    Serial.println(payload);
    sendData(payload);
    toggle = false;
    digitalWrite(unavailableLedPin, state);
    digitalWrite(availableLedPin, !state);
    attachInterrupt(digitalPinToInterrupt(interruptPin), blink, CHANGE);
  }
}

void blink() {
  state = digitalRead(interruptPin);
  toggle = true;
}

int sendData(String payload) {
  HTTPClient http;
  String url = "json";
  http.begin(ENDPOINT_ADDRESS);
  //http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Token token=" + String(API_TOKEN) + ")");
  int httpCode = http.POST(payload);
  if(httpCode > 0) {
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      if(httpCode == HTTP_CODE_OK) {
      String response = http.getString();
        Serial.println(response);
      }
  } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

