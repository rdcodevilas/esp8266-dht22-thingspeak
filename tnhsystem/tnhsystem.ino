#include <DHT.h>  // Including library for dht
#include <ESP8266WiFi.h>

String apiKey = "PJWE9YUUN6KYG4YO";     //  Enter your Write API key from ThingSpeak
 
const char* ssid = "yap";     // replace with your wifi ssid and wpa2 key
const char* pass = "12345678";
const char* server = "api.thingspeak.com";

#define DHTPIN 0
 
// Initialize DHT sensor.
DHT dht(DHTPIN, DHT22); 

#define USE_NODE_MCU_BOARD
float voltage;
int analogInPin  = A0;    // Analog input pin
float sensorValue;          // Analog Output of Sensor
float calibration = 0.40; // Check Battery voltage using multimeter & add/subtract the value
float bat_percentage;

 
WiFiClient client;
 
void setup() 
{
       Serial.begin(115200);
       delay(10);
       dht.begin();
 
       Serial.println("Connecting to ");
       Serial.println(ssid);
 
 
       WiFi.begin(ssid, pass);
 
      while (WiFi.status() != WL_CONNECTED) 
     {
            delay(500);
            Serial.print(".");
     }
      Serial.println("");
      Serial.println("WiFi connected");
 
}
 
void loop() 
{
  
      float h = dht.readHumidity();
      float t = dht.readTemperature();             
      
              if (isnan(h) || isnan(t)) 
                 {
                     Serial.println("Failed to read from DHT sensor!");
                      return;
                 }
 
                         if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
                          {  
                          sensorValue = analogRead(analogInPin);
                          float voltage = (((sensorValue * 3.3) / 1024) * 2 + calibration); //multiply by two as voltage divider network is 100K & 100K Resistor
                        
                          bat_percentage = mapfloat(voltage, 2.8, 4.2, 0, 100); //2.8V as Battery Cut off Voltage & 4.2V as Maximum Voltage
                        
                          if (bat_percentage >= 100)
                          {
                            bat_percentage = 100;
                          }
                          if (bat_percentage <= 0)
                          {
                            bat_percentage = 1;
                          }

                             String postStr = apiKey;
                             postStr +="&field1=";
                             postStr += String(t);                            
                             postStr +="&field2=";
                             postStr += String(h);
                             postStr +="&field3=";
                             postStr += String(voltage);
                             postStr +="&field4=";
                             postStr += String(bat_percentage);

                             postStr += "\r\n\r\n";
                             
                              client.print("POST /update HTTP/1.1\n");
                              delay(100);
                              client.print("Host: api.thingspeak.com\n");
                              delay(100);
                              client.print("Connection: close\n");
                              delay(100);
                              client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
                              delay(100);
                              client.print("Content-Type: application/x-www-form-urlencoded\n");
                              delay(100);
                              client.print("Content-Length: ");
                              delay(100);
                              client.print(postStr.length());
                              delay(100);
                              client.print("\n\n");
                              delay(100);
                              client.print(postStr);
                              delay(100);
 
                               //Print data on serial monitor
                              Serial.print("Temperature: ");
                              Serial.print(t);
                              Serial.println(" *C");

                              Serial.print("Humidity: ");
                              Serial.print(h);
                              Serial.println(" %");

                              Serial.print("Analog Value = ");
                              Serial.println(sensorValue);
                              Serial.print("Output Voltage = ");
                              Serial.println(voltage);
                              Serial.print("Battery Percentage = ");
                              Serial.println(bat_percentage);

                              Serial.println("%. Send to Thingspeak.");

                              Serial.println();
                              Serial.println("****************************");
                              Serial.println();
                              delay(1000);
                              }
                              
                              client.stop();
                              Serial.println("Sending...");
                              // thingspeak needs minimum 15 sec delay between updates
                              delay(15000);
                              
                              }
                                        float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
                              {
                                return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
                              }