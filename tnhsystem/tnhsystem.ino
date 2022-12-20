#include <DHT.h>  // Including library for dht
#include <ESP8266WiFi.h>

String apiKey = "PJWE9YUUN6KYG4YO";     //  Enter your Write API key from ThingSpeak
 
const char* ssid = "yap";     // replace with your wifi ssid and wpa2 key
const char* pass = "12345678";
const char* server = "api.thingspeak.com";

#define DHTPIN 0
 
// Initialize DHT sensor.
DHT dht(DHTPIN, DHT22); 

 
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
      float tg = 7;
      float tgm = 42;
      float good = 100;
      float hg = 40; 
      float hgm = 70;
      float bad = 0;                 
      
              if (isnan(h) || isnan(t)) 
                 {
                     Serial.println("Failed to read from DHT sensor!");
                      return;
                 }
 
                         if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
                      {  
                            
                             String postStr = apiKey;
                             postStr +="&field1=";
                             postStr += String(t);                            
                             postStr +="&field2=";
                             postStr += String(h);
                             if ((tg = tg) && (hg =hg)){
                             postStr +="&field3=";
                             postStr += String(good);                               
                             }
                             else if ((tg <= tgm) && (hg <=hgm)){ 
                             postStr +="&field3=";    
                             postStr += String(good);                           
                             }
                             else if ((tg <= tgm) && (hg >=hgm)){ 
                             postStr +="&field3=";    
                             postStr += String(good);                           
                             }
                             else if ((tg >= tgm) && (hg >=hgm)){ 
                             postStr +="&field3=";    
                             postStr += String(good);                           
                             }
                             postStr += "\r\n\r\n\r\n\r\n";
                             
                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n\n\n");
                             client.print(postStr);
 
                             Serial.print("Temperature: ");
                             Serial.print(t);
                             Serial.print(" degrees Celcius, Humidity: ");
                             Serial.print(h);
                             Serial.println("%. Send to Thingspeak.");
                        }
          client.stop();
 
          Serial.println("Waiting...");
  
  // thingspeak needs minimum 15 sec delay between updates
  delay(1000);
}