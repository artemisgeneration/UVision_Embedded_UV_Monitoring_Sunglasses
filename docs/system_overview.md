# UVision System Overview

UVision is an embedded wearable UV monitoring system designed to operate as a fully self-contained device. All sensing, processing, and user feedback are handled locally on the device without reliance on external computation, connectivity, or mobile applications.

---

## System Architecture

The system is built around an ESP32-based microcontroller that coordinates UV data acquisition, processing, and user feedback. A SparkFun AS7331 UV sensor provides ultraviolet intensity data via I²C communication, while an OLED display and vibration motor deliver real-time user feedback.

The embedded firmware follows a synchronous, interrupt-driven architecture to ensure reliable and timely UV measurements while minimizing unnecessary processing.

---

## Data Flow

1. The AS7331 UV sensor performs a synchronized UV measurement.
2. Upon completion, the sensor asserts an interrupt signal.
3. The ESP32 interrupt service routine flags new data availability.
4. Firmware retrieves raw UVA and UVB measurements over I²C.
5. Sensor outputs are empirically scaled to approximate UV exposure level.
6. A real-time UV index value is computed and classified.
7. The OLED display updates to reflect current exposure status.
8. If exposure exceeds a defined threshold, the vibration motor is activated using PWM control.

This flow repeats periodically at a fixed sampling interval.

---

## Embedded Firmware Design

The firmware is written in C/C++ and organized around the following functional responsibilities:
- Interrupt-driven UV data acquisition
- Periodic sampling control
- UV index computation and classification
- Display formatting and rendering
- Haptic feedback control via PWM output
- Diagnostic serial logging and error handling

All decision logic executes on-device to ensure deterministic behavior and immediate user feedback.

---

## Prototyping and Validation Strategy

Initial development was performed on a breadboard to validate:
- I²C communication reliability
- Interrupt timing and synchronization
- Firmware logic and execution flow
- Component interoperability

After successful validation, the system was migrated to a hand-soldered works-like prototype and integrated into a wearable sunglasses form factor for system-level testing.

---

## Design Considerations and Limitations

The system is intended as a functional prototype and demonstration platform. UV index values are empirically scaled and are not calibrated for certified medical or safety-critical use. Power consumption, enclosure optimization, and environmental sealing were considered but not fully optimized in this prototype iteration.

---

## Summary

UVision demonstrates end-to-end embedded system development, including sensor integration, firmware design, electrical architecture, prototyping, and user-facing feedback — all implemented within a constrained wearable form factor.

