/*
  FSWebServer - Example WebServer with SPIFFS backend for esp8266
  Copyright (c) 2015 Hristo Gochkov. All rights reserved.
  This file is part of the ESP8266WebServer library for Arduino environment.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  upload the contents of the data folder with MkSPIFFS Tool ("ESP8266 Sketch Data Upload" in Tools menu in Arduino IDE)
  or you can upload the contents of a folder if you CD in that folder and run the following command:
  for file in `ls -A1`; do curl -F "file=@$PWD/$file" esp8266fs.local/edit; done

  access the sample web page at http://esp8266fs.local
  edit the page by going to http://esp8266fs.local/edit
*/
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TimeLib.h>
#include <TimeAlarms.h>
#include <WiFiUdp.h>
#include "./struct-filler.h"
#include "./ajax-requests.h"

#define DBG_OUTPUT_PORT Serial

#define ONE_WIRE_BUS 2  // DS18B20 pin

#define wielokosc_bufora_pomiarow 100   //Ilość pomiarów do wykresu
#define POMIARY_USREDNIANE	2
#define ApClientPin 16 //jesli 1 to lacze sie z siecia jesli 0 tworze wlasna

//Control structures
times_struct times;
temperature_struct temperature;
airHum_struct airHum;
hum_struct hum;
ethernet_struct ethernet;
system_struct System_s;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
//Measures
float temps[wielokosc_bufora_pomiarow];
float temps_avg[POMIARY_USREDNIANE];
//const char* ssid = "Ether Eden";
//const char* password = "pingwin199";
const char* host = "esp8266fs";

ESP8266WebServer server(80);
//holds the current upload
File fsUploadFile;
unsigned int i;//licznik petli
unsigned char tmp_cnt;
unsigned char buff_full_cnt;
unsigned int temps_cnt;

unsigned int measure_intervall = 10;//173

// NTP Servers:
//static const char ntpServerName[] = "us.pool.ntp.org";

const int timeZone = 2;     // Central European Time

WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets

float temp;               //przechowuję chwilową wartość temp
int last_meas_time[6];

char NTP_found;
void measure_task() {

  float suma;
  char tmpStr[150];
  DS18B20.requestTemperatures();
  temp = DS18B20.getTempCByIndex(0);
  temps_avg[tmp_cnt] = temp;
  //Serial.print("Temperature: ");
 // Serial.println(temps_avg[tmp_cnt]);
	snprintf(tmpStr,sizeof(tmpStr),"Temps%d: ",tmp_cnt);
	Serial.print(tmpStr);
	Serial.println(temps_avg[tmp_cnt]);
	if (tmp_cnt >= (POMIARY_USREDNIANE -1)) {
    //temps[temps_cnt];

    for (i = 0; i < POMIARY_USREDNIANE ; i++) {
      suma += temps_avg[i];
    }
	if(buff_full_cnt < wielokosc_bufora_pomiarow){
		temps[buff_full_cnt] = suma / (float)POMIARY_USREDNIANE;
		buff_full_cnt++;
	}else{
		for(i = 0;i<=(wielokosc_bufora_pomiarow - 1);i++){
			temps[i] = temps[i+1];
		}
		temps[wielokosc_bufora_pomiarow - 1] = suma / (float)POMIARY_USREDNIANE;
	}
    for(i = 0;i<wielokosc_bufora_pomiarow;i++){
		snprintf(tmpStr,sizeof(tmpStr),"Bufor pomiaru%d: ",i);
		Serial.print(tmpStr);
		Serial.println(temps[i]);
	}
    Serial.print("Srednia: ");
    Serial.println(temps[0]);
    if(temps_cnt < wielokosc_bufora_pomiarow) temps_cnt++;
	
	last_meas_time[0] = year();
	last_meas_time[1] = month();
	last_meas_time[2] = day();
	last_meas_time[3] = hour();
	last_meas_time[4] = minute();
	last_meas_time[5] = second();
	
    tmp_cnt = 0;
    return;
  }

  tmp_cnt++;
}

//format bytes
String formatBytes(size_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  } else {
    return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
  }
}

String getContentType(String filename) {
  if (server.hasArg("download")) return "application/octet-stream";
  else if (filename.endsWith(".htm")) return "text/html";
  else if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".xml")) return "text/xml";
  else if (filename.endsWith(".pdf")) return "application/x-pdf";
  else if (filename.endsWith(".zip")) return "application/x-zip";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}
/*
	int server.args() - liczba argumentów od ajaxa
	String arg(String name);        // get request argument value by name
	String arg(int i);              // get request argument value by number
	String argName(int i);          // get request argument name by number

Przykładowe zapytanie ajaxa z danymi
		$.ajax({
		  method: "POST",
		  url: "cont",
		  data: { name: "John", location: "Boston" }
		})
		  .done(function( msg ) {
			alert( "Data Saved: " + msg );
		  });
*/

void timSterControl(){
	DBG_OUTPUT_PORT.println("timSterControl: " + String(server.args()));
	
	times.tim1Max = String(server.arg("Tim1Max")).toInt();
	times.tim1Min = String(server.arg("Tim1Min")).toInt();
	times.tim1 = String(server.arg("Tim1Stat")).toInt();
	times.tim2Max = String(server.arg("Tim2Min")).toInt();
	times.tim2Min = String(server.arg("Tim2Max")).toInt();
	times.tim2 = String(server.arg("Tim2Stat")).toInt();
	times.tim3Max = String(server.arg("Tim3Min")).toInt();
	times.tim3Min = String(server.arg("Tim3Max")).toInt();
	times.tim3 = String(server.arg("Tim3Stat")).toInt();
	DBG_OUTPUT_PORT.println("Tim1Max: " + String(times.tim1Max) + ",tim1Min: " + String(times.tim1Min) + ",tim1: " + String((int)times.tim1) + ",tim2Max: " + String(times.tim2Max) + ",tim2Min: " + String(times.tim2Min) + ",tim2: " + String((int)times.tim2) + ",tim3Max: " + String(times.tim3Max) + ",tim3Min: " + String(times.tim3Min) + ",tim3: " + String((int)times.tim3) );
	server.send(200, "text/json", "data send correctly!");
	saveTimesConfig();
}

void temperatureControl(){
	temperature.tempMax = String(server.arg("tempMax")).toInt();
	temperature.tempMaxOn = String(server.arg("tempMaxOn")).toInt();
	temperature.tempMin = String(server.arg("tempMin")).toInt();
	temperature.tempMinOn = String(server.arg("tempMinOn")).toInt();
	temperature.DeltaT = String(server.arg("DeltaT")).toInt();
	temperature.DeltaTim = String(server.arg("DeltaTim")).toInt();
	temperature.DeltaRelayTime = String(server.arg("DeltaRelayTime")).toInt();
	temperature.DeltaTOn = String(server.arg("DeltaTOn")).toInt();
	DBG_OUTPUT_PORT.println("tempMax: "+String((int)temperature.tempMax)+" ,tempMaxOn:"+String((int)temperature.tempMaxOn)+" ,tempMin: "+String((int)temperature.tempMin)+" ,tempMinOn: "+String((int)temperature.tempMinOn)+",DeltaT: "+String((int)temperature.DeltaT)+" ,DeltaTim: "+String((int)temperature.DeltaTim)+" ,DeltaRelayTime: "+String((int)temperature.DeltaRelayTime)+" ,DeltaTOn: "+String((int)temperature.DeltaTOn));
	server.send(200, "text/json", "data send correctly!");
	saveTemperatureConfig();
}
void airHumControl(){
	airHum.wilgPowMax = String(server.arg("wilgPowMax")).toInt();
	airHum.wilgPowMaxOn = String(server.arg("wilgPowMaxOn")).toInt();
	airHum.wilgPowMin = String(server.arg("wilgPowMin")).toInt();
	airHum.wilgPowMinOn = String(server.arg("wilgPowMinOn")).toInt();
	airHum.DeltaWilgPow = String(server.arg("DeltaWilgPow")).toInt();
	airHum.DeltaWilgPowTim = String(server.arg("DeltaWilgPowTim")).toInt();
	airHum.DeltaWilgPowRelayTim = String(server.arg("DeltaWilgPowRelayTim")).toInt();
	airHum.DeltaWilgPowOn = String(server.arg("DeltaWilgPowOn")).toInt();
	DBG_OUTPUT_PORT.println("wilgPowMax: "+String((int)airHum.wilgPowMax)+" ,wilgPowMaxOn:"+String((int)airHum.wilgPowMaxOn)+" ,wilgPowMin: "+String((int)airHum.wilgPowMin)+" ,wilgPowMinOn: "+String((int)airHum.wilgPowMinOn)+",DeltaWilgPow: "+String((int)airHum.DeltaWilgPow)+" ,DeltaWilgPowTim: "+String((int)airHum.DeltaWilgPowTim)+" ,DeltaWilgPowRelayTim: "+String((int)airHum.DeltaWilgPowRelayTim)+" ,DeltaWilgPowOn: "+String((int)airHum.DeltaWilgPowOn));
	server.send(200, "text/json", "data send correctly!");
	saveAirhumConfig();
}
void HumControl(){
	hum.wilgMax = String(server.arg("wilgMax")).toInt();
	hum.wilgMaxOn = String(server.arg("wilgMaxOn")).toInt();
	hum.wilgMin = String(server.arg("wilgMin")).toInt();
	hum.wilgMinOn = String(server.arg("wilgMinOn")).toInt();
	hum.DeltaWilg = String(server.arg("DeltaWilg")).toInt();
	hum.DeltaWilgTim = String(server.arg("DeltaWilgTim")).toInt();
	hum.DeltaWilgRelayTim = String(server.arg("DeltaWilgRelayTim")).toInt();
	hum.DeltaWilgOn = String(server.arg("DeltaWilgOn")).toInt();
	DBG_OUTPUT_PORT.println("WilgMax: "+String((int)hum.wilgMax)+" ,WilgMaxOn:"+String((int)hum.wilgMaxOn)+" ,WilgMin: "+String((int)hum.wilgMin)+" ,WilgMinOn: "+String((int)hum.wilgMinOn)+",DeltaWilg: "+String((int)hum.DeltaWilg)+" ,DeltaWilgTim: "+String((int)hum.DeltaWilgTim)+" ,DeltaWilgRelayTim: "+String((int)hum.DeltaWilgRelayTim)+" ,DeltaWilgOn: "+String((int)hum.DeltaWilgOn));
	server.send(200, "text/json", "data send correctly!");
	saveHumConfig();
}
void ethernetControl(){
	ethernet.ip0 = String(server.arg("ip0")).toInt();
	ethernet.ip1 = String(server.arg("ip1")).toInt();
	ethernet.ip2 = String(server.arg("ip2")).toInt();
	ethernet.ip3 = String(server.arg("ip3")).toInt();
	ethernet.ip3 = String(server.arg("ip3")).toInt();
	ethernet.m0 = String(server.arg("m0")).toInt();
	ethernet.m1 = String(server.arg("m1")).toInt();
	ethernet.m2 = String(server.arg("m2")).toInt();
	ethernet.m3 = String(server.arg("m3")).toInt();
	ethernet.gat0 = String(server.arg("gat0")).toInt();
	ethernet.gat1 = String(server.arg("gat1")).toInt();
	ethernet.gat2 = String(server.arg("gat2")).toInt();
	ethernet.gat3 = String(server.arg("gat3")).toInt();
	ethernet.dns1_0 = String(server.arg("dns1_0")).toInt();
	ethernet.dns1_1 = String(server.arg("dns1_1")).toInt();
	ethernet.dns1_2 = String(server.arg("dns1_2")).toInt();
	ethernet.dns1_3 = String(server.arg("dns1_3")).toInt();
	ethernet.dns2_0 = String(server.arg("dns2_0")).toInt();
	ethernet.dns2_1 = String(server.arg("dns2_1")).toInt();
	ethernet.dns2_2 = String(server.arg("dns2_2")).toInt();
	ethernet.dns2_3 = String(server.arg("dns2_3")).toInt();
	ethernet.dhcpOn = String(server.arg("dhcpOn")).toInt();
	if(server.arg("siec") != ""){
		ethernet.siec = server.arg("siec");
		ethernet.haslo = server.arg("haslo");
	}
	DBG_OUTPUT_PORT.println(
	"ip0: " + String((int)ethernet.ip0) + 
	" ip1: " + String((int)ethernet.ip1) +
	" ip2: " + String((int)ethernet.ip2) +
	" ip3: " + String((int)ethernet.ip3) +
	" m0: " + String((int)ethernet.m0) +
	" m1: " + String((int)ethernet.m1) +
	" m2: " + String((int)ethernet.m2) +
	" m3: " + String((int)ethernet.m3) +
	" gat0: " + String((int)ethernet.gat0) +
	" gat1: " + String((int)ethernet.gat1) +
	" gat2: " + String((int)ethernet.gat2) +
	" gat3: " + String((int)ethernet.gat3) +
	" dns1_0: " + String((int)ethernet.dns1_0) +
	" dns1_1: " + String((int)ethernet.dns1_1) +
	" dns1_2: " + String((int)ethernet.dns1_2) +
	" dns1_3: " + String((int)ethernet.dns1_3) +
	" dns2_0: " + String((int)ethernet.dns2_0) +
	" dns2_1: " + String((int)ethernet.dns2_1) +
	" dns2_2: " + String((int)ethernet.dns2_2) +
	" dns2_3: " + String((int)ethernet.dns2_3) +
	" dhcpOn: " + String((int)ethernet.dhcpOn) +
	" siec: " + String(ethernet.siec) +
	" haslo: " + String(ethernet.haslo)
	);
	server.send(200, "text/json", "data send correctly!");
	saveEthernetConfig();
}
void systemControl(){
	//const char* POSIX_wsk = server.arg("POSIX_time").c_str();
	char * pKoniec;
	System_s.workMod = String(server.arg("workMod")).toInt();
	System_s.chandModeOff = String(server.arg("chandModeOff")).toInt();
	System_s.chandModeHumTimeEnd = String(server.arg("chandModeHumTimeEnd")).toInt();
	System_s.timeMod = String(server.arg("timeMod")).toInt();
	System_s.NTP_addr = String(server.arg("NTP_addr"));
	System_s.year = String(server.arg("year")).toInt();
	System_s.month = String(server.arg("month")).toInt();
	System_s.day = String(server.arg("day")).toInt();
	System_s.hour = String(server.arg("hour")).toInt();
	System_s.minute = String(server.arg("minute")).toInt();
	System_s.second = String(server.arg("second")).toInt();
	DBG_OUTPUT_PORT.println(
		"workMod: " + String((int)System_s.workMod) + 
		" chandModeOff: " + String((int)System_s.chandModeOff) +
		" chandModeHumTimeEnd: " + String((int)System_s.chandModeHumTimeEnd) +
		" timeMod: " + String((int)System_s.timeMod) +
		" NTP_addr: " + System_s.NTP_addr +
		" year: " + String((int)System_s.year) +
		" month: " + String((int)System_s.month) +
		" day: " + String((int)System_s.day) +
		" hour: " + String((int)System_s.hour) +
		" minute: " + String((int)System_s.minute) +
		" second: " + String((int)System_s.second)
		);
	server.send(200, "text/json", "data send correctly!");
	saveSystemConfig();
}

bool handleFileRead(String path) {
  DBG_OUTPUT_PORT.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.htm";
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
    if (SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

void handleFileUpload() {
  if (server.uri() != "/edit") return;
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    if (!filename.startsWith("/")) filename = "/" + filename;
    DBG_OUTPUT_PORT.print("handleFileUpload Name: "); DBG_OUTPUT_PORT.println(filename);
    fsUploadFile = SPIFFS.open(filename, "w");
    filename = String();
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    //DBG_OUTPUT_PORT.print("handleFileUpload Data: "); DBG_OUTPUT_PORT.println(upload.currentSize);
    if (fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile)
      fsUploadFile.close();
    DBG_OUTPUT_PORT.print("handleFileUpload Size: "); DBG_OUTPUT_PORT.println(upload.totalSize);
  }
}

void handleFileDelete() {
  if (server.args() == 0) return server.send(500, "text/plain", "BAD ARGS");
  String path = server.arg(0);
  DBG_OUTPUT_PORT.println("handleFileDelete: " + path);
  if (path == "/")
    return server.send(500, "text/plain", "BAD PATH");
  if (!SPIFFS.exists(path))
    return server.send(404, "text/plain", "FileNotFound");
  SPIFFS.remove(path);
  server.send(200, "text/plain", "");
  path = String();
}

void handleFileCreate() {
  if (server.args() == 0)
    return server.send(500, "text/plain", "BAD ARGS");
  String path = server.arg(0);
  DBG_OUTPUT_PORT.println("handleFileCreate: " + path);
  if (path == "/")
    return server.send(500, "text/plain", "BAD PATH");
  if (SPIFFS.exists(path))
    return server.send(500, "text/plain", "FILE EXISTS");
  File file = SPIFFS.open(path, "w");
  if (file)
    file.close();
  else
    return server.send(500, "text/plain", "CREATE FAILED");
  server.send(200, "text/plain", "");
  path = String();
}

void handleFileList() {
  if (!server.hasArg("dir")) {
    server.send(500, "text/plain", "BAD ARGS");
    return;
  }

  String path = server.arg("dir");
  DBG_OUTPUT_PORT.println("handleFileList: " + path);
  Dir dir = SPIFFS.openDir(path);
  path = String();

  String output = "[";
  while (dir.next()) {
    File entry = dir.openFile("r");
    if (output != "[") output += ',';
    bool isDir = false;
    output += "{\"type\":\"";
    output += (isDir) ? "dir" : "file";
    output += "\",\"name\":\"";
    output += String(entry.name()).substring(1);
    output += "\"}";
    entry.close();
  }

  output += "]";
  server.send(200, "text/json", output);
}

void Repeats(){
  Serial.println("15 second timer");         
}
void setup(void) {
	
  pinMode(ApClientPin, INPUT);     
  digitalWrite(ApClientPin, HIGH);  
  
  DBG_OUTPUT_PORT.begin(115200);
  DBG_OUTPUT_PORT.print("\n");
  DBG_OUTPUT_PORT.setDebugOutput(true);
  SPIFFS.begin();
  
  //wczytanie ustawień z plikow
  readTimesConfig();
  readTemperaturesConfig();
  readAirhumConfig();
  readHumConfig();
  readEthernetConfig();
  readSystemConfig();
  
  
  
  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      DBG_OUTPUT_PORT.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }
    DBG_OUTPUT_PORT.printf("\n");
  }
  //mesures
  setTime(8,29,0,1,1,11); // set time to Saturday 8:29:00am Jan 1 2011
  Alarm.timerRepeat(measure_intervall, measure_task);
  digitalClockDisplay();

  //WIFI INIT

  listNetworks();
  DBG_OUTPUT_PORT.println("ApClientPin: " + String(digitalRead(ApClientPin)));
  if(digitalRead(ApClientPin)){

	  while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		
		if(ethernet.dhcpOn){
			DBG_OUTPUT_PORT.print("DHCP enabled");
			
			DBG_OUTPUT_PORT.printf("Connecting to %s\n", ethernet.siec.c_str());
			if (String(WiFi.SSID()) != ethernet.siec) {
				WiFi.begin(ethernet.siec.c_str(), ethernet.haslo.c_str());
			}

		}else{
			
			IPAddress dns1(ethernet.dns1_0,ethernet.dns1_1,ethernet.dns1_2,ethernet.dns1_3);
			IPAddress dns2(ethernet.dns2_0,ethernet.dns2_1,ethernet.dns2_2,ethernet.dns2_3);
			// the router's gateway address:
			IPAddress gateway(ethernet.gat0,ethernet.gat1,ethernet.gat2,ethernet.gat3);
			// the subnet:

			IPAddress subnet(ethernet.m0,ethernet.m1,ethernet.m2,ethernet.m3);
			//the IP address is dependent on your network
			IPAddress ip(ethernet.ip0,ethernet.ip1,ethernet.ip2,ethernet.ip3);

			//config(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dns1, IPAddress dns2)
			WiFi.config(ip,gateway,subnet,dns1,dns2);
			DBG_OUTPUT_PORT.printf("Connecting to %s\n", ethernet.siec.c_str());
			if (String(WiFi.SSID()) != ethernet.siec) {
				WiFi.begin(ethernet.siec.c_str(), ethernet.haslo.c_str());
			}
		}

	  }		
	ethernet.ip3 = (0xFF000000 & WiFi.localIP()) >> 24;
	ethernet.ip2 = (0xFF0000 & WiFi.localIP()) >> 16;
	ethernet.ip1 = (0xFF00 & WiFi.localIP()) >> 8;
	ethernet.ip0 = 0xFF & WiFi.localIP();

	ethernet.m3 = (0xFF000000 & WiFi.subnetMask()) >> 24;
	ethernet.m2 = (0xFF0000 & WiFi.subnetMask()) >> 16;
	ethernet.m1 = (0xFF00 & WiFi.subnetMask()) >> 8;
	ethernet.m0 = 0xFF & WiFi.subnetMask(); 

	ethernet.gat3 = (0xFF000000 & WiFi.gatewayIP()) >> 24;
	ethernet.gat2 = (0xFF0000 & WiFi.gatewayIP()) >> 16;
	ethernet.gat1 = (0xFF00 & WiFi.gatewayIP()) >> 8;
	ethernet.gat0 = 0xFF & WiFi.gatewayIP();

	ethernet.dns1_3 = (0xFF000000 & WiFi.dnsIP(0)) >> 24;
	ethernet.dns1_2 = (0xFF0000 & WiFi.dnsIP(0)) >> 16;
	ethernet.dns1_1 = (0xFF00 & WiFi.dnsIP(0)) >> 8;
	ethernet.dns1_0 = 0xFF & WiFi.dnsIP(0);

	ethernet.dns2_3 = (0xFF000000 & WiFi.dnsIP(1)) >> 24;
	ethernet.dns2_2 = (0xFF0000 & WiFi.dnsIP(1)) >> 16;
	ethernet.dns2_1 = (0xFF00 & WiFi.dnsIP(1)) >> 8;
	ethernet.dns2_0 = 0xFF & WiFi.dnsIP(1);
	
	DBG_OUTPUT_PORT.println("");
	DBG_OUTPUT_PORT.print("Connected! IP address: ");
	DBG_OUTPUT_PORT.println(WiFi.localIP());
	  
	  
	  MDNS.begin(host);
	  DBG_OUTPUT_PORT.print("Open http://");
	  DBG_OUTPUT_PORT.print(host);
	  DBG_OUTPUT_PORT.println(".local/edit to see the file browser");
  }else{
	  Serial.print("Configuring access point...");
  // You can remove the password parameter if you want the AP to be open. 
	  WiFi.softAP("NodeMcu", "12345678");
 
      IPAddress myIP = WiFi.softAPIP();
      Serial.print("AP IP address: ");
      Serial.println(myIP);
	  //Żeby ustawić własne parametry acces pointu
	  //ESP8266WiFiAPClass::softAPConfig(IPAddress local_ip, IPAddress gateway, IPAddress subnet) 
	  			
		ethernet.ip3 = 1;
		ethernet.ip2 = 4;
		ethernet.ip1 = 168;
		ethernet.ip0 = 192;

		ethernet.m3 = 0;
		ethernet.m2 = 255;
		ethernet.m1 = 255;
		ethernet.m0 = 255; 

		ethernet.gat3 = 1;
		ethernet.gat2 = 4;
		ethernet.gat1 = 168;
		ethernet.gat0 = 192;

		ethernet.dns1_3 = 0;
		ethernet.dns1_2 = 0;
		ethernet.dns1_1 = 0;
		ethernet.dns1_0 = 0;
		
		ethernet.dns2_3 = 0;
		ethernet.dns2_2 = 0;
		ethernet.dns2_1 = 0;
		ethernet.dns2_0 = 0;
  
  }


  //SERVER INIT
  //Ustawienia sterowania
  server.on("/timSter", HTTP_POST, timSterControl);
  server.on("/temperature", HTTP_POST, temperatureControl);
  server.on("/airHum", HTTP_POST, airHumControl);
  server.on("/Hum", HTTP_POST, HumControl);
  server.on("/ethernet", HTTP_POST, ethernetControl);
  server.on("/system", HTTP_POST, systemControl);
  //list directory
  server.on("/list", HTTP_GET, handleFileList);
  //load editor
  server.on("/edit", HTTP_GET, []() {
    if (!handleFileRead("/edit.htm")) server.send(404, "text/plain", "FileNotFound");
  });
  //create file
  server.on("/edit", HTTP_PUT, handleFileCreate);
  //delete file
  server.on("/edit", HTTP_DELETE, handleFileDelete);
  //first callback is called after the request has ended with all parsed arguments
  //second callback handles file uploads at that location
  server.on("/edit", HTTP_POST, []() {
    server.send(200, "text/plain", "");
  }, handleFileUpload);

  //called when the url is not defined here
  //use it to load content from SPIFFS
  server.onNotFound([]() {
    if (!handleFileRead(server.uri()))
      server.send(404, "text/plain", "FileNotFound");
  });

  //get heap status, analog input value and all GPIO statuses in one json call
  server.on("/all", HTTP_GET, []() {
    
    String json = "{";
    json += "\"heap\":" + String(ESP.getFreeHeap());
    json += ",\"analog\":" + String(analogRead(A0));
    json += ",\"gpio\":" + String((uint32_t)(((GPI | GPO) & 0xFFFF) | ((GP16I & 0x01) << 16)));
    json += ",\"meas_cnt\":" + String(temps_cnt);
    json += ",\"temperatura\":"  + String(temp);
    json += ",\"year\":"  + String(year());
    json += ",\"month\":"  + String(month());
    json += ",\"day\":"  + String(day());
    json += ",\"second\":"  + String(second());
    json += ",\"minute\":"  + String(minute());
    json += ",\"hour\":"  + String(hour());
	json += ",\"measure_intervall\":" + String(measure_intervall * POMIARY_USREDNIANE);
	for(i=0;i<=5;i++){
		json += ",\"last_meas_time" +String(i)+"\":" + String(last_meas_time[i]);
	}
    for(i = 0;i<=wielokosc_bufora_pomiarow;i++){
        json += ",\"t" + String(i) + "\":" + String(temps[i]);
      }

    json += "}";
    server.send(200, "text/json", json);
    json = String();
  });
    server.on("/timSter", HTTP_GET, []() {
		
		String json = "{";
		json += "\"tim1Max\":" + String(times.tim1Max);
		json += ",\"tim1Min\":" + String(times.tim1Min);
		json += ",\"tim1\":" + String((int)times.tim1);
		json += ",\"tim2Max\":" + String(times.tim2Max);
		json += ",\"tim2Min\":" + String(times.tim2Min);
		json += ",\"tim2\":" + String((int)times.tim2);
		json += ",\"tim3Max\":" + String(times.tim3Max);
		json += ",\"tim3Min\":" + String(times.tim3Min);
		json += ",\"tim3\":" + String((int)times.tim3);
		json += "}";
		server.send(200, "text/json", json);
		json = String();
	});
	
	server.on("/temperature", HTTP_GET, []() {
		
		String json = "{";
		json += "\"tempMax\":" + String((int)temperature.tempMax);
		json += ",\"tempMaxOn\":" + String((int)temperature.tempMaxOn);
		json += ",\"tempMin\":" + String((int)temperature.tempMin);
		json += ",\"tempMinOn\":" + String((int)temperature.tempMinOn);
		json += ",\"DeltaT\":" + String((int)temperature.DeltaT);
		json += ",\"DeltaTim\":" + String((int)temperature.DeltaTim);
		json += ",\"DeltaRelayTime\":" + String((int)temperature.DeltaRelayTime);
		json += ",\"DeltaTOn\":" + String((int)temperature.DeltaTOn);
		json += "}";
		server.send(200, "text/json", json);
		json = String();
	});    
	server.on("/airhum", HTTP_GET, []() {
		
		String json = "{";
		json += "\"wilgPowMax\":" + String((int)airHum.wilgPowMax);
		json += ",\"wilgPowMaxOn\":" + String((int)airHum.wilgPowMaxOn);
		json += ",\"wilgPowMin\":" + String((int)airHum.wilgPowMin);
		json += ",\"wilgPowMinOn\":" + String((int)airHum.wilgPowMinOn);
		json += ",\"DeltaWilgPow\":" + String((int)airHum.DeltaWilgPow);
		json += ",\"DeltaWilgPowTim\":" + String(airHum.DeltaWilgPowTim);
		json += ",\"DeltaWilgPowRelayTim\":" + String(airHum.DeltaWilgPowRelayTim);
		json += ",\"DeltaWilgPowOn\":" + String((int)airHum.DeltaWilgPowOn);
		json += "}";
		server.send(200, "text/json", json);
		json = String();
	});    
	server.on("/hum", HTTP_GET, []() {
		String json = "{";
		json += "\"wilgMax\":" + String((int)hum.wilgMax);
		json += ",\"wilgMaxOn\":" + String((int)hum.wilgMaxOn);
		json += ",\"wilgMin\":" + String((int)hum.wilgMin);
		json += ",\"wilgMinOn\":" + String((int)hum.wilgMinOn);
		json += ",\"DeltaWilg\":" + String((int)hum.DeltaWilg);
		json += ",\"DeltaWilgTim\":" + String(hum.DeltaWilgTim);
		json += ",\"DeltaWilgRelayTim\":" + String(hum.DeltaWilgRelayTim);
		json += ",\"DeltaWilgOn\":" + String((int)hum.DeltaWilgOn);		
		json += "}";
		server.send(200, "text/json", json);
		json = String();
	});    
	server.on("/ethernet", HTTP_GET, []() {
		byte mac[6];   
		
		String json = "{";
		json += "\"ip0\":" + String((int)ethernet.ip0);
		json += ",\"ip1\":" + String((int)ethernet.ip1);
		json += ",\"ip2\":" + String((int)ethernet.ip2);
		json += ",\"ip3\":" + String((int)ethernet.ip3);
		json += ",\"m0\":" + String((int)ethernet.m0);
		json += ",\"m1\":" + String((int)ethernet.m1);
		json += ",\"m2\":" + String((int)ethernet.m2);
		json += ",\"m3\":" + String((int)ethernet.m3);
		
		WiFi.macAddress(mac);
		
		json += ",\"mac0\":\"" + String(mac[5],HEX) +"\"";
		json += ",\"mac1\":\"" + String(mac[4],HEX) +"\"";
		json += ",\"mac2\":\"" + String(mac[3],HEX) +"\"";
		json += ",\"mac3\":\"" + String(mac[3],HEX) +"\"";
		json += ",\"mac4\":\"" + String(mac[2],HEX) +"\"";
		json += ",\"mac5\":\"" + String(mac[1],HEX) +"\"";
		
		json += ",\"gat0\":" + String((int)ethernet.gat0);
		json += ",\"gat1\":" + String((int)ethernet.gat1);
		json += ",\"gat2\":" + String((int)ethernet.gat2);
		json += ",\"gat3\":" + String((int)ethernet.gat3);
		json += ",\"dns1_0\":" + String((int)ethernet.dns1_0);
		json += ",\"dns1_1\":" + String((int)ethernet.dns1_1);
		json += ",\"dns1_2\":" + String((int)ethernet.dns1_2);
		json += ",\"dns1_3\":" + String((int)ethernet.dns1_3);
		json += ",\"dns2_0\":" + String((int)ethernet.dns2_0);
		json += ",\"dns2_1\":" + String((int)ethernet.dns2_1);
		json += ",\"dns2_2\":" + String((int)ethernet.dns2_2);
		json += ",\"dns2_3\":" + String((int)ethernet.dns2_3);
		json += ",\"dhcpOn\":" + String((int)ethernet.dhcpOn);
		json += listNetworks();
		
		json += "}";
		server.send(200, "text/json", json);
		json = String();
	});    
	server.on("/system", HTTP_GET, []() {
		
		String json = "{";
		json += "\"workMod\":" + String((int)System_s.workMod);
		json += ",\"chandModeOff\":" + String((int)System_s.chandModeOff);
		json += ",\"timeMod\":" + String((int)System_s.timeMod);
		json += ",\"chandModeHumTimeEnd\":" + String((int)System_s.chandModeHumTimeEnd);

		json += ",\"year\":" + String((int)System_s.year);
		json += ",\"month\":" + String((int)System_s.month);
		json += ",\"day\":" + String((int)System_s.day);
		json += ",\"hour\":" + String((int)System_s.hour);
		json += ",\"minute\":" + String((int)System_s.minute);
		json += ",\"second\":" + String((int)System_s.second);

		json += ",\"NTP_addr\":\"" + System_s.NTP_addr + "\"";
		json += "}";
		server.send(200, "text/json", json);
		json = String();
	});
	if(digitalRead(ApClientPin)){
		for(i = 0;((i>5)||(NTP_found > 1)) == 0;i++){
		  Serial.println("Starting UDP");
		  Udp.begin(localPort);
		  Serial.print("Local port: ");
		  Serial.println(Udp.localPort());
		  Serial.println("waiting for sync");
		  setSyncProvider(getNtpTime);
		  setSyncInterval(300);
		} 
	}
  server.begin();
  DBG_OUTPUT_PORT.println("HTTP server started");
}
time_t prevDisplay = 0; // when the digital clock was displayed
void loop(void) {
  if (timeStatus() != timeNotSet) {
    if (now() != prevDisplay) { //update the display only if time has changed
      prevDisplay = now();
      //digitalClockDisplay();
    }
  }
  
  server.handleClient();
  Alarm.delay(1); // wait one second between clock display
}

/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP server's ip address

  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  // get a random server from the pool
  WiFi.hostByName(System_s.NTP_addr.c_str(), ntpServerIP);
  Serial.print(System_s.NTP_addr.c_str());
  Serial.print(": ");
  Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
	  
	  NTP_found++;
	  
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
void digitalClockDisplay()
{
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(".");
  Serial.print(month());
  Serial.print(".");
  Serial.print(year());
  Serial.println();
}

void printDigits(int digits)
{
  // utility for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void saveTimesConfig(void){
	File f = SPIFFS.open("/timesConf.txt", "w+");
	if (!f) {
		Serial.println("file open failed");
	}else{
		//ustawienia czasow
		f.print(String(times.tim1Max) + ";");
		f.print(String(times.tim1Min) + ";");
		f.print(String((int)times.tim1) + ";");
		f.print(String(times.tim2Max) + ";");
		f.print(String(times.tim2Min) + ";");
		f.print(String((int)times.tim2) + ";");
		f.print(String(times.tim3Max) + ";");
		f.print(String(times.tim3Min) + ";");
		f.print(String((int)times.tim3) + ";");
		f.println();
	}
	f.close();
}

void readTimesConfig(void){
	File f = SPIFFS.open("/timesConf.txt", "r+");
	char charBuf[500];
	char korektor[] = ";";
	char * schowek;
	String settings_str;
	if (!f) {
		Serial.println("file open failed");
	}else{
		settings_str = f.readStringUntil('\n');
		Serial.println(settings_str);
		settings_str.toCharArray(charBuf, 500);
		Serial.println(charBuf);
		schowek = strtok( charBuf , korektor );
		Serial.println(schowek);
		times.tim1Max = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		times.tim1Min = atoi(schowek);
		Serial.println(schowek);
		schowek = strtok( NULL , korektor );
		times.tim1 = atoi(schowek);
		Serial.println(schowek);
		schowek = strtok( NULL , korektor );
		times.tim2Max = atoi(schowek);
		Serial.println(schowek);
		schowek = strtok( NULL , korektor );
		times.tim2Min = atoi(schowek);
		Serial.println(schowek);
		schowek = strtok( NULL , korektor );
		times.tim2 = atoi(schowek);
		Serial.println(schowek);
		schowek = strtok( NULL , korektor );
		times.tim3Max = atoi(schowek);
		Serial.println(schowek);
		schowek = strtok( NULL , korektor );
		times.tim3Min = atoi(schowek);
		Serial.println(schowek);
		schowek = strtok( NULL , korektor );
		times.tim3 = atoi(schowek);
		Serial.println(schowek);
	}
	f.close();
}
void saveTemperatureConfig(void){
	File f = SPIFFS.open("/tempConf.txt", "w+");
	if (!f) {
		Serial.println("file open failed");
	}else{
		//ustawienia czasow
		f.print(String((int)temperature.tempMax) + ";");
		f.print(String((int)temperature.tempMaxOn) + ";");
		f.print(String((int)temperature.tempMin) + ";");
		f.print(String((int)temperature.tempMinOn) + ";");
		f.print(String((int)temperature.DeltaT) + ";");
		f.print(String(temperature.DeltaTim) + ";");
		f.print(String(temperature.DeltaRelayTime) + ";");
		f.print(String((int)temperature.DeltaTOn) + ";");
		f.println();
	}
	f.close();
}
void readTemperaturesConfig(void){
	File f = SPIFFS.open("/tempConf.txt", "r+");
	char charBuf[500];
	char korektor[] = ";";
	char * schowek;
	String settings_str;
	if (!f) {
		Serial.println("file open failed");
	}else{
		settings_str = f.readStringUntil('\n');
		Serial.println(settings_str);
		settings_str.toCharArray(charBuf, 500);
		Serial.println(charBuf);
		schowek = strtok( charBuf , korektor );
		Serial.println(schowek);
		temperature.tempMax = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		temperature.tempMaxOn = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		temperature.tempMin = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		temperature.tempMinOn = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		temperature.DeltaT = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		temperature.DeltaTim = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		temperature.DeltaRelayTime = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		temperature.DeltaTOn = atoi(schowek);
	}
	f.close();
}

void saveAirhumConfig(void){
	File f = SPIFFS.open("/airhumConf.txt", "w+");
	if (!f) {
		Serial.println("file open failed");
	}else{
		//ustawienia czasow
		f.print(String((int)airHum.wilgPowMax) + ";");
		f.print(String((int)airHum.wilgPowMaxOn) + ";");
		f.print(String((int)airHum.wilgPowMin) + ";");
		f.print(String((int)airHum.wilgPowMinOn) + ";");
		f.print(String((int)airHum.DeltaWilgPow) + ";");
		f.print(String(airHum.DeltaWilgPowTim) + ";");
		f.print(String(airHum.DeltaWilgPowRelayTim) + ";");
		f.print(String((int)airHum.DeltaWilgPowOn) + ";");
		f.println();
	}
	f.close();
}

void readAirhumConfig(void){
	File f = SPIFFS.open("/airhumConf.txt", "r+");
	char charBuf[500];
	char korektor[] = ";";
	char * schowek;
	String settings_str;
	if (!f) {
		Serial.println("file open failed");
	}else{
		settings_str = f.readStringUntil('\n');
		Serial.println(settings_str);
		settings_str.toCharArray(charBuf, 500);
		Serial.println(charBuf);
		schowek = strtok( charBuf , korektor );
		Serial.println(schowek);
		airHum.wilgPowMax = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		airHum.wilgPowMaxOn = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		airHum.wilgPowMin = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		airHum.wilgPowMinOn = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		airHum.DeltaWilgPow = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		airHum.DeltaWilgPowTim = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		airHum.DeltaWilgPowRelayTim = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		airHum.DeltaWilgPowOn = atoi(schowek);
	}
	f.close();
}

void saveHumConfig(void){
	File f = SPIFFS.open("/humConf.txt", "w+");
	if (!f) {
		Serial.println("file open failed");
	}else{
		//ustawienia czasow
		f.print(String((int)hum.wilgMax) + ";");
		f.print(String((int)hum.wilgMaxOn) + ";");
		f.print(String((int)hum.wilgMin) + ";");
		f.print(String((int)hum.wilgMinOn) + ";");
		f.print(String((int)hum.DeltaWilg) + ";");
		f.print(String(hum.DeltaWilgTim) + ";");
		f.print(String(hum.DeltaWilgRelayTim) + ";");
		f.print(String((int)hum.DeltaWilgOn) + ";");
		f.println();
	}
	f.close();
}

void readHumConfig(void){
	File f = SPIFFS.open("/humConf.txt", "r+");
	char charBuf[500];
	char korektor[] = ";";
	char * schowek;
	String settings_str;
	if (!f) {
		Serial.println("file open failed");
	}else{
		settings_str = f.readStringUntil('\n');
		Serial.println(settings_str);
		settings_str.toCharArray(charBuf, 500);
		Serial.println(charBuf);
		schowek = strtok( charBuf , korektor );
		Serial.println(schowek);
		hum.wilgMax = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		hum.wilgMaxOn = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		hum.wilgMin = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		hum.wilgMinOn = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		hum.DeltaWilg = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		hum.DeltaWilgTim = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		hum.DeltaWilgRelayTim = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		hum.DeltaWilgOn = atoi(schowek);
	}
	f.close();
}

void saveEthernetConfig(void){
	File f = SPIFFS.open("/ethernetConf.txt", "w+");
	if (!f) {
		Serial.println("file open failed");
	}else{
		//ustawienia czasow
		f.print(String((int)ethernet.ip0) + ";");
		f.print(String((int)ethernet.ip1) + ";");
		f.print(String((int)ethernet.ip2) + ";");
		f.print(String((int)ethernet.ip3) + ";");
		
		f.print(String((int)ethernet.m0) + ";");
		f.print(String((int)ethernet.m1) + ";");
		f.print(String((int)ethernet.m2) + ";");
		f.print(String((int)ethernet.m3) + ";");
		
		f.print(String((int)ethernet.gat0) + ";");
		f.print(String((int)ethernet.gat1) + ";");
		f.print(String((int)ethernet.gat2) + ";");
		f.print(String((int)ethernet.gat3) + ";");
		
		f.print(String((int)ethernet.dns1_0) + ";");
		f.print(String((int)ethernet.dns1_1) + ";");
		f.print(String((int)ethernet.dns1_2) + ";");
		f.print(String((int)ethernet.dns1_3) + ";");
		
		f.print(String((int)ethernet.dns2_0) + ";");
		f.print(String((int)ethernet.dns2_1) + ";");
		f.print(String((int)ethernet.dns2_2) + ";");
		f.print(String((int)ethernet.dns2_3) + ";");
		
		f.print(String((int)ethernet.dhcpOn) + ";");
		
		f.print(ethernet.siec + ";");
		f.print(ethernet.haslo + ";");

		f.println();
	}
	f.close();
}

void readEthernetConfig(void){
	File f = SPIFFS.open("/ethernetConf.txt", "r+");
	char charBuf[500];
	char korektor[] = ";";
	char * schowek;
	String settings_str;
	if (!f) {
		Serial.println("file open failed");
	}else{
		settings_str = f.readStringUntil('\n');
		Serial.println(settings_str);
		settings_str.toCharArray(charBuf, 500);
		Serial.println(charBuf);
		schowek = strtok( charBuf , korektor );
		Serial.println(schowek);
		ethernet.ip0 = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		ethernet.ip1 = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		ethernet.ip2 = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		ethernet.ip3 = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		ethernet.m0 = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		ethernet.m1 = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		ethernet.m2 = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		ethernet.m3 = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		ethernet.gat0 = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		ethernet.gat1 = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		ethernet.gat2 = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		ethernet.gat3 = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		ethernet.dns1_0 = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		ethernet.dns1_1 = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		ethernet.dns1_2 = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		ethernet.dns1_3 = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		ethernet.dns2_0 = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		ethernet.dns2_1 = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		ethernet.dns2_2 = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		ethernet.dns2_3 = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		ethernet.dhcpOn = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		ethernet.siec = schowek;
			
		schowek = strtok( NULL , korektor );
		ethernet.haslo = schowek;

	}
	f.close();
}

void saveSystemConfig(void){
	File f = SPIFFS.open("/systemConf.txt", "w+");
	if (!f) {
		Serial.println("file open failed");
	}else{
		//ustawienia czasow
		f.print(String((int)System_s.workMod) + ";");
		f.print(String((int)System_s.chandModeOff) + ";");
		f.print(String((int)System_s.chandModeHumTimeEnd) + ";");
		f.print(String((int)System_s.timeMod) + ";");
		f.print(String(System_s.NTP_addr) + ";");
		f.print(String(System_s.year) + ";");
		f.print(String((int)System_s.month) + ";");
		f.print(String((int)System_s.day) + ";");
		f.print(String((int)System_s.hour) + ";");
		f.print(String((int)System_s.minute) + ";");
		f.print(String((int)System_s.second) + ";");
		
		f.println();
	}
	f.close();
}

void readSystemConfig(void){
	File f = SPIFFS.open("/systemConf.txt", "r+");
	char charBuf[500];
	char korektor[] = ";";
	char * schowek;
	String settings_str;
	if (!f) {
		Serial.println("file open failed");
	}else{
		settings_str = f.readStringUntil('\n');
		Serial.println(settings_str);
		settings_str.toCharArray(charBuf, 500);
		Serial.println(charBuf);
		schowek = strtok( charBuf , korektor );
		Serial.println(schowek);
		System_s.workMod = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		System_s.chandModeOff = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		System_s.chandModeHumTimeEnd = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		System_s.timeMod = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		System_s.NTP_addr = String(schowek);
		
		schowek = strtok( NULL , korektor );
		System_s.year = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		System_s.month = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		System_s.day = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		System_s.hour = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		System_s.minute = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		System_s.second = atoi(schowek);

	}
	f.close();
}

String listNetworks() {
  String wrless;
  // scan for nearby networks:
  Serial.println("** Scan Networks **");
  int numSsid = WiFi.scanNetworks();
  if (numSsid == -1) {
    Serial.println("Couldn't get a wifi connection");
    while (true);
  }

  // print the list of networks seen:
  Serial.print("number of available networks:");
  Serial.println(numSsid);
  wrless += ",\"numOfWiFi\":" + String(numSsid);
  // print the network number and name for each network found:
  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    Serial.print(thisNet);
    Serial.print(") ");
    Serial.print(WiFi.SSID(thisNet));
	wrless += ",\"name"+String(thisNet)+"\":\"" + String(WiFi.SSID(thisNet)) +"\"";
    Serial.print("\tSignal: ");
	wrless += ",\"signal"+String(thisNet)+"\":" + String(WiFi.RSSI(thisNet));
    Serial.print(WiFi.RSSI(thisNet));
    Serial.print(" dBm");
    Serial.print("\tEncryption:");
	wrless += ",\"encryption" + String(thisNet) +"\":\"" + printEncryptionType(WiFi.encryptionType(thisNet))+"\"";
  }
  return wrless;
}

String printEncryptionType(int thisType) {
  String passType;
  // read the encryption type and print out the name:
  switch (thisType) {
    case ENC_TYPE_WEP:
      Serial.println("WEP");
	  passType = "WEP";
	  return(passType);
      break;
    case ENC_TYPE_TKIP:
      Serial.println("WPA");
	  passType = "WPA";
	  return(passType);
      break;
    case ENC_TYPE_CCMP:
      passType = "WPA2";
	  return(passType);
      break;
    case ENC_TYPE_NONE:
      Serial.println("None");
	  passType = "None";
	  return(passType);
      break;
    case ENC_TYPE_AUTO:
      Serial.println("Auto");
	  passType = "Auto";
	  return(passType);
      break;
  }
}