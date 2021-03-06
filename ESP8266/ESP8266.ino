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

#include "esp8266.h"

//#define DBG_OUTPUT_PORT Serial




//Control structures
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


// NTP Servers:
//static const char ntpServerName[] = "us.pool.ntp.org";

WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets

float temp;               //przechowujÄ™ chwilowÄ… wartoĹ›Ä‡ temp
int last_meas_time[6];

char NTP_found;

ExprEval eval;	//Klasa parsujÄ…ca rĂłwnania matematyczne

DS1302RTC RTC(RTC_RESET, RTC_CLK, RTC_DATA);

void measure_task() {  
  for(int i = 0 ; i < MAX_NUM_SEN + NUM_OF_INTERNAL_SENSOR ; i++){
	if(i < MAX_NUM_SEN){
		obPointArr[i]->readMeasurments(0);
	}else{
		obPointArr[i]->readMeasurments(1);
	}
  }
}
File filescript;
void scriptTask(void){
	DB2("scriptTask, przed wczytaniem skryptu: "+String(ESP.getFreeHeap()));
	File filescript = SPIFFS.open(System_s.workingScript, "r+");
	DB2("Po wczytaniu: "+String(ESP.getFreeHeap()));
	String script_line;
	if (!filescript) {
		DB1("file open failed");
	}else{
		//DB2("Przed przel. zmien. 123: "+String(ESP.getFreeHeap()));
		externVir.reloadVirValues();
		//DB2("Po przeladowaniu zmiennych123: "+String(ESP.getFreeHeap()));
		script_line = filescript.readStringUntil('\n');
		eval.Count((EVAL_CHAR*)script_line.c_str());
		while(script_line != ""){
			script_line = filescript.readStringUntil('\n');
			String(eval.Count((EVAL_CHAR*)script_line.c_str()));	
		}

	}
	filescript.close();
	DB2("Po zamknieciu pliku: "+String(ESP.getFreeHeap()));
	return;
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
	int server.args() - liczba argumentĂłw od ajaxa
	String arg(String name);        // get request argument value by name
	String arg(int i);              // get request argument value by number
	String argName(int i);          // get request argument name by number

PrzykĹ‚adowe zapytanie ajaxa z danymi
		$.ajax({
		  method: "POST",
		  url: "cont",
		  data: { name: "John", location: "Boston" }
		})
		  .done(function( msg ) {
			alert( "Data Saved: " + msg );
		  });
*/

void readAjaxStructFromFile(void){
	String tmpStr;
	File f = SPIFFS.open("/"+server.arg("fileName")+".sav", "r+");
	if (!f) {
		server.send(200, "File open failed!", tmpStr);
		DB1("file open failed");
	}else{
		tmpStr += "{\"contentStr\":\"";
		tmpStr += f.readStringUntil('\n');
		DB1(f.readStringUntil('\n'));
		tmpStr += "\"}";
	
		server.send(200, "text/json", tmpStr);
	}
	f.close();
}
void saveAjaxStructToFile(void){
	String tmpStr;
	int allArgs;
	allArgs = server.args();
	DB1("Liczba argumentow: " + String(allArgs));
	for(int i = 0; i<allArgs;i++){
		DB1("Arg name: " + server.argName(i) + " = " + server.arg(i));
	}
	File f = SPIFFS.open("/"+server.arg("fileName")+".sav", "w+");
	if (!f) {
		server.send(200, "text/json", "\"Error while saving data!\"");
		DB1("file open failed");
	}else{
		for(int i = 0; i<allArgs;i++){
			tmpStr += String(server.argName(i))+"#:";
			tmpStr += String(server.arg(i))+"#$";
			server.send(200, "text/json", "\"Data send correctly!\"");
		}
		f.print(tmpStr);
	}
	generateAutoScript();
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
	DB1(
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
	server.send(200, "text/json", "\"Data send correctly!\"");
	saveEthernetConfig();
}
void systemControl(){
	//const char* POSIX_wsk = server.arg("POSIX_time").c_str();
	char * pKoniec;
	System_s.timeMod = String(server.arg("timeMod")).toInt();
	System_s.NTP_addr = String(server.arg("NTP_addr"));
	System_s.year = String(server.arg("year")).toInt();
	System_s.month = String(server.arg("month")).toInt();
	System_s.day = String(server.arg("day")).toInt();
	System_s.hour = String(server.arg("hour")).toInt();
	System_s.minute = String(server.arg("minute")).toInt();
	System_s.second = String(server.arg("second")).toInt();
	System_s.timeZone = String(server.arg("timeZone")).toInt();
	System_s.measInt = String(server.arg("measInt")).toInt();
	DB1(
		" timeMod: " + String((int)System_s.timeMod) +
		" NTP_addr: " + System_s.NTP_addr +
		" year: " + String((int)System_s.year) +
		" month: " + String((int)System_s.month) +
		" day: " + String((int)System_s.day) +
		" hour: " + String((int)System_s.hour) +
		" minute: " + String((int)System_s.minute) +
		" second: " + String((int)System_s.second)+
		" timeZone: " + String((int)System_s.timeZone)+
		" measInt: " + String((int)System_s.measInt)
		);
	server.send(200, "text/json", "\"Data send correctly!\"");
	saveSystemConfig();
}
void scriptSettings(){
	System_s.workingScript = String(server.arg("workingScript"));
	System_s.relayScriptTime = String(server.arg("relayScriptTime")).toInt();
	DB1(
		"workingScript: " + String(System_s.workingScript) + 
		" relayScriptTime: " + String(System_s.relayScriptTime)
		);
	server.send(200, "text/json", "\"Data send correctly!\"");
	saveSystemConfig();	
}
void getMeasuredConfigAjax(){
	String output = "";
	
	String allTypes = "";
	
	for(int i = 0; i<MAX_NUM_SEN + NUM_OF_INTERNAL_SENSOR; i++){
		allTypes += obPointArr[i]->getMeasuredConfigAjax();
	}
	
	output += "{\"intervall\":" + String(System_s.measInt);
	output += ",\"allMeasuredTypes\":\"" + String(allTypes)+"\"";
	output += "}";
	server.send(200, "text/json", output);
}

void parseEquation(){
	String equation;
	equation = String(server.arg("Equation"));
	server.send(200, "text/json", String(eval.Count((EVAL_CHAR*)equation.c_str())));
}
bool handleFileRead(String path) {
  DB1("handleFileRead: " + path);
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
    DB1("handleFileUpload Name: "); DB1(filename);
    fsUploadFile = SPIFFS.open(filename, "w");
    filename = String();
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    //DB1("handleFileUpload Data: "); DB1(upload.currentSize);
    if (fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile)
      fsUploadFile.close();
    DB1("handleFileUpload Size: "); DB1(upload.totalSize);
  }
}

void handleFileDelete() {
  if (server.args() == 0) return server.send(500, "text/plain", "BAD ARGS");
  String path = server.arg(0);
  DB1("handleFileDelete: " + path);
  if (path == "/")
    return server.send(500, "text/plain", "BAD PATH");
  if (!SPIFFS.exists(path))
    return server.send(404, "text/plain", "FileNotFound");
  SPIFFS.remove(path);
  server.send(200, "text/plain", "");
  path = String();
}
void fileDelete() {
  if (server.args() == 0) return server.send(500, "text/plain", "BAD ARGS");
  String path = server.arg(0);
  DB1("handleFileDelete: " + path);
  if (path == "/")
    return server.send(500, "text/plain", "BAD PATH");
  if (!SPIFFS.exists(path))
    return server.send(404, "text/plain", "FileNotFound");
  SPIFFS.remove(path);
  server.send(200, "text/plain", "");
  path = String();
}
void reset(void){
	//ESP.reset();
	ESP.restart();
}
void handleFileCreate() {
  if (server.args() == 0)
    return server.send(500, "text/plain", "BAD ARGS");
  String path = server.arg(0);
  DB1("handleFileCreate: " + path);
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
  DB1("handleFileList: " + path);
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
    output += String(entry.name());
    output += "\"}";
    entry.close();
  }

  output += "]";
  server.send(200, "text/json", output);
}
String scriptName;
void getScriptLine(){
	saveScriptFile(server.arg("scriptLine"));
	server.send(200, "text/json", scriptName);
}
void getScriptName(){
	scriptName = String(server.arg("scriptName"));
	server.send(200, "text/json", scriptName);
}
void getScriptContent(){
	File f = SPIFFS.open(String(server.arg("scriptName")), "r+");
	String output;
	if (!f) {
		DB1("file open failed");
	}else{
		output = f.readString();
	}
	f.close();
	server.send(200, "text/json", output);	
}
void getScriptsNames(){
	String path = "/";
	Dir dir = SPIFFS.openDir(path);
	String output = "[";
	while (dir.next()) {
		File entry = dir.openFile("r");
		if(strstr(String(entry.name()).c_str(),".scr")){
			if (output != "[") output += ',';
				output += "{\"name\":\"/";
				output += String(entry.name()).substring(1);
				output += "\"}";
		}	
	}
	output += "]";
	server.send(200, "text/json", output);
}


void getMeasureValues(){
	String output = "[";
	//if(output == "["){
		output += obPointArr[atoi(server.arg("numb").c_str())]->getAjaxMeasuredValues(0,server.arg("measName"),atoi(server.arg("Cmin").c_str()),atoi(server.arg("Cmax").c_str()));
	//}else{
	//	output += obPointArr[atoi(server.arg("numb").c_str())]->getAjaxMeasuredValues(1,server.arg("measName"),atoi(server.arg("Cmin").c_str()),atoi(server.arg("Cmax").c_str()));
	//}
	
	output += "]";
	server.send(200, "text/json", output);
}
void Repeats(){
  DB1("15 second timer");         
}

uint8_t systemIsLoaded = 0;
tmElements_t tm;
unsigned long start, finished, elapsed;
void setup(void) {
	
	
	
	//Checking type of init AP or Client dependly of pinstate
	pinMode(ApClientPin, INPUT);     
	digitalWrite(ApClientPin, HIGH);  
	//Relay 0 onboard init
	pinMode(RELAY0, OUTPUT);     
	digitalWrite(RELAY0, LOW);  
	
	OUT.begin(BAUDRATE);
	OUT.print("\n");
	OUT.setDebugOutput(true);
	SPIFFS.begin();

	//wczytanie ustawieĹ„ z plikow
	readEthernetConfig();
	readSystemConfig();
	

    displayInit();
  #ifdef DEBUG
  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      DB1("FS File: "+String(fileName)+", size: " + String(formatBytes(fileSize)));
    }
    DB1("\n");
  }
  #endif
  //mesures

  RTC.read(tm);
  setTime(tm.Hour,tm.Minute,tm.Second,tm.Day,tm.Month,(tm.Year - 30));
  //WIFI INIT

  listNetworks();

  tryToConnect();
  initRC();
  //SERVER INIT
  //Zapis odczytanej struktury ajax do pliku
  server.on("/saveAjaxStructToFile", HTTP_POST, saveAjaxStructToFile);
  server.on("/readAjaxStructFromFile", HTTP_GET, readAjaxStructFromFile);
  
  server.on("/getScriptContent", HTTP_GET, getScriptContent);
  //delete
  server.on("/deleteFile", HTTP_POST, fileDelete);
  //Ustawienia sterowania
  server.on("/ethernet", HTTP_POST, ethernetControl);
  server.on("/system", HTTP_POST, systemControl);
  server.on("/scriptSettings", HTTP_POST, scriptSettings);
  //rownanie matematyczne do parsera
  server.on("/parse", HTTP_POST, parseEquation);
  //Zapisz nazwę pliku
  server.on("/getScriptName", HTTP_POST, getScriptName);
  //Zapisz zawartość pliku
  server.on("/getScriptLine", HTTP_POST, getScriptLine);
  //Pobierz listę plików
  server.on("/getScriptsNames", HTTP_GET, getScriptsNames);
  server.on("/measureValues", HTTP_GET, getMeasureValues);
  server.on("/reset", HTTP_GET, reset);
  //list directory
  server.on("/list", HTTP_GET, handleFileList);
  server.on("/getMeasuredConfigAjax", HTTP_GET, getMeasuredConfigAjax);
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
		ESP.wdtFeed();
      }

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
		readSystemConfig();
		
		String json = "{";
		json += "\"timeMod\":" + String((int)System_s.timeMod);

		json += ",\"year\":" + String((int)year());
		json += ",\"month\":" + String((int)month());
		json += ",\"day\":" + String((int)day());
		json += ",\"hour\":" + String((int)hour());
		json += ",\"minute\":" + String((int)minute());
		json += ",\"second\":" + String((int)second());

		json += ",\"timeZone\":" + String((int)System_s.timeZone);
		json += ",\"measInt\":" + String(System_s.measInt);
		
		json += ",\"NTP_addr\":\"" + System_s.NTP_addr + "\"";
		json += "}";
		server.send(200, "text/json", json);
		json = String();
	});
	server.on("/scriptSettings", HTTP_GET, []() {
		
		String json = "{";
		json += "\"workingScript\":\"" + System_s.workingScript +"\"";
		json += ",\"relayScriptTime\":" + String(System_s.relayScriptTime);

		json += "}";
		server.send(200, "text/json", json);
		json = String();
	});
	
	server.on("/systemVirablesValues", HTTP_GET, []() {
		server.send(200, "text/json", externVir.generateValNamePairString());
	});
	server.on("/systemOutVirablesValues", HTTP_GET, []() {
		server.send(200, "text/json", externVir.generateOutValNamePairString());
	});


    server.begin();
    DB1("HTTP server started");
  
  
	//Konfiguracja sond
	OUT.begin(BAUDRATE);
	OUT.setTimeout(SERIAL_TIMEOUT);
	readMagistralConfig();				//and init virtual obiects
	countAllocMeasuresSize();
	OUT.setTimeout(SERIAL_TIMEOUT1);
	
	externVir.initSizeOfProbesValues();		//przypisywanie pamieci dla pomiarow
	externVir.initInputVirablesNames();		//inicjacja nazw zmiennych wejsciowych
	externVir.initOutputVirablesNames();	//init zmiennych wyjsciowych
	externVir.reloadVirValues();			//przeladowanie wartosci zmiennych wejsciowych
	//Zadania okresowe

	if(System_s.measInt<60) System_s.measInt = 60;
	
	Alarm.timerRepeat(System_s.measInt, setMeasureTaskFlag);
	Alarm.timerRepeat(60, tryToConnect);
	Alarm.timerRepeat(1, displayOled);
	//Alarm.timerRepeat(5, getFreeHeap);
	
	if(System_s.relayScriptTime < 5){
		System_s.relayScriptTime = 5;
	}
	Alarm.timerRepeat(System_s.relayScriptTime,setScriptTaskFlag);
	digitalClockDisplay();

	systemIsLoaded = 1;
}

time_t prevDisplay = 0; // when the digital clock was displayed

uint8_t mtFlag = 1;
void setMeasureTaskFlag(void){
	mtFlag = 1;
}
uint8_t stFlag = 0;
void setScriptTaskFlag(void){
	stFlag = 1;
}
uint8_t doFlag = 0;
void setDisplayOledFLag(void){
	doFlag = 1;
}
uint8_t silenceFlag = 1;
void setSilenceFLag(void){
	silenceFlag = 1;
}

void loop(void) {
	if(systemIsLoaded){	
		//priority of task
	  if(mtFlag){
		  measure_task();
		  mtFlag = 0;
		  silenceFlag = 0;
		  Alarm.alarmOnce(System_s.measInt-((elapsed/1000) + 1), setSilenceFLag);
	   }				//1
	  if(silenceFlag == 0){
		  if(stFlag){
			  start=millis();
			  scriptTask();
			  finished=millis();
			  elapsed=finished-start;
			  DB3("Czas sykonywania skryptu:" + String(elapsed));
			  stFlag = 0;
			  if(System_s.relayScriptTime<(elapsed/1000)){
				  System_s.relayScriptTime = (elapsed/1000)+1;
			  }
			  }					//2
		  if(doFlag){displayOled();doFlag = 0;}					//3
		  server.handleClient();								//4
	  }
	  Alarm.serviceAlarms(); 								//5 	Check task times
	}
}


/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP server's ip address

  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  DB1("Transmit NTP Request");
  // get a random server from the pool
  WiFi.hostByName(System_s.NTP_addr.c_str(), ntpServerIP);
  DB1(System_s.NTP_addr.c_str());
  DB1(": ");
  DB1(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
		int size = Udp.parsePacket();
		if (size >= NTP_PACKET_SIZE) {
		  DB1("Receive NTP Response");
		  Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
		  unsigned long secsSince1900;
		  // convert four bytes starting at location 40 to a long integer
		  secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
		  secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
		  secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
		  secsSince1900 |= (unsigned long)packetBuffer[43];
		  
		  NTP_found++;
		  
		  return (secsSince1900 - 2208988800UL);
		}
	  DB1("No NTP Response :-(");
	  return 0; // return 0 if unable to get the time
	}
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
  DB1(hour());
  printDigits(minute());
  printDigits(second());
  DB1(" ");
  DB1(day());
  DB1(".");
  DB1(month());
  DB1(".");
  DB1(year());
  DB1();
}

void printDigits(int digits)
{
  // utility for digital clock display: prints preceding colon and leading 0
  DB1(":");
  if (digits < 10)
    DB1('0');
  DB1(digits);
}

void saveEthernetConfig(void){
	File f = SPIFFS.open("/ethernetConf.txt", "w+");
	if (!f) {
		DB1("file open failed");
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
		DB1("file open failed");
	}else{
		settings_str = f.readStringUntil('\n');
		DB1(settings_str);
		settings_str.toCharArray(charBuf, 500);
		DB1(charBuf);
		schowek = strtok( charBuf , korektor );
		DB1(schowek);
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
		DB1("file open failed");
	}else{
		//ustawienia czasow
		f.print(String((int)System_s.timeMod) + ";");
		f.print(String(System_s.NTP_addr) + ";");
		f.print(String(System_s.year) + ";");
		f.print(String((int)System_s.month) + ";");
		f.print(String((int)System_s.day) + ";");
		f.print(String((int)System_s.hour) + ";");
		f.print(String((int)System_s.minute) + ";");
		f.print(String((int)System_s.second) + ";");
		
		f.print(String((int)System_s.timeZone) + ";");
		f.print(String(System_s.measInt) + ";");
		
		f.print(System_s.workingScript + ";");
		f.print(String((int)System_s.relayScriptTime) + ";");
		f.println();
	}
	f.close();
	setTime((int)System_s.hour,(int)System_s.minute,(int)System_s.second,(int)System_s.day, (int)System_s.month, (int)System_s.year);
	RTC.writeEN(1);
	RTC.set(now() - (3600*timeZone()));
	RTC.writeEN(0);
	RTC.read(tm);
}

void readSystemConfig(void){
	File f = SPIFFS.open("/systemConf.txt", "r+");
	char charBuf[500];
	char korektor[] = ";";
	char * schowek;
	String settings_str;
	if (!f) {
		DB1("file open failed");
	}else{
		settings_str = f.readStringUntil('\n');
		DB1(settings_str);
		settings_str.toCharArray(charBuf, 500);
		DB1(charBuf);
		schowek = strtok( charBuf , korektor );
		DB1(schowek);

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
		
		schowek = strtok( NULL , korektor );
		System_s.timeZone = atoi(schowek);
		
		schowek = strtok( NULL , korektor );
		System_s.measInt = atoi(schowek);
	
		schowek = strtok( NULL , korektor );
		System_s.workingScript = String(schowek);
		
		schowek = strtok( NULL , korektor );
		System_s.relayScriptTime = atoi(schowek);

	}
	f.close();
}

String listNetworks() {
  String wrless;
  // scan for nearby networks:
  DB1("** Scan Networks **");
  int numSsid = WiFi.scanNetworks();
  if (numSsid == -1) {
    DB1("Couldn't get a wifi connection");
    while (true);
  }

  // print the list of networks seen:
  DB1("number of available networks:");
  DB1(numSsid);
  wrless += ",\"numOfWiFi\":" + String(numSsid);
  // print the network number and name for each network found:
  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    DB1(thisNet);
    DB1(") ");
    DB1(WiFi.SSID(thisNet));
	wrless += ",\"name"+String(thisNet)+"\":\"" + String(WiFi.SSID(thisNet)) +"\"";
    DB1("\tSignal: ");
	wrless += ",\"signal"+String(thisNet)+"\":" + String(WiFi.RSSI(thisNet));
    DB1(WiFi.RSSI(thisNet));
    DB1(" dBm");
    DB1("\tEncryption:");
	wrless += ",\"encryption" + String(thisNet) +"\":\"" + printEncryptionType(WiFi.encryptionType(thisNet))+"\"";
  }
  return wrless;
}

String printEncryptionType(int thisType) {
  String passType;
  // read the encryption type and print out the name:
  switch (thisType) {
    case ENC_TYPE_WEP:
      DB1("WEP");
	  passType = "WEP";
	  return(passType);
      break;
    case ENC_TYPE_TKIP:
      DB1("WPA");
	  passType = "WPA";
	  return(passType);
      break;
    case ENC_TYPE_CCMP:
      passType = "WPA2";
	  return(passType);
      break;
    case ENC_TYPE_NONE:
      DB1("None");
	  passType = "None";
	  return(passType);
      break;
    case ENC_TYPE_AUTO:
      DB1("Auto");
	  passType = "Auto";
	  return(passType);
      break;
  }
}

//Funkcja zapisuje przyjmowany skrypt do pliku
String saveScriptFile(String scriptContent){
	File f = SPIFFS.open(scriptName, "w+");
	DB1("Script name: " + scriptName);
	if (!f) {
		DB1("file open failed");
		f.close();
		return "";
	}else{
		DB1("Script content: " + scriptContent);
		f.print(scriptContent);
	}

	f.close();
	return scriptContent;
}
char apConnected = 0;
void tryToConnect(void){
	short conn = digitalRead(ApClientPin);

	if((WiFi.status() != WL_CONNECTED)&&(apConnected == 0)){
	  DB1("ApClientPin: " + String(conn));

	  if(conn){
		  WiFi.mode(WIFI_STA);
		//10 times trying to connect with wi-fi
		  //for (int conCounter = 0; (conCounter < 10)/*&&(WiFi.status() != WL_CONNECTED)*/; conCounter++) {
			delay(500);
			if(ethernet.dhcpOn){
				DB1("DHCP enabled");
				
				DB1("Connecting to " + String(ethernet.siec));
				if (String(WiFi.SSID()) != ethernet.siec) {
					WiFi.begin(ethernet.siec.c_str(), ethernet.haslo.c_str());
					
					/*while(((String("0")==String(WiFi.localIP())))){
						DB1("No IP!");
						ESP.wdtFeed();
					}*/
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
				delay(100);
				DB1("Connecting to " + String(ethernet.siec));
				if (String(WiFi.SSID()) != ethernet.siec) {
					WiFi.begin(ethernet.siec.c_str(), ethernet.haslo.c_str());
				}
				DB1("Inicjalizacja!");
				for(int i = 0;i<20;i++){
					DB1("Connecting...");
					delay(100);
					if(
						(ethernet.ip3 == (0xFF000000 & WiFi.localIP()) >> 24)
						||(ethernet.ip2 == (0xFF0000 & WiFi.localIP()) >> 16)
						||(ethernet.ip1 == (0xFF00 & WiFi.localIP()) >> 8)
						||(ethernet.ip0 == 0xFF & WiFi.localIP())
					) break;
					WiFi.config(ip,gateway,subnet,dns1,dns2);
				}
			}
		 // }		
		if(ethernet.dhcpOn == 0){
			if(
				(ethernet.ip3 != (0xFF000000 & WiFi.localIP()) >> 24)
				||(ethernet.ip2 != (0xFF0000 & WiFi.localIP()) >> 16)
				||(ethernet.ip1 != (0xFF00 & WiFi.localIP()) >> 8)
				||(ethernet.ip0 != 0xFF & WiFi.localIP())
			) ESP.reset();  
		} 
		saveIpSettings();
		
		DB1("");
		DB1("Connected! IP address: ");
		DB1(WiFi.localIP());
		
		  MDNS.begin(host);
		  DB1("Open http://");
		  DB1(host);
		  DB1(".local/edit to see the file browser");
	  }else{
		  
		  apConnected++;
		  DB1("Configuring access point...");
	  // You can remove the password parameter if you want the AP to be open. 
		  WiFi.softAP("NodeMcu", "12345678");
	 
		  IPAddress myIP = WiFi.softAPIP();
		  DB1("AP IP address: ");
		  DB1(myIP);
		  //Ĺ»eby ustawiÄ‡ wĹ‚asne parametry acces pointu
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
	}
	if((ethernet.ip0 == 0)&&(ethernet.ip1 == 0)&&(ethernet.ip2 == 0)&&(ethernet.ip3 == 0)){
		saveIpSettings();
	}
}

void initRC(void){
	if(digitalRead(ApClientPin)&&(System_s.timeMod==0)){
		for(i = 0;((i>5)||(NTP_found > 1)) == 0;i++){
		  DB1("Starting UDP");
		  Udp.begin(localPort);
		  DB1("Local port: ");
		  DB1(Udp.localPort());
		  DB1("waiting for sync");
		  setSyncProvider(getNtpTime);
		  setSyncInterval(300);
		}
		if(NTP_found){
			RTC.writeEN(1);
			RTC.set(now());
			RTC.writeEN(0);
		}
		
	}
	adjustTime(timeZone()*3600);
}
void saveIpSettings(void){
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
		Alarm.timerOnce(10, saveIpSettings);
}
void getFreeHeap(void){
	DB1("Free Heap"+String(ESP.getFreeHeap()));
}
int timeZone(void){
	DB1("Wybrana opcja strefy: " + String((int)System_s.timeZone));	
	switch((int)System_s.timeZone){
		case 0:
			return 12;
			break;
		case 1:
			return 11;
			break;
		case 2:
			return 10;
			break;
		case 3:
			return 9;
			break;
		case 4:
			return 8;
			break;
		case 5:
			return 7;
			break;
		case 6:
			return 6;
			break;
		case 7:
			return 5;
			break;
		case 8:
			return 4;
			break;
		case 9:
			return 3;
			break;
		case 10:
			return 2;
			break;
		case 11:
			return 1;
			break;
		case 12:
			return 0;
			break;
		case 13:
			return -1;
			break;
		case 14:
			return -2;
			break;
		case 15:
			return -3;
			break;
		case 16:
			return -4;
			break;
		case 17:
			return -5;
			break;
		case 18:
			return -6;
			break;
		case 19:
			return -7;
			break;
		case 20:
			return -8;
			break;
		case 21:
			return -9;
			break;
		case 22:
			return -10;
			break;
		case 23:
			return -11;
			break;
		case 24:
			return -12;
			break;
		case 25:
			if(RTC_TestTime(tm)) return 2; else return 1;
			break;
	}
}
//--------------------------------------------------------------------------------------------------------------------- 
// Testuj czas 
// Funkcja wykonywana jest tylko podczas startu sterownika 
// Funkcja sprawdzajaca poprawnosc czasu na podstawie flagi czasu oraz aktualnego czasu i daty 
// Funkcja zwraca 0- zimowy 1- letni
// TestujCzas 
unsigned char RTC_TestTime(tmElements_t DateTime) 
{ 
    unsigned char sts = 0; 
	DB1(String(DateTime.Hour)+String(":")+String(DateTime.Minute)+String(" ")+String(DateTime.Day)+String("-")+String(DateTime.Month)+String("-")+String(DateTime.Year));
    /* sprawdzamy miesiac i dzien tygodnia */ 
    /* czas letni - zpis znacznika tylko jezeli sie zmienil */ 
    if((DateTime.Month < 10) && (DateTime.Month > 3)) 
    { 
		DB1("Miesiac wskazuje czas letni");
        sts = 1; 
    } 
    /* czas zimowy */ 
    if(((DateTime.Month < 3) || (DateTime.Month > 10))) 
    { 
		DB1("Miesiac wskazuje czas zimowy");
        sts = 0; 
    } 

    /* sprawdzenie dla marca i pazdziernika musimy ustalic czy jestesmy przed niedziela zmiany czasu czy tez za ostatnia niedz. */ 
    if(DateTime.Month == 3) 
    { 
        if(DateTime.Day < 25) 
        { 
                sts = 0; 
				DB1("<25 marca");
																/* dla dnia < 25 na pewno czas zimowy                         */ 
        } 
        else 
        { 
            if((DateTime.Day + (7-((DateTime.Wday + 1) % 7))) > 31) 
            { 
					/* dla dnia>25 jezeli nie bedzie juz niedzieli jest czas letni */ 
                    sts = 1; 
					DB1(">25 marca");
            } 
            else 
            { 		
					DB1("wystapi niedziela");
                    sts = 0; /* zmiana czasu dopiero nastapi - wystapi jeszcze niedz.        */ 
            } 
        } 
        /* sprawdzamy dodatkowo czy nie wystepuje dzien zmiany czasu */ 
        if((DateTime.Wday==6) && ((DateTime.Day + 7) > 31)) 
        { 
            if(DateTime.Hour < 2) 
            { 
					DB1("dzien zmiany czasu");
                    sts = 0; 
            } 
            else 
            { 
					DB1("po godzinie zmiany czasu");
                     /* jest juz okres po zmienie czasu                              */
                    sts = 1; 
            } 
        } 
    } 
    if(DateTime.Month==10) 
    { 
        if(DateTime.Day < 25) 
        { 
			DB1("przed 25 padz");
            /* dla dnia < 25 na pewno czas letni                            */ 
            sts = 1; 
        } 
        else 
        { 
            if((DateTime.Day + (7 - ((DateTime.Wday + 1) % 7))) > 31) 
            { 
                    /* dla dnia>25 jezeli nie bedzie juz niedzieli jest czas zimowy */ 
                    sts = 0; 
                DB1("przed niedz 25 padz");
            } 
            else 
            {
                sts = 1; 
				DB1("po niedz 25 padz");
            } 
        } 
        /* sprawdzamy dodatkowo czy nie wystepuje dzien zmiany czasu  */ 
        /* oraz czy zmiana nie byla juz wykonana                      */ 
        if((DateTime.Wday == 6) && ((DateTime.Day + 7) > 31)) 
        { 
            if(DateTime.Hour < 3) 
            { 
                    sts = 1; 
					DB1("dzien zm cz. przed 3");
            } 
            else 
            { 
                /* jest juz okres po zmienie czasu                              */ 
                    sts = 0; 
					DB1("dzien zm cz. po 3");

            } 
        } 
    } 
    /* Flaga czasu ustawiona */
	DB1("Automatyczna zmiana czasu: gtm +" + String(sts));	
    return sts; 
}
