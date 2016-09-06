#ifndef _ESP8255_
#define _ESP8266_

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TimeLib.h>
#include <TimeAlarms.h>
#include <WiFiUdp.h>
#include "./ajax-requests.h"
#include "./equationParser.h"
#include <limits.h>
#include <vector>
#include "./externalProbes.h"
#include "./definicje.h"

extern "C" {
#include "c_types.h"
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
}

#define ONE_WIRE_BUS 2  // DS18B20 pin

#define wielokosc_bufora_pomiarow 100   //IloĹ›Ä‡ pomiarĂłw do wykresu
#define POMIARY_USREDNIANE	2
#define ApClientPin 16 //jesli 1 to lacze sie z siecia jesli 0 tworze wlasna





void measure_task();
void scriptTask(void);
String formatBytes(size_t bytes);
void timSterControl();
void temperatureControl();
void airHumControl();
void HumControl();
void ethernetControl();
void systemControl();
void scriptSettings();
void parseEquation();
bool handleFileRead(String path);
void handleFileUpload();
void handleFileDelete();
void fileDelete();
void handleFileCreate();
void handleFileList();
void getScriptLine();
void getScriptName();
void getScriptContent();
void getScriptsNames();
void Repeats();
time_t getNtpTime();
void sendNTPpacket(IPAddress &address);
void digitalClockDisplay();
void printDigits(int digits);
void saveTimesConfig(void);
void readTimesConfig(void);
void saveTemperatureConfig(void);
void readTemperaturesConfig(void);
void saveAirhumConfig(void);
void readAirhumConfig(void);
void saveHumConfig(void);
void readHumConfig(void);
void saveEthernetConfig(void);
void readEthernetConfig(void);
void saveSystemConfig(void);
void readSystemConfig(void);
void tryToConnect(void);
String listNetworks();
String printEncryptionType(int thisType);
String saveScriptFile(String scriptContent);

#endif