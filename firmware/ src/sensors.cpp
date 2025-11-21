#include "sensors.h"
#include "config.h"
#include <Wire.h>
#include <MAX30105.h>
#include <MPU6050.h>

MAX30105 heartRateSensor;
MPU6050 mpu;

bool heartRateInitialized = false;
bool mpuInitialized = false;

int stepCount = 0;
float lastAccelMagnitude = 0;
int movementCounter = 0;
int interactionCounter = 0;

void initSensors() {
    Serial.println("Initializing sensors...");
    
    // Initialize Heart Rate Sensor
    if (heartRateSensor.begin(Wire, I2C_SPEED_STANDARD)) {
        heartRateSensor.setup();
        heartRateSensor.setPulseAmplitudeRed(0x0A);
        heartRateInitialized = true;
        Serial.println("Heart rate sensor initialized");
    } else {
        Serial.println("Heart rate sensor not found");
    }
    
    // Initialize MPU6050
    mpu.initialize();
    if (mpu.testConnection()) {
        mpuInitialized = true;
        Serial.println("MPU6050 initialized");
    } else {
        Serial.println("MPU6050 not found");
    }
    
    Serial.println("Sensors initialization complete");
}

bool readHeartRate(float &heartRate) {
    if (!heartRateInitialized) {
        heartRate = 0;
        return false;
    }
    
    long irValue = heartRateSensor.getIR();
    
    if (irValue < 50000) {
        heartRate = 0;  // No finger detected
        return false;
    }
    
    // Simple heart rate calculation (in production, use proper algorithm)
    static float filteredBPM = 75.0;
    float rawBPM = map(irValue, 50000, 100000, 60, 100);
    
    // Simple low-pass filter
    filteredBPM = filteredBPM * 0.9 + rawBPM * 0.1;
    heartRate = filteredBPM;
    
    return true;
}

bool readAccelerometer(float &x, float &y, float &z) {
    if (!mpuInitialized) {
        x = y = z = 0;
        return false;
    }
    
    int16_t ax, ay, az;
    mpu.getAcceleration(&ax, &ay, &az);
    
    // Convert to g
    x = ax / 16384.0;
    y = ay / 16384.0;
    z = az / 16384.0;
    
    return true;
}

int calculateSteps() {
    float x, y, z;
    if (!readAccelerometer(x, y, z)) {
        return stepCount;
    }
    
    // Calculate magnitude
    float magnitude = sqrt(x*x + y*y + z*z);
    
    // Simple step detection using threshold crossing
    const float STEP_THRESHOLD = 1.2;
    const float STEP_COOLDOWN = 0.5;
    
    static unsigned long lastStepTime = 0;
    unsigned long currentTime = millis();
    
    if (magnitude > STEP_THRESHOLD && 
        lastAccelMagnitude < STEP_THRESHOLD &&
        (currentTime - lastStepTime) > 500) {
        stepCount++;
        lastStepTime = currentTime;
    }
    
    lastAccelMagnitude = magnitude;
    return stepCount;
}

float calculateTemperature() {
    if (!heartRateInitialized) return 0;
    
    // MAX30102 has a built-in temperature sensor
    return heartRateSensor.readTemperature();
}

int detectMovement() {
    float x, y, z;
    if (!readAccelerometer(x, y, z)) {
        return movementCounter;
    }
    
    float magnitude = sqrt(x*x + y*y + z*z);
    
    // Detect significant movement
    const float MOVEMENT_THRESHOLD = 1.3;
    
    if (magnitude > MOVEMENT_THRESHOLD) {
        movementCounter++;
    }
    
    return movementCounter;
}

SensorData collectAllSensorData() {
    SensorData data;
    
    readHeartRate(data.heartRate);
    data.steps = calculateSteps();
    data.temperature = calculateTemperature();
    data.movementCount = detectMovement();
    data.interactionCount = interactionCounter;
    data.timestamp = millis();
    
    return data;
}

FocusData calculateFocusScore(const SensorData &data) {
    FocusData focus;
    
    // Heart Rate Score (0-1)
    float hrScore = 1.0;
    if (data.heartRate < HEART_RATE_MIN || data.heartRate > HEART_RATE_MAX) {
        hrScore = 0.7;
    }
    
    // Movement Score (0-1)
    float movementScore = 1.0;
    if (data.movementCount > FOCUS_MOVEMENT_THRESHOLD) {
        movementScore = max(0.0f, 1.0f - (data.movementCount / 20.0f));
    }
    
    // Interaction Score (0-1)
    float interactionScore = 1.0;
    if (data.interactionCount > 3) {
        interactionScore = max(0.0f, 1.0f - (data.interactionCount / 10.0f));
    }
    
    // Calculate weighted focus score
    focus.focusScore = (hrScore * 0.25 + movementScore * 0.40 + interactionScore * 0.35) * 100;
    focus.distractionCount = max(0, data.movementCount - DISTRACTION_THRESHOLD);
    focus.heartRateAvg = data.heartRate;
    focus.movementScore = movementScore * 100;
    focus.sessionStart = data.timestamp - SERVER_SYNC_INTERVAL;
    focus.sessionEnd = data.timestamp;
    
    return focus;
}

void displaySensorData(const SensorData &data) {
    Serial.println("=== Sensor Data ===");
    Serial.print("Heart Rate: ");
    Serial.print(data.heartRate);
    Serial.println(" BPM");
    Serial.print("Steps: ");
    Serial.println(data.steps);
    Serial.print("Temperature: ");
    Serial.print(data.temperature);
    Serial.println(" °C");
    Serial.print("Movement Count: ");
    Serial.println(data.movementCount);
}

bool isSensorHealthy() {
    return (heartRateInitialized && mpuInitialized);
}
