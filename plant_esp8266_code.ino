#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

// --- WiFi Credentials ---
const char* ssid = "Starlink_petcho";
const char* password = "13579000";

// --- Supabase Credentials ---
// Note: Add /rest/v1/your_table_name to the end of your project URL
const char* supabase_url = "https://hbrujdruzrjaaqbpjucp.supabase.co/rest/v1/sensor_data";
const char* supabase_key = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImhicnVqZHJ1enJqYWFxYnBqdWNwIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NzMyNDQ3ODMsImV4cCI6MjA4ODgyMDc4M30.ypjUq1IL3BdtrUW7OGE2HbKbY3t6O1c7xGShJF873PY";

void setup() {
  Serial.begin(115200);
  Serial.println();

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    
    // Supabase requires HTTPS, so we use WiFiClientSecure
    WiFiClientSecure client;
    
    // For testing purposes, we bypass SSL certificate validation. 
    // In a strict production environment, you'd provide the root CA cert.
    client.setInsecure(); 

    HTTPClient http;
    
    // Initialize the HTTP client with the Supabase URL
    if (http.begin(client, supabase_url)) {
      
      // Add required Supabase headers
      http.addHeader("apikey", supabase_key);
      http.addHeader("Authorization", String("Bearer ") + supabase_key);
      http.addHeader("Content-Type", "application/json");
      http.addHeader("Prefer", "return=representation"); // Returns the inserted data
      // Create a JSON document using ArduinoJson
      StaticJsonDocument<200> doc;
      doc["plant_id"] = 2; // Example data
      doc["moisture"] = 50.4;    // Example data
      
      String requestBody;
      serializeJson(doc, requestBody);

      Serial.println("Sending data to Supabase...");
      
      // Send the HTTP POST request (Use POST to Insert, PATCH to Update)
      int httpResponseCode = http.POST(requestBody);

      if (httpResponseCode > 0) {
        Serial.printf("HTTP Response code: %d\n", httpResponseCode);
        String response = http.getString();
        Serial.println("Response payload: " + response);
      } else {
        Serial.printf("Error code: %d\n", httpResponseCode);
        Serial.println(http.errorToString(httpResponseCode).c_str());
      }
      
      // Free resources
      http.end();
    } else {
      Serial.println("Unable to connect to Supabase URL");
    }
  } else {
    Serial.println("WiFi Disconnected");
  }

  // Wait 10 seconds before sending the next update
  delay(10000); 
}