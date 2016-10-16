#include "./externalProbes.h"

std::vector <String>		inputVirablesNames;
std::vector <float>			inputVirablesValues;
std::vector <String>		outputVirablesNames;
std::vector <float>			outputVirablesValues;

void readMagistralConfig(void);
void countAllocMeasuresSize(void);

char response[20];


sensorExecutorObiect * obPointArr[MAX_NUM_SEN];

void readMagistralConfig(void){
	//Configuration of virtual obiect in memory
	int sensorConfig;
	String question;	//Tworzenie pytania z adresem
	
	char * wsk;
	DB1("ESP free heap: "+String(ESP.getFreeHeap()));
	for(int i = 0 ; i < MAX_NUM_SEN ; i++){	
		response[0] = 0;
		//Checking adresses on magistral and reading devices config;
		OUT.print("#"+String(i));
		OUT.readBytes(response,20);
		DB1((wsk = strchr(response,'&')));
		if((wsk = strchr(response,'&')) != NULL){
			OUT.readBytes(response, 20);
			wsk++;
			sensorConfig = atoi(wsk);
			//Creating virtual sensor obiects

			obPointArr[i] = new sensorExecutorObiect;
			obPointArr[i]->init(sensorConfig,i);
				
			DB1("Obiect nr." +String(i)+" created with config: "+String(obPointArr[i]->config));
			drawFoundSensor(String(String("Dev. ") + String(i) + String(" det.")).c_str());
		}else{
			obPointArr[i] = new sensorExecutorObiect;
			obPointArr[i]->init(0,i);
			
			DB1("Obiect nr." +String(i)+" created with config: "+String(obPointArr[i]->config));
		}
		
	}
	
	for(int i = 0 ; i < MAX_NUM_SEN ; i++){
		DB1("Size ob."+String(i)+": " + String(obPointArr[i]->sizeOfClass()));
	}
	
}
void countAllocMeasuresSize(void){
	unsigned int sizeOfallMeasures = 0;
	unsigned int numberOfMeasures;
	for(int i = 0 ; i < MAX_NUM_SEN ; i++){
		sizeOfallMeasures += obPointArr[i]->sizeOfClass();
	}

	DB1("Size of all class: "+String(sizeOfallMeasures));
	
	DB1("ESP free heap: "+String(ESP.getFreeHeap()));
	if(sizeOfallMeasures>0) numberOfMeasures = (unsigned int)((ESP.getFreeHeap()-FREE_RAM)/sizeOfallMeasures);
	for(int i = 0 ; i < MAX_NUM_SEN ; i++){
		if(sizeOfallMeasures>0) obPointArr[i]->create_tables(numberOfMeasures);
		DB1("Heap"+String(i)+": "+String(ESP.getFreeHeap()));
	}
	DB1("Liczba pomiarów: "+String(obPointArr[0]->numOfMeasure)+String(" heap: ")+String(ESP.getFreeHeap()));
	#ifdef DEBUG2
		DB2("Lista nazw zmiennych");
		for(int i = 0;i<inputVirablesNames.size();i++){
			DB2(inputVirablesNames.at(i));
		}
	#endif
	
}