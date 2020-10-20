#include "Arduino.h"
#include <EMailSender.h>
#include <ESP8266WiFi.h>

//update your SSID and SSID password below
    const char* ssid = "SSID";
    const char* password = "SSID_PASSWORD";

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

//The setup function is called once at startup of the sketch
void setup()
{
    Serial.begin(115200);
    connection_state = WiFiConnect(ssid, password);
    if(!connection_state)  // if not connected to WIFI
        Awaits();          // constantly trying to connect

    
    EMailSender::EMailMessage message;
    message.subject = "Caroline Dunn";
    message.message = "This is Caroline from YouTube.";
//change YOUR_GMAIL_ADDRESS below
    EMailSender::Response resp = emailSend.send("YOUR_GMAIL_ADDRESS", message);

    Serial.println("Sending status: ");

    Serial.println(resp.status);
    Serial.println(resp.code);
    Serial.println(resp.desc);
}

// The loop function is called in an endless loop
void loop()
{
//Add your repeated code here
}
