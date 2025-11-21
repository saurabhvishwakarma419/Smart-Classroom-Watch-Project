#include "communication.h"
#include "config.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

bool wifiConnected = false;

bool initWiFi() {
    Serial.println("Initializing WiFi...");
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    return true;
}

bool connectWiFi() {
    Serial.print("Connecting to WiFi: ");
    Serial.println(WIFI_SSID);
    
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - startTime) < WIFI_TIMEOUT) {
        delay(500);
        Serial.print(".");
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi connected!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        Serial.print("Signal Strength: ");
        Serial.print(WiFi.RSSI());
        Serial.println(" dBm");
        wifiConnected = true;
        return true;
    } else {
        Serial.println("\nWiFi connection failed!");
        wifiConnected = false;
        return false;
    }
}

void disconnectWiFi() {
    WiFi.disconnect();
    wifiConnected = false;
    Serial.println("WiFi disconnected");
}

bool isWiFiConnected() {
    return (WiFi.status() == WL_CONNECTED);
}

String getDeviceMacAddress() {
    return WiFi.macAddress();
}

CommStatus sendHTTPPost(const String &endpoint, const String &jsonData) {
    if (!isWiFiConnected()) {
        return COMM_WIFI_ERROR;
    }
    
    HTTPClient http;
    String url = String(API_SERVER) + endpoint;
    
    Serial.print("POST to: ");
    Serial.println(url);
    
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    
    int httpResponseCode = http.POST(jsonData);
    
    if (httpResponseCode > 0) {
        Serial.print("HTTP Response: ");
        Serial.println(httpResponseCode);
        
        if (httpResponseCode == 200 || httpResponseCode == 201) {
            String response = http.getString();
            Serial.println("Response: " + response);
            http.end();
            return COMM_SUCCESS;
        } else if (
