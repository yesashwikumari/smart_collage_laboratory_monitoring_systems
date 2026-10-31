
# Smart College Laboratory Monitoring System

## 1. Introduction
The Smart College Laboratory Monitoring System is an ESP32-based project designed to monitor laboratory conditions and provide alerts during emergency situations.

It monitors environmental conditions, gas leakage, fire, voltage and current.

## 2. Objectives
- Monitor laboratory environmental conditions.
- Detect smoke and gas leakage.
- Detect fire emergencies.
- Monitor voltage and current.
- Provide alerts during emergency conditions.
- Demonstrate an automatic emergency door mechanism.

## 3. Components Used
- ESP32 Development Board
- DHT11 Temperature and Humidity Sensor
- MQ-2 Gas Sensor
- ZMPT101B Voltage Sensor
- ACS712 Current Sensor
- Flame Sensor
- PIR Motion Sensor
- Servo Motor
- OLED Display
- Buzzer
- Green and Red LEDs

## 4. Working Principle
The sensors continuously monitor laboratory conditions and send their readings to the ESP32.

The ESP32 processes the sensor data and displays the sensor status on the OLED display.

When an emergency condition is detected, the system activates the buzzer. The PIR sensor is used to detect motion for the servo-based emergency door mechanism.

## 5. Technologies Used
- Hardware: ESP32 and electronic sensors
- Programming Language: Embedded C/C++ 
- IDE: Arduino IDE

## 6. Applications
- College laboratories
- Educational institutions
- Electronics laboratories
- Laboratory safety monitoring

## 7. Project Status
Prototype developed for academic demonstration.

## 8. Conclusion

The Smart College Laboratory Monitoring System demonstrates the use of ESP32 and multiple sensors for laboratory safety monitoring. It provides a prototype for detecting emergency conditions and generating alerts.

## 9. Future Scope
- IoT-based remote monitoring.
- Mobile application integration.
- Cloud-based data storage.
- Improved emergency notification system.
