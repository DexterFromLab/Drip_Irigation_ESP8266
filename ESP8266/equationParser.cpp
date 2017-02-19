#include "equationParser.h"

externalVirables externVir;

std::vector<unsigned int> splitTimeStringToVector(String timesStringHlp);



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
  File controlScript = SPIFFS.open("/auto.scr", "w+");
  int startSel;
  int endSel;
  String settings_str;

  DB1("Num of all sav files: "+String(numOfAllSavedStruct()));
  int allString = numOfAllSavedStruct();
  for(int i = 0;i<allString;i++){
	 String name;
	 name = dispNextName();
	 DB1(name);
    File f = SPIFFS.open("/"+name, "r+");
    settings_str = f.readStringUntil('\n');
      if (!f) {
        DB1("file open failed");
      }else{
		  
			DB1("List of virables: ");
		  
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
			
			DB1("After all list");
			
			timeMark = getValueByName(settings_str,"timeMark");
			nameOfRelay = getValueByName(settings_str,"nameOfRelay");
			autoCont = getValueByName(settings_str,"autoCont");
			controlMark = getValueByName(settings_str,"controlMark");
			day1startTimes = getValueByName(settings_str,"day1%5BstartTimes");
			day1endTimes= getValueByName(settings_str,"day1%5BendTimes");
			day2startTimes = getValueByName(settings_str,"day2%5BstartTimes");
			day2endTimes = getValueByName(settings_str,"day2%5BendTimes");
			day3startTimes = getValueByName(settings_str,"day3%5BstartTimes");
			day3endTimes = getValueByName(settings_str,"day3%5BendTimes");
			day4startTimes = getValueByName(settings_str,"day4%5BstartTimes");
			day4endTimes = getValueByName(settings_str,"day4%5BendTimes");
			day5startTimes = getValueByName(settings_str,"day5%5BstartTimes");
			day5endTimes = getValueByName(settings_str,"day5%5BendTimes");
			day6startTimes = getValueByName(settings_str,"day6%5BstartTimes");
			day6endTimes = getValueByName(settings_str,"day6%5BendTimes");
			day7startTimes = getValueByName(settings_str,"day7%5BstartTimes");
			day7endTimes = getValueByName(settings_str,"day7%5BendTimes");
			typeOfControlminVal = getValueByName(settings_str,"typeOfControl%5BminVal");
			typeOfControlmaxVal = getValueByName(settings_str,"typeOfControl%5BmaxVal");
			
			
			
			DB1("day1startTimes: "+ day1startTimes);
			
			
			std::vector<unsigned int> startTimeDaySec;
			std::vector<unsigned int> endTimeDaySec;
			
			char timeChar[6];
			short hour;
			short minute;
			unsigned int daySec;
			char * wsk;
			char * wsk1;

			DB1("atoi(autoCont.c_str()): "+String(atoi(autoCont.c_str())));
			if(atoi(autoCont.c_str())){
				DB1("atoi(timeMark.c_str()): " + String(atoi(timeMark.c_str())));
				if(atoi(timeMark.c_str())==1){
					startTimeDaySec = splitTimeStringToVector(day1startTimes);
					endTimeDaySec = splitTimeStringToVector(day1endTimes);
					controlScript.println("0=Tmp9");
					controlScript.println("1=Rel0AutoControl");
					for(int i = 0; i < startTimeDaySec.size();i++){
						controlScript.println("DaySec>" + String(startTimeDaySec.at(i)) + "=Tmp1");
						controlScript.println("DaySec<" + String(endTimeDaySec.at(i)) + "=Tmp2");
						controlScript.println("Tmp1&Tmp2+Tmp9=Tmp9");
						DB1("Day"+String(i)+" string generated!");
					}
				}else if(atoi(timeMark.c_str())==2){
					startTimeDaySec = splitTimeStringToVector(day1startTimes);
					endTimeDaySec = splitTimeStringToVector(day1endTimes);
					controlScript.println("0=Tmp9");
					for(int i = 0; i < startTimeDaySec.size();i++){
						controlScript.println("DaySec>" + String(startTimeDaySec.at(i)) + "=Tmp1");
						controlScript.println("DaySec<" + String(endTimeDaySec.at(i)) + "=Tmp2");
						controlScript.println("weekd>0=Tmp3");
						controlScript.println("weekd<2=Tmp4");
						controlScript.println("Tmp3&Tmp4=Tmp3");
						controlScript.println("Tmp1&Tmp2&Tmp3+Tmp9=Tmp9");
						DB1("Day"+String(i)+" string generated!");
					}
					startTimeDaySec = splitTimeStringToVector(day2startTimes);
					endTimeDaySec = splitTimeStringToVector(day2endTimes);
					for(int i = 0; i < startTimeDaySec.size();i++){
						controlScript.println("DaySec>" + String(startTimeDaySec.at(i)) + "=Tmp1");
						controlScript.println("DaySec<" + String(endTimeDaySec.at(i)) + "=Tmp2");
						controlScript.println("weekd>2=Tmp3");
						controlScript.println("weekd<4=Tmp4");
						controlScript.println("Tmp3&Tmp4=Tmp3");
						controlScript.println("Tmp1&Tmp2&Tmp3+Tmp9=Tmp9");
						DB1("Day"+String(i)+" string generated!");
					}
					startTimeDaySec = splitTimeStringToVector(day3startTimes);
					endTimeDaySec = splitTimeStringToVector(day3endTimes);
					for(int i = 0; i < startTimeDaySec.size();i++){
						controlScript.println("DaySec>" + String(startTimeDaySec.at(i)) + "=Tmp1");
						controlScript.println("DaySec<" + String(endTimeDaySec.at(i)) + "=Tmp2");
						controlScript.println("weekd>3=Tmp3");
						controlScript.println("weekd<5=Tmp4");
						controlScript.println("Tmp3&Tmp4=Tmp3");
						controlScript.println("Tmp1&Tmp2&Tmp3+Tmp9=Tmp9");
						DB1("Day"+String(i)+" string generated!");
					}
					startTimeDaySec = splitTimeStringToVector(day4startTimes);
					endTimeDaySec = splitTimeStringToVector(day4endTimes);
					for(int i = 0; i < startTimeDaySec.size();i++){
						controlScript.println("DaySec>" + String(startTimeDaySec.at(i)) + "=Tmp1");
						controlScript.println("DaySec<" + String(endTimeDaySec.at(i)) + "=Tmp2");
						controlScript.println("weekd>4=Tmp3");
						controlScript.println("weekd<6=Tmp4");
						controlScript.println("Tmp3&Tmp4=Tmp3");
						controlScript.println("Tmp1&Tmp2&Tmp3+Tmp9=Tmp9");
						DB1("Day"+String(i)+" string generated!");
					}
					startTimeDaySec = splitTimeStringToVector(day5startTimes);
					endTimeDaySec = splitTimeStringToVector(day5endTimes);
					for(int i = 0; i < startTimeDaySec.size();i++){
						controlScript.println("DaySec>" + String(startTimeDaySec.at(i)) + "=Tmp1");
						controlScript.println("DaySec<" + String(endTimeDaySec.at(i)) + "=Tmp2");
						controlScript.println("weekd>5=Tmp3");
						controlScript.println("weekd<7=Tmp4");
						controlScript.println("Tmp3&Tmp4=Tmp3");
						controlScript.println("Tmp1&Tmp2&Tmp3+Tmp9=Tmp9");
						DB1("Day"+String(i)+" string generated!");
					}
					startTimeDaySec = splitTimeStringToVector(day6startTimes);
					endTimeDaySec = splitTimeStringToVector(day6endTimes);
					for(int i = 0; i < startTimeDaySec.size();i++){
						controlScript.println("DaySec>" + String(startTimeDaySec.at(i)) + "=Tmp1");
						controlScript.println("DaySec<" + String(endTimeDaySec.at(i)) + "=Tmp2");
						controlScript.println("weekd>6=Tmp3");
						controlScript.println("Tmp1&Tmp2&Tmp3+Tmp9=Tmp9");
						DB1("Day"+String(i)+" string generated!");
					}
					startTimeDaySec = splitTimeStringToVector(day7startTimes);
					endTimeDaySec = splitTimeStringToVector(day7endTimes);
					for(int i = 0; i < startTimeDaySec.size();i++){
						controlScript.println("DaySec>" + String(startTimeDaySec.at(i)) + "=Tmp1");
						controlScript.println("DaySec<" + String(endTimeDaySec.at(i)) + "=Tmp2");
						controlScript.println("weekd<1=Tmp3");
						controlScript.println("Tmp1&Tmp2&Tmp3+Tmp9=Tmp9");
						DB1("Day"+String(i)+" string generated!");
					}
				}
			}else{
				controlScript.println("0=Rel0AutoControl");
			}
			DB1("atoi(controlMark.c_str()):"+String(atoi(controlMark.c_str())));
			DB1(controlMark);
			if(controlMark.length()>1){
				controlScript.println("0=Tmp7");
				controlScript.println("0=Tmp8");
				controlScript.println(controlMark+"<"+typeOfControlmaxVal+"+Tmp7=Tmp7");
				controlScript.println(controlMark+">"+typeOfControlminVal+"+Tmp7=Tmp7");
				controlScript.println(controlMark+"<"+typeOfControlminVal+"+Tmp8=Tmp8");
				controlScript.println(controlMark+"<"+typeOfControlmaxVal+"+Tmp8=Tmp8");
				controlScript.println("Tmp7<2&Tmp8&Tmp9="+nameOfRelay);
			}else if((atoi(timeMark.c_str())>0)){
				controlScript.println("Tmp9="+nameOfRelay);
			}
      }
    f.close();
  }
  controlScript.close();
}
	
std::vector<unsigned int> splitTimeStringToVector(String timesStringHlp){
	char timesString[301];
	char *wsk;
	char timeChar[6];
	std::vector<String> timeStringHolder;
	
	

	sprintf(timesString,timesStringHlp.c_str());
	DB1(timesString);
	wsk = strtok(timesString,";");
	
	while(wsk != 0){
		sprintf(timeChar,wsk);
		timeStringHolder.push_back(String(timeChar));
		DB1(wsk);
		wsk = strtok(NULL,";");
	}
	short minute;
	short hour;
	unsigned int daySec;
	char * wsk1;
	std::vector<unsigned int> daySecVector;
	for(int i = 0; i < timeStringHolder.size();i++){
		sprintf(timeChar,timeStringHolder.at(i).c_str());
		wsk1 = strchr(timeChar,':');
		wsk = strtok(timeChar, ":");
		hour = atoi(wsk);
		minute = atoi(wsk1+1);
		DB1("hour: " + String(hour));
		DB1("minute: " + String(minute));
		daySec = (hour*3600)+(minute*60);
		DB1("daySec: " + String(daySec));
		daySecVector.push_back(daySec);
	}
	return daySecVector;
}


String getValueByName(String settings_str,String name){
	int startSel = 0;
	int endSel = 0;
	
	DB1("settings_str: " + settings_str);
	startSel = settings_str.indexOf(name);
	settings_str = settings_str.substring(startSel);
	startSel = settings_str.indexOf("#:") + 2;
	endSel = settings_str.indexOf("#$");
	DB1(name + ": "+settings_str.substring(startSel,endSel));

	return(settings_str.substring(startSel,endSel));
	
}