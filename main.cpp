#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "env.h"

// LCD setup (I2C address, 16 columns, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);
WiFiClientSecure client;

void connectToWiFi() {
    Serial.print("Connecting to WiFi...");
    if (IS_WOKWI) 
        WiFi.begin(SSID, PASS, CHANNEL);
    else 
        WiFi.begin(SSID, PASS);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected!");
}

void fetchMessage() {
    client.setInsecure(); // Ignore SSL cert validation
    if (!client.connect(ENDPOINT, 443)) {
        Serial.println("Connection to API failed.");
        return;
    }

    // Send GET request
    client.println("GET /message HTTP/1.1");
    client.println("Host: ecse3038-lab5-tester.onrender.com");
    client.print("api-key: ");
    client.println(API_KEY);
    client.println("Connection: close");
    client.println();

    // Read response
    String response = "";
    while (client.available()) {
        response += client.readString();
    }
    Serial.println("Response: " + response);

    // Parse JSON
    DynamicJsonDocument doc(256);
    DeserializationError error = deserializeJson(doc, response);
    if (error) {
        Serial.println("JSON Parsing Failed");
        return;
    }

    String line1 = doc["line_1"].as<String>();
    String line2 = doc["line_2"].as<String>();

    // Display on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(line1);
    lcd.setCursor(0, 1);
    lcd.print(line2);
}

void setup() {
    Serial.begin(115200);
    lcd.init();
    lcd.backlight();

    connectToWiFi();
    fetchMessage();
}

void loop() {
    // Fetch new message every 30 seconds
    delay(30000);
    fetchMessage();
}
