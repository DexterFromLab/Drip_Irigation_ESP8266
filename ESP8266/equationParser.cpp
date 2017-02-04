#include "equationParser.h"

externalVirables externVir;

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
	
	String nameOfRelay;
	int timeMark;
	String controlMark;
	int autoCont;
	String startTimes;
	String endTimes;
	
	char * charWsk;
	char * charWsk1;
	
	DB1("Num of all sav files: "+String(numOfAllSavedStruct()));
	int allString = numOfAllSavedStruct();
	for(int i = 0;i<allString;i++){
		File f = SPIFFS.open("/"+dispNextName(), "r+");
		settings_str = f.readStringUntil('\n');
			if (!f) {
				DB1("file open failed");
			}else{
				startSel = settings_str.indexOf("nameOfRelay");
				endSel = settings_str.indexOf("#",startSel+13);
				nameOfRelay = settings_str.substring(startSel+13,endSel);
				
				DB1("nameOfRelay: " + nameOfRelay);
				
				startSel = settings_str.indexOf("timeMark#:");
				endSel = settings_str.indexOf("#",startSel+10);
				timeMark = atoi(settings_str.substring(startSel+10,endSel).c_str());
				
				DB1("timeMark: " + timeMark);
				
				startSel = settings_str.indexOf("$controlMark#:");
				endSel = settings_str.indexOf("#",startSel+14);
				controlMark = String(settings_str.substring(startSel+14,endSel).c_str());
				
				DB1("controlMark: " + controlMark);
				
				startSel = settings_str.indexOf("autoCont#:");
				endSel = settings_str.indexOf("#",startSel+10);
				autoCont = atoi(settings_str.substring(startSel+10,endSel).c_str());
				
				DB1("autoCont: " + autoCont);
				
				if(autoCont){
					if(timeMark != 1){
						if(timeMark > 0){
							startSel = settings_str.indexOf("$day1%5BstartTimes%5D#:");
							endSel = settings_str.indexOf(";#",startSel+23);
							startTimes = settings_str.substring(startSel+14,endSel).c_str();
							DB1(startTimes);
							
							startSel = settings_str.indexOf("$day1%5BendTimes%5D#:");
							endSel = settings_str.indexOf(";#",startSel+23);
							endTimes = settings_str.substring(startSel+14,endSel).c_str();
							DB1(endTimes);
							
							charWsk = strtok(&startTimes[0],";");
							charWsk1 = strtok(&endTimes[0],";");
							int startSepWsk;
							int endSepWsk;
							while(charWsk != NULL){
								startSepWsk = startTimes.indexOf(":");
								endSepWsk = endTimes.indexOf(":");
								
								DB1(startTimes.substring(0,startSepWsk));
								DB1(endTimes.substring(0,endSepWsk));
								DB1(startTimes.substring(startSepWsk));
								DB1(endTimes.substring(endSepWsk));
								
								charWsk = strtok(NULL,";");
								charWsk1 = strtok(NULL,";");
							}
						}
						if(timeMark>1){
							
						}
					}
				}
				
			}
		f.close();
	}
	controlScript.close();
}