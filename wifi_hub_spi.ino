#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <SPI.h>

const char* ssid = "Starlink_petcho";
const char* password = "13579000";
const char* supabase_url = "https://hbrujdruzrjaaqbpjucp.supabase.co/rest/v1/sensor_data";
const char* supabase_key = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImhicnVqZHJ1enJqYWFxYnBqdWNwIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NzMyNDQ3ODMsImV4cCI6MjA4ODgyMDc4M30.ypjUq1IL3BdtrUW7OGE2HbKbY3t6O1c7xGShJF873PY";

const int NUM_PLANTS = 5;
const int SS_PIN = D10; 

void setup() {
  Serial.begin(115200);
  
  // Initialize SPI Master
  pinMode(SS_PIN, OUTPUT);
  digitalWrite(SS_PIN, HIGH); 
  SPI.begin();
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); 

  WiFi.begin(ssid, password);
  Serial.println("\nConnecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    byte moistureValues[NUM_PLANTS];

    // --- 1. Fetch data from the Uno via SPI ---
    digitalWrite(SS_PIN, LOW); // Select the Uno
    delay(1); 

    // Generate clock pulses and read the responses
    for(int i = 0; i < NUM_PLANTS; i++) {
      moistureValues[i] = SPI.transfer(0x00); 
      delayMicroseconds(20); 
    }
    digitalWrite(SS_PIN, HIGH); // Deselect the Uno

    Serial.print("SPI Data Received: ");
    for(int i=0; i<NUM_PLANTS; i++) {
      Serial.print(moistureValues[i]); Serial.print(" ");
    }
    Serial.println();

    // --- 2. Upload to Supabase ---
    WiFiClientSecure client;
    client.setInsecure(); 
    HTTPClient http;
    
    if (http.begin(client, supabase_url)) {
      http.addHeader("apikey", supabase_key);
      http.addHeader("Authorization", String("Bearer ") + supabase_key);
      http.addHeader("Content-Type", "application/json");
      http.addHeader("Prefer", "resolution=merge-duplicates, return=representation"); 

      StaticJsonDocument<512> doc;
      JsonArray dataArray = doc.to<JsonArray>();

      for (int i = 0; i < NUM_PLANTS; i++) {
        JsonObject sensor = dataArray.createNestedObject();
        sensor["plant_id"] = i + 1; 
        sensor["moisture"] = moistureValues[i]; 
      }
      
      String requestBody;
      serializeJson(doc, requestBody);
      
      digitalWrite(LED_BUILTIN, LOW); 
      int httpResponseCode = http.POST(requestBody);
      
      if (httpResponseCode > 0) {
        Serial.printf("Supabase Upload Success! HTTP Code: %d\n", httpResponseCode);
      } else {
        Serial.printf("Supabase Upload Failed! Error: %d\n", httpResponseCode);
      }
      
      http.end();
      digitalWrite(LED_BUILTIN, HIGH); 
    }
  }

  delay(10000); 
}