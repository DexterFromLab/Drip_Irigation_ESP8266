#include <Arduino.h>
#include <FS.h>

#define DB1(x)  Serial.println(x)
#define OUT	Serial
String structNames(void){
  String path = "/";
  Dir dir = SPIFFS.openDir(path);
  String output = "";
  while (dir.next()) {
    File entry = dir.openFile("r");
    if(strstr(String(entry.name()).c_str(),".sav")){
        output += String(entry.name()).substring(1);
        output += ";";
    }
  }
  return output;
}
int numOfStructs = 0;
int numOfAllSavedStruct(void){
  String allStructNames;
  allStructNames = structNames();
  char * charWsk;

  charWsk = strtok(&allStructNames[0],";");

  numOfStructs = 0;
  while(charWsk != NULL){
    charWsk = strtok(NULL,";");
    numOfStructs++;
  }
  return numOfStructs;
}
void dispAllNames(void){

  String allStructNames;
  allStructNames = structNames();
  char * charWsk;

  charWsk = strtok(&allStructNames[0],";");

  int numOfStructs = 0;
  while(charWsk != NULL){
    DB1(charWsk);
    charWsk = strtok(NULL,";");
  }

}
int nameStructCounter = 0;
String dispNextName(void){

  String allStructNames;
  allStructNames = structNames();
  char * charWsk;
  String lastStructName;

  charWsk = strtok(&allStructNames[0],";");

  int i = 0;
  while(charWsk != NULL){
    lastStructName = String(charWsk);
    if(nameStructCounter == i){
      break;
    }
    charWsk = strtok(NULL,";");
    i++;
  }
  nameStructCounter++;
  if(charWsk == NULL) nameStructCounter = 0;
  return lastStructName;
}
int readAutoControlState(String fileName){
  File f = SPIFFS.open("/"+fileName, "r+");
  DB1("/"+fileName);

  String settings_str;
  if (!f) {
    DB1("file open failed");
  }else{

    settings_str = f.readStringUntil('\n');
    DB1(settings_str);
    int numOfChar = settings_str.indexOf("$autoCont#");
    DB1(numOfChar);
    settings_str = settings_str.substring(numOfChar+11,numOfChar+12);
    DB1(fileName+" controlState value: "+settings_str);
  }
  f.close();
  return atoi(settings_str.c_str());
}
void generateAutoScript(){
  File controlScript = SPIFFS.open("/auto.scr", "r+");
  int startSel;
  int endSel;
  String settings_str;

  DB1("Num of all sav files: "+String(numOfAllSavedStruct()));
  int allString = numOfAllSavedStruct();
  for(int i = 0;i<allString;i++){
    File f = SPIFFS.open("/"+dispNextName(), "r+");
    settings_str = f.readStringUntil('\n');
      if (!f) {
        DB1("file open failed");
      }else{
		  
			String timeMark;
			String nameOfRelay;
			String autoCont;
			String controlMark;
			String day1startTimes;
			String day1endTimes;
			String day2startTimes;
			String day2endTimes;
			String day3startTimes;
			String day3endTimes;
			String day4startTimes;
			String day4endTimes;
			String day5startTimes;
			String day5endTimes;
			String day6startTimes;
			String day6endTimes;
			String day7startTimes;
			String day7endTimes;
			String typeOfControlminVal;
			String typeOfControlmaxVal;
			
			timeMark = getValueByName("timeMark");
			nameOfRelay = getValueByName("nameOfRelay");
			autoCont = getValueByName("autoCont");
			controlMark = getValueByName("controlMark");
			day1startTimes = getValueByName("day1%5BstartTimes");
			day1endTimes= getValueByName("day1%5BendTimes");
			day2startTimes = getValueByName("day2%5BstartTimes");
			day2endTimes = getValueByName("day2%5BendTimes");
			day3startTimes = getValueByName("day3%5BstartTimes");
			day3endTimes = getValueByName("day3%5BendTimes");
			day4startTimes = getValueByName("day4%5BstartTimes");
			day4endTimes = getValueByName("day4%5BendTimes");
			day5startTimes = getValueByName("day5%5BstartTimes");
			day5endTimes = getValueByName("day5%5BendTimes");
			day6startTimes = getValueByName("day6%5BstartTimes");
			day6endTimes = getValueByName("day6%5BendTimes");
			day7startTimes = getValueByName("day7%5BstartTimes");
			day7endTimes = getValueByName("day7%5BendTimes");
			typeOfControlminVal = getValueByName("typeOfControl%5BminVal");
			typeOfControlmaxVal = getValueByName("typeOfControl%5BmaxVal");
			
			DB1(timeMark);
      }
    f.close();
  }
  controlScript.close();
}

String getValueByName(String name){
	int startSel = 0;
	int endSel = 0;
	String settings_str;
	
	  int allString = numOfAllSavedStruct();
	  for(int i = 0;i<allString;i++){
		File f = SPIFFS.open("/"+dispNextName(), "r+");
		settings_str = f.readStringUntil('\n');
		  if (!f) {
			DB1("file open failed");
		  }else{
			  startSel = settings_str.indexOf(name);
			  settings_str = settings_str.substring(startSel);
			  startSel = settings_str.indexOf("#:") + 2;
			  endSel = settings_str.indexOf("#$");
			  f.close();
			  return(settings_str.substring(startSel,endSel));
		  }
	}
}
void setup() {
	OUT.begin(115200);
	OUT.print("\n");
	OUT.setDebugOutput(true);
	SPIFFS.begin();
	DB1("Program start");
	
	generateAutoScript();
}

void loop() {
  // put your main code here, to run repeatedly:

}
