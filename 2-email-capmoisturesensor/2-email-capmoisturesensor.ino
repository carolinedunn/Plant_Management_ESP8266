
#include "Arduino.h"
#include <EMailSender.h>
#include <ESP8266WiFi.h>

    uint8_t connection_state = 0;
    uint16_t reconnect_interval = 10000;
    
    // update YOUR_GMAIL_ADDRESS and YOUR_PASSWORD below
    EMailSender emailSend("YOUR_GMAIL_ADDRESS", "YOUR_PASSWORD");

uint8_t WiFiConnect(const char* nSSID = nullptr, const char* nPassword = nullptr)
{
    static uint16_t attempt = 0;
    Serial.print("Connecting to ");
    if(nSSID) {
        WiFi.begin(nSSID, nPassword);
        Serial.println(nSSID);
    }

    uint8_t i = 0;
    while(WiFi.status()!= WL_CONNECTED && i++ < 50)
    {
        delay(200);
        Serial.print(".");
    }
    ++attempt;
    Serial.println("");
    if(i == 51) {
        Serial.print("Connection: TIMEOUT on attempt: ");
        Serial.println(attempt);
        if(attempt % 2 == 0)
            Serial.println("Check if access point available or SSID and Password\r\n");
        return false;
    }
    Serial.println("Connection: ESTABLISHED");
    Serial.print("Got IP address: ");
    Serial.println(WiFi.localIP());
    return true;
}

void Awaits()
{
    uint32_t ts = millis();
    while(!connection_state)
    {
        delay(50);
        if(millis() > (ts + reconnect_interval) && !connection_state){
            connection_state = WiFiConnect();
            ts = millis();
        }
    }
}
int sensor_pin = A0; 
int output_value ;


void setup() {
  Serial.begin(115200);
  
  //update your SSID and SSID password below
  const char* ssid = "SSID";
    const char* password = "SSID_PASSWORD";
  connection_state = WiFiConnect(ssid, password);
    if(!connection_state)  // if not connected to WIFI
        Awaits();          // constantly trying to connect
  Serial.println("Reading From the Sensor ...");
  delay(2000);
  }

void loop() {

  output_value= analogRead(sensor_pin);
  output_value = map(output_value,550,0,0,100);
  Serial.print("Moisture : ");
  Serial.print(output_value);
  Serial.println("%");
  EMailSender::EMailMessage message;
  String start = "Moisture Reading = ";
  String end = "%";
  message.subject = "Moisture Reading";
  String text4message = start + output_value + end;
  Serial.println(text4message);
  message.message = (text4message);
  
  //change YOUR_GMAIL_ADDRESS below
  EMailSender::Response resp = emailSend.send("YOUR_GMAIL_ADDRESS", message);
  Serial.println("Sending status: ");
  Serial.println(resp.status);
  Serial.println(resp.code);
  Serial.println(resp.desc);
  delay(3600000); //wait 1 hour
  }
