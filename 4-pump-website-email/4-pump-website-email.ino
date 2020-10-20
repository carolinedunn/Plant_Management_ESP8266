
#include "Arduino.h"
#ifdef ESP32
  #include <WiFi.h>
  #include <HTTPClient.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
  #include <WiFiClient.h>
#endif

#include <EMailSender.h>
#include <Wire.h>

int sensor_pin = A0; 
int output_value ;
const int pump = 5;      // digital pin where the relay is plugged in
const int threshold = 34;  //threshold value to trigger pump

// Replace with your network credentials
const char* ssid = "SSID";
const char* password = "SSID_PASSWORD";

// REPLACE with your Domain name and URL path or IP address with path
const char* serverName = "http://YOUR_WEBSITE/post-data.php";

// Keep this API Key value to be compatible with the PHP code provided in the project page. 
// If you change the apiKeyValue value, the PHP file /post-data.php also needs to have the same key 
String apiKeyValue = "tPmAT5Ab3j7F9";

    uint8_t connection_state = 0;
    uint16_t reconnect_interval = 10000;
// replace with your gmail and password as in video 2
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
  pinMode(sensor_pin, INPUT);  //setup for the soil moisture senor aka INPUt
  pinMode(pump, OUTPUT);      //setup for the pump aka OUTPUT
  connection_state = WiFiConnect(ssid, password);
    if(!connection_state)  // if not connected to WIFI
        Awaits();          // constantly trying to connect
  Serial.println("Initialize pump to off state.");
  delay(20);
  //initialize pump
  digitalWrite(pump, HIGH);
  delay(10);
  }
  

void loop() {
    //Check WiFi connection status
  if(WiFi.status()== WL_CONNECTED){
    HTTPClient http;
    
    // Your Domain name with URL path or IP address with path
    http.begin(serverName);
    
    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    //get analog output

  output_value= analogRead(sensor_pin);
  output_value = map(output_value,550,0,0,100);
  Serial.print("Moisture : ");
  Serial.print(output_value);
  Serial.println("%");
  
  // Prepare your HTTP POST request data
  String httpRequestData = "api_key=" + apiKeyValue + "&value1=" + String(output_value) + "";
  Serial.print("httpRequestData: ");
  Serial.println(httpRequestData);

  // Send HTTP POST request
  int httpResponseCode = http.POST(httpRequestData);
   
   if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      }
      else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
      }

     //watering code
    if (output_value < threshold)  //if the soil is try then turn on pump
    {
    digitalWrite(pump, LOW);
    Serial.println("pump on");
    delay(1000);  //run pump for 1 second;
    digitalWrite(pump, HIGH);
    Serial.println("pump off");
    delay(2000);//wait 2 secondS.
      //send email code
    EMailSender::EMailMessage message;
    String start = "Moisture Reading = ";
    String end = "%";
    message.subject = "Watering Active";
    String text4message = start + output_value + end;
    Serial.println(text4message);
    message.message = (text4message);
    // replace with your gmail address
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
  

  // Free resources
  http.end();
  }   else {
    Serial.println("WiFi Disconnected");
  }
  //delay(2000);//ENABLE FOR testing
  delay(1800000); //wait 30 minutes before checking again
}
