#ifndef _EXTERNAL_PROBES_

#include <vector>
#include "./definicje.h"
#include <ESP8266WiFi.h>


//deklaracje klass sensorow i urz. wyk

extern std::vector <String>		inputVirablesNames;
extern std::vector <float>		inputVirablesValues;
extern std::vector <String>		outputVirablesNames;
extern std::vector <float>		outputVirablesValues;

extern void readMagistralConfig(void);
extern void countAllocMeasuresSize(void);

extern char response[20];

extern void readMagistralConfig(void);
extern void countAllocMeasuresSize(void);

class sensorExecutorObiect{
	public:
	short temp;
	short hum;
	char hum_g;

	unsigned int config;	
	unsigned int addr;
	unsigned int classSize;
	unsigned int measCount;
	unsigned int numOfMeasure;
	
	
	void init(unsigned int conf, unsigned int address){
		classSize = sizeof(sensorExecutorObiect);
		config = conf;
		addr = address;
		measCount = 0;
	}
	
	void readMeasurments(void){
		char * wsk;
		if(config>0){
			response[0]=0;
			OUT.print("#"+String(addr)+String("$a"));
			//Ponawiamy probe odebarania odpowiedzi, czasami moze sie zdazyc ze czujnik akurat
			//zbiera dane z otoczenia i wowczas potrzebna jest mu wieksza przerwa
			for(int i = 0;(i<20)&&(strchr(response,'&') == NULL);i++){
				OUT.readBytes(response,20);
				if((wsk = strchr(response,'&')) != NULL){
					if(config&1){
						wsk = strchr(response,'t');
						wsk++;
						temp = short(atof(wsk)*10);
						DB1("T"+String(addr)+":"+String(temp));
					}
					if(config&2){
						wsk = strchr(response,'h');
						wsk++;
						hum = short(atof(wsk)*10);
						DB1("H"+String(addr)+":"+String(hum));
					}
					if(config&4){
						wsk = strchr(response,'g');
						wsk++;
						hum_g = short(atof(wsk));
						DB1("G"+String(addr)+":"+String((short)hum_g));
					}
				}
			}
		}
		putMeasureToTables();
	}
	void putMeasureToTables(void){
		if(config>0){
			if(measCount<numOfMeasure){
				if(config&1) *(tab_temp+measCount) = temp;
				if(config&2) *(tab_hum+measCount) = hum;
				if(config&4) *(tab_hum_g+measCount) = hum_g;
				measCount++;
			}else{
				for(int i = 0;i<(numOfMeasure - 1);i++){
					if(config&1) *(tab_temp+i) = *(tab_temp+i + 1);
					if(config&2) *(tab_hum+i) = *(tab_hum+i + 1);
					if(config&4) *(tab_hum_g+i) = *(tab_hum_g+i + 1);
				}
				*(tab_temp+numOfMeasure-1) = temp;
				*(tab_hum+numOfMeasure-1) = hum;
				*(tab_hum_g+numOfMeasure-1) = hum_g;
				#ifdef DEBUG2
				DB2("Used numOfMeasure="+String(numOfMeasure));
				for(int i = 0;i<numOfMeasure;i++){
					DB2("T"+String(i)+":"+String(float(*(tab_temp+i))/10) + " " +
						"H"+String(i)+":"+String(float(*(tab_hum+i))/10)+ " " +
						"G"+String(i)+":"+String(short(*(tab_hum_g+i)))
					);
					ESP.wdtFeed();
				}
				delay(1000);
				#endif
			}
		}
	}
	unsigned int sizeOfClass(void){
		classSize = 0;
		if(config & 1) classSize = sizeof(short);			//We are counting class size
		if(config & 2) classSize += sizeof(short);
		if(config & 4) classSize += sizeof(char);
		return classSize;
	}
	
	
	short * tab_temp;
	short * tab_hum;
	char * tab_hum_g;
	//Numery kolejnych zmiennych w tablicy danych
	char tempNum;
	char humNum;
	char hum_gNum;
	//Funkcja tworzaca tablice danych w zaleznosci od konfiguracji urzadzenia
	void create_tables(unsigned int measuresNum){
		numOfMeasure = measuresNum;
		DB2("Created numOfMeasure="+String(numOfMeasure));
		if(config & 1){
			tab_temp = new short[numOfMeasure];			//tablica pomiarow
			tempNum = inputVirablesNames.size();
			inputVirablesNames.push_back("Temp"+String(addr));				//wektor danych wejściowych
			inputVirablesValues.push_back(0);
		}
		if(config & 2){
			tab_hum = new short[numOfMeasure];
			humNum = inputVirablesNames.size();
			inputVirablesNames.push_back("Hum"+String(addr));
			inputVirablesValues.push_back(0);
		}
		if(config & 4){
			tab_hum_g = new char[numOfMeasure];
			hum_gNum = inputVirablesNames.size();
			inputVirablesNames.push_back("SoilH"+String(addr));
			inputVirablesValues.push_back(0);
		}
	}
	
	//Funkcja przypisujaca wartosc aktualnych pomiarow do zmiennych lokalnych
	void loadVirableValue(void){
		char counter = 0;
		if(config & 1){
			inputVirablesValues[tempNum] = temp/10;
			DB2("Temp"+String(addr)+": "+String(inputVirablesValues[tempNum]));
			}
		if(config & 2){
			inputVirablesValues[humNum] = hum/10;
			DB2("hum"+String(addr)+": "+String(inputVirablesValues[humNum]));
			}
		if(config & 4){
			inputVirablesValues[hum_gNum] = hum_g/10;
			DB2("hum_g"+String(addr)+": "+String(inputVirablesValues[hum_gNum]));
			} 
	}
	//Funkcja tworzaca odpowiedz na zapytanie ajaxa o zawartosc probek w archiwum
	String getAjaxMeasuredValues(char next){
		String json = "";
		if(config & 1){
			if(next) json += ",";
			json += "{\"Nam\": \"temp"+String(addr)+"\"";
			json += ",\"Count\": ";
			json += String(measCount);
			for(int i = 0;i<measCount;i++){
				json += ",\"V"+String(i)+"\":" + String((float)(tab_temp[i]/10));
			}
			json += "}";
		}
		if(config & 2){
			if((config & 1)  ||next) json += ",";
			json += "{\"Nam\": \"hum"+String(addr)+"\"";
			json += ",\"Count\": ";
			json += String(measCount);
			for(int i = 0;i<measCount;i++){
				json += ",\"V"+String(i)+"\":" + String((float)(tab_hum[i]/10));
			}
			json += "}";
		}
		if(config & 4){
			if((config & 1)||(config & 2) || next) json += ",";
			json += "{\"Nam\": \"hum_g"+String(addr)+"\"";
			json += ",\"Count\": ";
			json += String(measCount);
			for(int i = 0;i<measCount;i++){
				json += ",\"V"+String(i)+"\":" + String((float)((unsigned int)tab_hum_g[i]/10));
			}
			json += "}";
		}
		return json;
	}
};

extern sensorExecutorObiect * obPointArr[MAX_NUM_SEN];

#define _EXTERNAL_PROBES_
#endif