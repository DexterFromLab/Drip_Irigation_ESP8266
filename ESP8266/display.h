#include <Time.h>
#include <TimeLib.h>
#include "definicje.h"
#include "display/SSD1306.h" // alias for `#include "SSD1306Wire.h"`
#include "display/OLEDDisplayUi.h" // Include the UI lib
#include "images.h"// Include custom images
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiAP.h>
#include <string.h>

void displayOled(void);
void display_time(void);
void drawCenter(const char * text, char size,char posx, char posy);
void displayInit(void);
void drawFoundSensor(const char * text);
void displayAddres(void);
void displayTempAndHum(void);
void displayExternalTemp(char measNum);
void displayExternalHum(char measNum);
void displayExternalSoil(char measNum);
void displayExternalTitle(char measNum);
void displayExternalValues(char measNum);
//extern DS1302RTC RTC(RTC_RESET, RTC_CLK, RTC_DATA);
//extern SSD1306  display(0x3c, OLED_SDA, OLED_SCL);