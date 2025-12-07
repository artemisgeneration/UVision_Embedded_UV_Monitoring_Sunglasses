# UVision — Embedded Wearable UV Monitoring System

UVision is an embedded wearable system designed to monitor real-time ultraviolet (UV) exposure and provide on-device visual and haptic feedback without reliance on external devices.

## System Overview
The system integrates a UV sensor, ESP32-based microcontroller, OLED display, and vibration motor into a wearable sunglasses form factor. All sensing, processing, and user feedback are handled locally through embedded firmware.

## My Role
I served as the lead for the electrical and embedded software subsystems, responsible for:
- Firmware architecture and development (C/C++)
- I²C sensor integration and interrupt-driven data acquisition
- Electrical integration schematic development
- Breadboard validation and hand-soldered prototype assembly
- System-level debugging and validation

## Hardware Components
- Arduino Nano ESP32
- SparkFun AS7331 UV Sensor (I²C, interrupt-driven)
- 128x32 OLED Display (I²C)
- PWM-capable vibration motor with control module

## Firmware Functionality
- Periodic UV data acquisition via I²C
- Empirical scaling of sensor output
- Real-time UV exposure classification
- Threshold-based OLED display updates
- Haptic feedback for high UV exposure
- Diagnostic serial logging and error handling

## System Architecture
![Electrical Schematic](hardware/electrical_schematic.png)

## Prototyping and Integration
![Breadboard Prototype](hardware/breadboard_prototype.jpg)
![Wearable Prototype](media/wearable_prototype.jpg)

## Display Output
![OLED Output](media/oled_output.jpg)

## Repository Structure

