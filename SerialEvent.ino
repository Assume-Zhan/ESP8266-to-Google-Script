#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "HTTPSRedirect.h"

// Enter network credentials:
const char* ssid = "AndroidAP";
const char* password = "0966480117";
int buttonState = 0;             // the current reading from the input pin
int lastButtonState = 0;   // the previous reading from the input pin
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers


// Pin
const int buttonPin = 4;

// Enter Google Script Deployment ID:
const char* GScriptId = "AKfycbxHTNNtq3yzmP0GCJjQ9-g9JIGTJ3wn2FIA4xEkqflUpJeHHpN_lNB-TcyRYJcQaA_0";

// Enter command (insert_row or append_row) and your Google Sheets sheet name (default is Sheet1):
String payload_base = "{\"command\": \"insert_row\", \"sheet_name\": \"Sheet1\", \"values\": ";
String payload = "";

// Google Sheets setup (do not edit)
const char* host = "script.google.com";
const int httpsPort = 443;
const char* fingerprint = "";
String url = String("/macros/s/") + GScriptId + "/exec";
HTTPSRedirect* client = nullptr;

// Declare variables that will be published to Google Sheets
int value0 = 0;
int value1 = 0;
int value2 = 0;

void setup(){

  Serial.begin(115200);
  delay(10);
  Serial.println('\n');

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  while(WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.print(".");
  }
  Serial.println('\n');
  Serial.println("Connection established!");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());

  // Use HTTPSRedirect class to create a new TLS connection
  client = new HTTPSRedirect(httpsPort);
  client->setInsecure();
  client->setPrintResponseBody(true);
  client->setContentTypeHeader("application/json");

  Serial.print("Connecting to ");
  Serial.println(host);

  // Try to connect for a maximum of 5 times
  bool flag = false;
  for(int i = 0; i < 5; i++){
    int retval = client->connect(host, httpsPort);
    if(retval == 1){
      flag = true;
      Serial.println("Connected");
      break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }
  if(!flag){
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    return;
  }
  delete client;    // delete HTTPSRedirect object
  client = nullptr; // delete HTTPSRedirect object
}

void sendData(){

  value0++;
  value1 = random(0, 1000);
  value2 = random(0, 100000);

  static bool flag = false;
  if(!flag){
    client = new HTTPSRedirect(httpsPort);
    client->setInsecure();
    flag = true;
    client->setPrintResponseBody(true);
    client->setContentTypeHeader("application/json");
  }
  if(client != nullptr){
    if(!client->connected()){
      client->connect(host, httpsPort);
    }
  }
  else{
    Serial.println("Error creating client object!");
  }

  // Create json object string to send to Google Sheets
  payload = payload_base + "\"" + value0 + "," + value1 + "," + value2 + "\"}";

  // Publish data to Google Sheets
  Serial.println("Publishing data...");
  Serial.println(payload);
  if(client->POST(url, host, payload)){

  }
  else{

    Serial.println("Error while connecting");
  }
}

void loop(){

  // Read the button pin
  int reading = digitalRead(buttonPin);
  // Serial.println(reading);


  // if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
  if(reading != buttonState){
    buttonState = reading;

    // only toggle the LED if the new button state is HIGH
    if(buttonState == 1){
      Serial.println("Trigger the button");
      sendData();
    }
  }
// }

// save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
  delay(30);
}