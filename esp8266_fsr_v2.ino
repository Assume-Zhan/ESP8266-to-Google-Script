#include <ArduinoJson.h>
#include <DebugMacros.h>
#include <HTTPSRedirect.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>

// Enter network credentials:
const char* ssid = "lgj";
const char* password = "lgjlgjlgj";
int count = 0;                   // count to decide if there's an obstacle staying there

// Pin
#define fsr_pin A0
#define led_pin D0

// Enter Google Script Deployment ID:
const char* GScriptId = "AKfycbw97lf137r2-nv95tq8fiPJKJrnLRdzOm1ddDntXt7T_8JjxO2Ltt3yHivLCiX9X73uWg";

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
int Number = 0;
int FSR_value = 0;
int stay = 0;

///////////////////////////////////////////////////////////////////////////////////////

void ParseData(String json){

  StaticJsonDocument<128> doc;

  DeserializationError error = deserializeJson(doc, json);

  if (error) {
    Serial.println(error.f_str());
    return;
  }

  String value_1 = doc["value_1"];

  Serial.println(value_1);

}

void GetData(){
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

  Serial.println("Publishing data from sheet...");
  Serial.println(payload);
//  if(client->GET(url + "?data=100", host)){
//  }
//  else{
//    Serial.println("Error while connecting with sheet");
//  }

   Serial.print("Data : ");
   
   Serial.println(client->GET(url + "?data=100", host));

   Serial.println(client->getResponseBody());
}


void setup(){
  Serial.begin(115200);
  pinMode(led_pin, OUTPUT);
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
  GetData();
  delete client;    // delete HTTPSRedirect object
  client = nullptr; // delete HTTPSRedirect object
}

//////////////////////////////////////////////////////////////////////////


void sendData(int fsr_value, int stay){

  Number++;

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
  payload = payload_base + "\"" + Number + "," + fsr_value + "," + stay + "\"}";

  // Publish data to Google Sheets
  Serial.println("Publishing data to sheet...");
  Serial.println(payload);
  if(client->POST(url, host, payload)){
  }
  else{
    Serial.println("Error while connecting");
  }
}

///////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////

void loop(){
  
  int fsr_value = analogRead(fsr_pin); // read FSR
  int led_value = 0;
  int Dlay = 500;
  if(fsr_value > 500){
    led_value = 1;
    digitalWrite(led_pin, HIGH);
    count++;
    stay = count*Dlay;
    if(count > 5){
      sendData(fsr_value, stay);
    }
  }  
  else{
    digitalWrite(led_pin, LOW);
    stay = 0;
    count = 0;
  }
  Serial.print("FSR value: ");
  Serial.println(fsr_value);
  Serial.print("LED status: ");
  Serial.println(led_value);
  Serial.print("time counting(ms): ");
  Serial.println(stay);
  Serial.println("-------------");
  delay(Dlay);
  
}
