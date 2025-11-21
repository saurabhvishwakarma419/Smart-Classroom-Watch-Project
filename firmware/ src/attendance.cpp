#include "attendance.h"
#include "config.h"
#include "communication.h"
#include <Wire.h>
#include <Adafruit_PN532.h>
#include <ArduinoJson.h>

Adafruit_PN532 nfc(I2C_SDA, I2C_SCL);
bool nfcInitialized = false;
unsigned long lastAttendanceTime = 0;

void initAttendance() {
    Serial.println("Initializing NFC reader...");
    
    nfc.begin();
    uint32_t versiondata = nfc.getFirmwareVersion();
    
    if (!versiondata) {
        Serial.println("NFC reader not found!");
        nfcInitialized = false;
        return;
    }
    
    Serial.print("Found chip PN5");
    Serial.println((versiondata >> 24) & 0xFF, HEX);
    
    // Configure board to read RFID tags
    nfc.SAMConfig();
    nfcInitialized = true;
    
    Serial.println("NFC reader initialized!");
}

bool readNFCTag(String &tagId) {
    if (!nfcInitialized) return false;
    
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
    uint8_t uidLength;
    
    bool success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 100);
    
    if (success) {
        tagId = "";
        for (uint8_t i = 0; i < uidLength; i++) {
            if (uid[i] < 0x10) tagId += "0";
            tagId += String(uid[i], HEX);
        }
        tagId.toUpperCase();
        
        Serial.print("NFC Tag detected: ");
        Serial.println(tagId);
        return true;
    }
    
    return false;
}

bool markAttendance(const String &nfcTagId) {
    // Check if already marked today
    if (isAttendanceMarkedToday()) {
        Serial.println("Attendance already marked today");
        return false;
    }
    
    AttendanceData data;
    data.studentId = STUDENT_ID;
    data.nfcTagId = nfcTagId;
    data.location = getCurrentLocation();
    data.timestamp = millis();
    data.deviceMac = getDeviceMacAddress();
    
    // Extract class ID from NFC tag (first 4 characters)
    data.classId = nfcTagId.substring(0, 4);
    
    bool success = sendAttendanceToServer(data);
    
    if (success) {
        lastAttendanceTime = millis();
        Serial.println("Attendance marked successfully!");
    } else {
        Serial.println("Failed to mark attendance");
    }
    
    return success;
}

bool sendAttendanceToServer(const AttendanceData &data) {
    if (!isWiFiConnected()) {
        Serial.println("WiFi not connected");
        return false;
    }
    
    // Create JSON payload
    StaticJsonDocument<512> doc;
    doc["studentId"] = data.studentId;
    doc["classId"] = data.classId;
    doc["nfcTagId"] = data.nfcTagId;
    doc["location"] = data.location;
    doc["deviceMac"] = data.deviceMac;
    doc["timestamp"] = data.timestamp;
    
    String jsonData;
    serializeJson(doc, jsonData);
    
    Serial.print("Sending attendance data: ");
    Serial.println(jsonData);
    
    CommStatus status = sendHTTPPost(API_ENDPOINT_ATTENDANCE, jsonData);
    return (status == COMM_SUCCESS);
}

String getCurrentLocation() {
    // In production, this could use GPS
    // For now, return device location
    return "Classroom";
}

bool isAttendanceMarkedToday() {
    // Check if attendance was marked in last 24 hours
    unsigned long currentTime = millis();
    unsigned long timeSinceLastAttendance = currentTime - lastAttendanceTime;
    
    // 24 hours = 86400000 milliseconds
    return (timeSinceLastAttendance < 86400000);
}

void displayAttendanceStatus(bool success) {
    // This would update the OLED display
    // Implementation depends on display library
    Serial.print("Attendance Status: ");
    Serial.println(success ? "SUCCESS" : "FAILED");
}
