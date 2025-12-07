UVision Embedded Firmware Pseudocode
===================================

SYSTEM INITIALIZATION
---------------------
Initialize serial communication for diagnostics
Initialize I²C bus for peripheral communication

Configure OLED display:
    - Set resolution and I²C address
    - Clear display buffer
    - Set text size and color

Configure UV sensor (AS7331):
    - Initialize sensor over I²C
    - Enable synchronous measurement mode
    - Enable interrupt on data-ready signal

Configure vibration motor:
    - Set PWM-capable GPIO as output
    - Ensure motor output is disabled at startup

Attach interrupt service routine to UV sensor interrupt pin
Initialize timing variables for periodic sampling
Trigger initial UV sensor measurement

------------------------------------------------------------

MAIN EXECUTION LOOP
-------------------
If UV sensor interrupt flag is set AND sampling interval has elapsed:
    Clear interrupt flag
    Update last sample timestamp

    Read raw UVA and UVB data from UV sensor over I²C
    Apply empirical scaling to sensor outputs

    Compute UV Index using weighted UVA/UVB contributions
    Determine exposure classification based on UV Index thresholds

    Update OLED display:
        - Display current UV Index value
        - Display exposure guidance message

    If UV Index exceeds warning threshold:
        Activate vibration motor using PWM output for a fixed duration
    Else:
        Ensure vibration motor remains disabled

    Trigger next synchronized UV sensor measurement

------------------------------------------------------------

INTERRUPT SERVICE ROUTINE
-------------------------
When UV sensor asserts data-ready interrupt:
    Set data-ready flag for main loop processing

