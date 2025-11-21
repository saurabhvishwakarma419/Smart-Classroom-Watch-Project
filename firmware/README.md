# Smart Classroom Watch - Firmware

ESP32-based firmware for the Smart Classroom Watch wearable device.

## Features
- NFC/RFID attendance marking
- Heart rate and movement sensors
- Focus score calculation
- Emergency SOS alerts
- WiFi and Bluetooth connectivity
- OLED display interface
- Low-power management

## Hardware Requirements
- ESP32 DevKit v1 or similar
- PN532 NFC Module
- MAX30102 Heart Rate Sensor
- MPU6050 Accelerometer/Gyroscope
- 0.96" OLED Display (SSD1306)
- Push buttons for input
- LiPo Battery (3.7V, 500mAh)

## Pin Configuration
NFC/RFID (PN532 - I2C):

SDA: GPIO 21
SCL: GPIO 22

Heart Rate Sensor (MAX30102 - I2C):

SDA: GPIO 21
SCL: GPIO 22

Accelerometer (MPU6050 - I2C):

SDA: GPIO 21
SCL: GPIO 22

OLED Display (SSD1306 - I2C):

SDA: GPIO 21
SCL: GPIO 22

Buttons:

Emergency SOS: GPIO 0
Menu/Select: GPIO 15
Back: GPIO 2

Battery Monitor:

ADC: GPIO 34
## Software Dependencies
- PlatformIO
- Adafruit_PN532
- MAX30105
- MPU6050
- Adafruit_SSD1306
- WiFi.h
- HTTPClient.h
- ArduinoJson

## Installation

### Using PlatformIO
```bash
cd firmware/
pio run --target upload
pio device monitor
```

### Using Arduino IDE
1. Install ESP32 board support
2. Install required libraries via Library Manager
3. Open src/main.cpp
4. Select board: ESP32 Dev Module
5. Upload

## Configuration
Edit `include/config.h` to set:
- WiFi credentials
- API server endpoint
- Device ID
- Sensor thresholds

## Usage
1. Power on the watch
2. Connect to WiFi (automatic)
3. Walk near NFC reader to mark attendance
4. View stats on OLED display
5. Press and hold emergency button for SOS

## Power Management
- Deep sleep after 2 minutes of inactivity
- Wake on button press or motion detection
- Battery level monitoring and low-battery alerts

## Troubleshooting
- **WiFi not connecting**: Check credentials in config.h
- **Sensors not responding**: Verify I2C connections
- **Display blank**: Check OLED power and I2C address
- **NFC not reading**: Ensure PN532 is in I2C mode

## License
MIT License
