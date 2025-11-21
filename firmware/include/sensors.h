#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

// Sensor data structure
struct SensorData {
    float heartRate;
    int steps;
    float temperature;
    int movementCount;
    int interactionCount;
    unsigned long timestamp;
};

// Focus analytics structure
struct FocusData {
    float focusScore;
    int distractionCount;
    float heartRateAvg;
    float movementScore;
    unsigned long sessionStart;
    unsigned long sessionEnd;
};

// Function declarations
void initSensors();
bool readHeartRate(float &heartRate);
bool readAccelerometer(float &x, float &y, float &z);
int calculateSteps();
float calculateTemperature();
int detectMovement();
SensorData collectAllSensorData();
FocusData calculateFocusScore(const SensorData &data);
void displaySensorData(const SensorData &data);
bool isSensorHealthy();

#endif
