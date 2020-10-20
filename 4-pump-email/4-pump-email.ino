
#include "Arduino.h"
#include <EMailSender.h>
#include <ESP8266WiFi.h>

int sensor_pin = A0; 
int output_value ;
const int pump = 5;      // digital pin where the relay is plugged in
const int threshold = 34;  //threshold value to trigger pump

// Replace with your network credentials
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

void setup() {
  Serial.begin(115200);
  
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
  //watering code
    if (output_value < threshold)  //if the soil is try then turn on pump
    {
    digitalWrite(pump, LOW);
    Serial.println("pump on");
    delay(1000);  //run pump for 1 second;
    digitalWrite(pump, HIGH);
    Serial.println("pump off");
    delay(2000);//wait 2 seconds 
   //send email
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
    }
    else
    {
    digitalWrite(pump, HIGH);
    Serial.println("do not turn on pump");
    //delay(300000); //wait 5 minutes
    delay(1000);// wait 1 second. This is for testing, uncomment the line above when implementing
    }
    //delay(2000);//ENABLE FOR testing
    delay(3600000); //wait 1 hour
  }
