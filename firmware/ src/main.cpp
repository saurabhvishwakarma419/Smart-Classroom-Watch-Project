#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include "config.h"
#include "attendance.h"
#include "sensors.h"
#include "communication.h"
#include "emergency.h"

// Display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Global variables
unsigned long lastSensorRead = 0;
unsigned long lastDisplayUpdate = 0;
unsigned long lastServerSync = 0;
unsigned long lastActivity = 0;

SensorData currentSensorData;
FocusData currentFocusData;
bool attendanceMarked = false;

// Function prototypes
void setup();
void loop();
void displayHomeScreen();
void displayTime();
void enterDeepSleep();
void checkBatteryLevel();

void setup() {
    Serial.begin(SERIAL_BAUD);
    
    if (DEBUG_MODE) {
        Serial.println("\n\n=== Smart Classroom Watch ===");
        Serial.println("Initializing...");
    }

    // Initialize I2C
    Wire.begin(I2C_SDA, I2C_SCL);
    
    // Initialize display
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println("SSD1306 allocation failed");
        for (;;);
    }
    
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Smart Watch");
    display.println("Booting...");
    display.display();
    delay(2000);

    // Initialize buttons
    pinMode(BTN_EMERGENCY, INPUT_PULLUP);
    pinMode(BTN_SELECT, INPUT_PULLUP);
    pinMode(BTN_BACK, INPUT_PULLUP);
    pinMode(BATTERY_PIN, INPUT);

    // Initialize modules
    initWiFi();
    initSensors();
    initAttendance();
    initEmergency();

    if (DEBUG_MODE) {
        Serial.println("Initialization complete!");
        Serial.print("Device MAC: ");
        Serial.println(getDeviceMacAddress());
    }

    lastActivity = millis();
}

void loop() {
    unsigned long currentTime = millis();

    // Check emergency button (highest priority)
    checkEmergencyButton();

    // Read sensors periodically
    if (currentTime - lastSensorRead >= SENSOR_READ_INTERVAL) {
        currentSensorData = collectAllSensorData();
        currentFocusData = calculateFocusScore(currentSensorData);
        lastSensorRead = currentTime;
        lastActivity = currentTime;
        
        if (DEBUG_MODE) {
            Serial.print("Heart Rate: ");
            Serial.print(currentSensorData.heartRate);
            Serial.print(" BPM | Steps: ");
            Serial.print(currentSensorData.steps);
            Serial.print(" | Focus: ");
            Serial.println(currentFocusData.focusScore);
        }
    }

    // Update display periodically
    if (currentTime - lastDisplayUpdate >= DISPLAY_UPDATE_INTERVAL) {
        displayHomeScreen();
        lastDisplayUpdate = currentTime;
    }

    // Sync with server periodically
    if (currentTime - lastServerSync >= SERVER_SYNC_INTERVAL) {
        if (isWiFiConnected()) {
            syncDataWithServer();
            lastServerSync = currentTime;
        }
    }

    // Check for NFC tag (attendance)
    if (!attendanceMarked) {
        String nfcTag;
        if (readNFCTag(nfcTag)) {
            attendanceMarked = markAttendance(nfcTag);
            displayAttendanceStatus(attendanceMarked);
            lastActivity = currentTime;
        }
    }

    // Check battery level
    checkBatteryLevel();

    // Enter deep sleep if inactive
    if (currentTime - lastActivity >= SLEEP_TIMEOUT) {
        enterDeepSleep();
    }

    delay(100);  // Small delay to prevent watchdog issues
}

void displayHomeScreen() {
    display.clearDisplay();
    
    // Time
    display.setTextSize(2);
    display.setCursor(10, 0);
    displayTime();
    
    // Divider line
    display.drawLine(0, 20, SCREEN_WIDTH, 20, SSD1306_WHITE);
    
    // Stats
    display.setTextSize(1);
    
    // Heart Rate
    display.setCursor(0, 25);
    display.print("HR: ");
    display.print((int)currentSensorData.heartRate);
    display.print(" BPM");
    
    // Steps
    display.setCursor(0, 35);
    display.print("Steps: ");
    display.print(currentSensorData.steps);
    
    // Focus Score
    display.setCursor(0, 45);
    display.print("Focus: ");
    display.print((int)currentFocusData.focusScore);
    display.print("%");
    
    // WiFi Status
    display.setCursor(0, 55);
    if (isWiFiConnected()) {
        display.print("WiFi: OK");
    } else {
        display.print("WiFi: --");
    }
    
    // Battery Level
    int batteryPercent = map(analogRead(BATTERY_PIN), 0, 4095, 0, 100);
    display.setCursor(70, 55);
    display.print("Bat:");
    display.print(batteryPercent);
    display.print("%");
    
    display.display();
}

void displayTime() {
    // Simple time display (hours:minutes)
    unsigned long seconds = millis() / 1000;
    int hours = (seconds / 3600) % 24;
    int minutes = (seconds / 60) % 60;
    
    if (hours < 10) display.print("0");
    display.print(hours);
    display.print(":");
    if (minutes < 10) display.print("0");
    display.print(minutes);
}

void enterDeepSleep() {
    if (DEBUG_MODE) {
        Serial.println("Entering deep sleep...");
    }
    
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(20, 28);
    display.println("Sleep Mode");
    display.display();
    delay(1000);
    
    // Configure wake-up sources
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0);  // Wake on emergency button
    
    // Enter deep sleep
    esp_deep_sleep_start();
}

void checkBatteryLevel() {
    static unsigned long lastBatteryCheck = 0;
    unsigned long currentTime = millis();
    
    if (currentTime - lastBatteryCheck >= 60000) {  // Check every minute
        int batteryPercent = map(analogRead(BATTERY_PIN), 0, 4095, 0, 100);
        
        if (batteryPercent <= BATTERY_LOW_THRESHOLD) {
            // Low battery warning
            display.clearDisplay();
            display.setTextSize(2);
            display.setCursor(10, 20);
            display.println("LOW");
            display.println("BATTERY");
            display.display();
            delay(2000);
        }
        
        lastBatteryCheck = currentTime;
    }
}
