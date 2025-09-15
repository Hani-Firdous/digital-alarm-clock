# digital-alarm-clock
An Arduino-based alarm clock with real-time tracking using DS1307 RTC module. Features a 16x2 LCD display for time/date, programmable alarm with buzzer notification, and one-button silencing functionality. 

## Components
- Arduino Uno
- DS1307 RTC
- 16x2 LCD (I2C)
- Buzzer
- Push Button

## Wiring
- RTC SDA → A4
- RTC SCL → A5  
- LCD SDA → A4
- LCD SCL → A5
- Buzzer → Pin 8
- Button → Pin 2 to GND

## Usage
1. Set alarm time in code
2. Alarm triggers at set time
3. Press button to stop alarm
