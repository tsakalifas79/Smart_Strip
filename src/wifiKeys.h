// #include "definitions.h"
#define BLYNK_PRINT Serial
// #include <ESP8266WiFi.h>
// #include <BlynkSimpleEsp8266.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// Change YOUR_AP_NAME and YOUR_AP_PASSWORD to your WiFi credentials
char* ssids[] = {"Sevah","CYTA98B7","paparious"}; //list a necessary wifi networks
char* passs[] = {"firiolas","ZTE1G8FJ4701956","saviola"}; //list a passwords

//const char auth[] = "uWISn6Ym8MKlMOlpy0TNUFxY45LR5T4_";//smarthomeAPP
// const char auth[] = "A0cdrRtCCKkplHPsv9esHuKyIYpXG5Dz";//esp32 smartGarden
//const char auth[] = "plmvG3mYJsWWjId5wEqB5GjubVEOXPZ3";//esp8266 smartGarden
// const char auth[] = "db0676ac7b2a475787a2ad353191c5de";// esp32 ledstrip
const char auth[] = "lpJGnOpGVnFRk8hdKd_0qFZax1FPLiFr";// esp32 ledstrip


  int ssid_count=0;
  int ssid_mas_size = 0;

uint8_t ap_name[32];
uint8_t passw[32];


void MultyWiFiBlynkBegin() {
  ssid_count=0;
  ssid_mas_size = sizeof(ssids) / sizeof(ssids[0]);
    
  do {
    Serial.println("\nTrying to connect to wi-fi " + String(ssids[ssid_count]));


    WiFi.begin(ssids[ssid_count], passs[ssid_count]);  

    int WiFi_timeout_count=0;
    while (WiFi.status() != WL_CONNECTED && WiFi_timeout_count<50) { //waiting 10 sec
      delay(200);
      Serial.print(".");
      ++WiFi_timeout_count;
    }
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Connected to WiFi! Now I will check the connection to the Blynk server");
      Blynk.config(auth);
      Blynk.connect(5000); //waiting 5 sec
    }
    ++ssid_count; 
  }
  while (!Blynk.connected() && ssid_count<ssid_mas_size);
  if (!Blynk.connected() && ssid_count==ssid_mas_size) {
    Serial.println("I could not connect to blynk =( Ignore and move on. but still I will try to connect to wi-fi " + String(ssids[ssid_count-1]));
  }
}