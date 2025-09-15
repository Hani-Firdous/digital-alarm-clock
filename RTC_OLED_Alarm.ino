                                                                        
/**********************************************************************
define enum
**********************************************************************/
enum{
  ERROR = 0,
  OK,
  PENDING,
};
/**********************************************************************
define struct
**********************************************************************/

struct TIMEFLAGS{
  unsigned int year   :7;
  unsigned int month  :4;
  unsigned int day    :5;
  unsigned int hour   :5;
  unsigned int minute :6;
  unsigned int second :6;
};
/**********************************************************************
system include
**********************************************************************/
#include <string.h>
#include <stdlib.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"
/**********************************************************************
user include
**********************************************************************/
/**********************************************************************
user define
**********************************************************************/
#define printf(s) Serial.print(s);
#define BUZZER_PIN 8
#define BUTTON_PIN 2
#define ALARM_HOUR 7
#define ALARM_MINUTE 40

/**********************************************************************
global variables
**********************************************************************/
RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27,16,2);
struct TIMEFLAGS timeFlags;
bool alarmTriggered = false;
int timeOutLCD = 0;
int timerOutLCD = 0;
/**********************************************************************
global functions
**********************************************************************/
void RTC_Init(void);
void RTC_Get(char *Date, char* Time);
void RTC_Set(int year,int month,int day,int hour,int minute,int sec);
void LCD_Ini(void);
void ItoA(int Num, char *Ptr, int Digi);
void showDateTime(void);
void outToggle(char pin);
void silenceAlarm(void);
void soundAlarm(void);
void displayAlarm(void);

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(BUZZER_PIN,OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  RTC_Init();
  LCD_Ini();
  // put your setup code here, to run once:
}

void loop() {
  // Get current time from RTC
  DateTime now = rtc.now();
  
  // 1. FIRST AND MOST IMPORTANT: Check silence button during alarm
  if (alarmTriggered && digitalRead(BUTTON_PIN) == LOW) {
    delay(50); // Wait a moment to confirm button press
    if (digitalRead(BUTTON_PIN) == LOW) {
      silenceAlarm(); // Immediately stop sound and return to normal display
    }
  }
  
  // 2. Check if it's exactly time to trigger the alarm (only if not already triggered)
  if (now.hour() == ALARM_HOUR && now.minute() == ALARM_MINUTE && !alarmTriggered) {
    alarmTriggered = true; // Activate the alarm - it will now run forever until button press
  }
  
  // 3. Handle the current state
  if (alarmTriggered) {
    // ALARM IS ACTIVE: Sound buzzer and show only alarm message
    soundAlarm();    // Continuous buzzing
    displayAlarm();  // Only shows "ALARM!" on LCD
  } else {
    // NORMAL OPERATION: No sound, show time/date
    noTone(BUZZER_PIN); // Ensure buzzer is silent
    showDateTime();     // Normal date/time display
  }

  // Small delay for stability
  delay(100);
}

  // put your main code here, to run repeatedly:
/*
@param function:
@param def:
@param in:
@param out:
@param return:
*/
void silenceAlarm() {
  alarmTriggered = false;   // Clear the alarm flag
  noTone(BUZZER_PIN);       // Turn off buzzer immediately
  lcd.clear();              // Clear the alarm message from display
  // Normal display will resume on next loop iteration
}
/*
@param function: void RTC_Init(void)
@param def: Initialize RTC driver
@param in: none
@param out: none
@param return: none
*/
void RTC_Init(void){
  if(rtc.begin() == ERROR){
    printf("fail init RTC\r\n");
  }else{
    printf("RTC OK\r\n");
  }
}
void soundAlarm() {
  // Pulsating "beep-beep-beep" pattern
  unsigned long currentTime = millis();
  if (currentTime % 2000 < 1000) { // Beep for 1 second, off for 1 second
    tone(BUZZER_PIN, 1000); // 1kHz tone
  } else {
    noTone(BUZZER_PIN); // Silence
  }
}
void displayAlarm() {
  lcd.setCursor(0, 0);
  lcd.print("    ALARM!    "); // Center the message
  lcd.setCursor(0, 1);
  lcd.print("  PRESS BUTTON "); // Instructions on second line
}
/*
@param function: void RTC_Get(int year, int month, int day, int hour, int minut, int sec)
@param def: get the date and time
@param in:  none
@param out: date/hour
@param return: none
*/
void RTC_Get(char *Date, char* Time){
  char Tmp[10] = {0};
  DateTime _get = rtc.now();

  timeFlags.year = ((_get.year()-2000));
  timeFlags.month = _get.month();
  timeFlags.day = _get.day();

  timeFlags.hour = _get.hour();
  timeFlags.minute = _get.minute();
  timeFlags.second = _get.second();

  ItoA(timeFlags.day,Tmp,2);
  strcat(Date,Tmp);
  strcat(Date,"/");
  ItoA(timeFlags.month,Tmp,2);
  strcat(Date,Tmp);
  strcat(Date,"/");
  ItoA(timeFlags.year,Tmp,2);
  strcat(Date,Tmp);

  ItoA(timeFlags.hour,Tmp,2);
  strcat(Time,Tmp);
  strcat(Time,":");
  ItoA(timeFlags.minute,Tmp,2);
  strcat(Time,Tmp);
  strcat(Time,":");
  ItoA(timeFlags.second,Tmp,2);
  strcat(Time,Tmp);
}
/*
@param function: void RTC_Set(int year, int month, int day, int hour, int minut, int sec)
@param def: 
@param in:
@param out:
@param return:
*/
void RTC_Set(int year,int month,int day,int hour,int minute,int sec){
  rtc.adjust(DateTime(year, month, day, hour, minute, sec));
}
/*
@param function: void LCD_Ini(void)
@param def: Initialize LCD driver
@param in: none
@param out: none
@param return: none
*/
void LCD_Ini(void){
  lcd.init();
  lcd.backlight();
  lcd.clear();
}
/*
@param function: void ItoA(int Num, char *Ptr, int Digi)
@param def:
@param in:
@param out:
@param return:
*/
void ItoA(int Num, char *Ptr, int Digi){
  int i = 0, j = 0;
  do{
    Ptr[i++] = (Num % 10) + 0x30;
    Num /= 10;
  }while(Num > 0);

  if(i < Digi){
    int k = i;
    for(j = 0; j < (Digi - k); j++)
      Ptr[i++] = 0x30;
  }

  char tmp[i + 1];
  memcpy(tmp, (const char*)Ptr, i);

  for(j = 0; j < i; j++)
    *Ptr++ = tmp[i - j - 1];
  *Ptr = 0;
}
/*
@param function:
@param def:
@param in:
@param out:
@param return:
*/
void showDateTime(void){
  char fecha[15] = {0};
  char hora[15] = {0};
  RTC_Get(fecha,hora);

  lcd.setCursor(4,0);
  lcd.print(fecha);
  lcd.setCursor(4,1);
  lcd.print(hora);
}
/*
@param function:
@param def:
@param in:
@param out:
@param return:
*/

/*
@param function:
@param def:
@param in:
@param out:
@param return:
*/
void outToggle(char pin){
  int status = digitalRead(pin);
  digitalWrite(pin,!status);
}
/*
@param function:
@param def:
@param in:
@param out:
@param return:
*/