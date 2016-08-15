#define OUT 					Serial
#define MAX_NUM_SEN				10			//Maximum number of sensor
#define BAUDRATE				9600
#define SERIAL_TIMEOUT			300
#define SERIAL_TIMEOUT1			30        //Delay timeouts between sensors asking

#define DEBUG2				//Czy debug jest w³¹czony?

#ifdef DEBUG
	#define DB1(x)	Serial.println(x)
#endif
#ifdef DEBUG2
	#define DB1(x)	Serial.println(x)
	#define DB2(x)	Serial.println(x)
	#define SERIAL_TIMEOUT1		300
#endif
#ifndef DEBUG
		#ifndef DEBUG2
			#define DB1(x)
		#endif;
#endif;
#ifndef DEBUG2
	#define DB2(x)
#endif;

char response[20];

//deklaracje klass sensorow i urz. wyk

class sensorExecutorObiect{
	public:
	float temp;
	float hum;
	float hum_g;

	unsigned int config;	
	unsigned int addr;
	unsigned int classSize;
	unsigned int measCount;
	
	#define numOfMeasure	100 //ilosc pomiarow odkladanych do tablicy
	
	void init(unsigned int conf, unsigned int address){
		classSize = sizeof(sensorExecutorObiect);
		config = conf;
		addr = address;
		create_tables();
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
						temp = atof(wsk);
						DB1("T"+String(addr)+":"+String(temp));
					}
					if(config&2){
						wsk = strchr(response,'h');
						wsk++;
						hum = atof(wsk);
						DB1("H"+String(addr)+":"+String(hum));
					}
					if(config&4){
						wsk = strchr(response,'g');
						wsk++;
						hum_g = atof(wsk);
						DB1("G"+String(addr)+":"+String(hum_g));
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
				for(int i = 0;i<numOfMeasure;i++){
					DB2("T"+String(i)+":"+String(*(tab_temp+i)) + " " +
						"H"+String(i)+":"+String(*(tab_hum+i))+ " " +
						"G"+String(i)+":"+String(*(tab_hum_g+i))
					);
					ESP.wdtFeed();
				}
				delay(1000);
				#endif
			}
		}
	}
	unsigned int sizeOfClass(void){
		return classSize;
	}
	private:	
	
	float * tab_temp;
	float * tab_hum;
	float * tab_hum_g;
	
	//Funkcja tworzaca tablice danych w zaleznosci od konfiguracji urzadzenia
	void create_tables(void){
		
		
		if(config & 1){
			tab_temp = new float[numOfMeasure];
			classSize += sizeof(float)* numOfMeasure;			//We are counting class size
		}
		if(config & 2){
			tab_hum = new float[numOfMeasure];
			classSize += sizeof(float)* numOfMeasure;
		}
		if(config & 4){
			tab_hum_g = new float[numOfMeasure];
			classSize += sizeof(float)* numOfMeasure;
		}
	}
	
};

//int sensorConfig[MAX_NUM_SEN];		//Array with read sensors config
sensorExecutorObiect * obPointArr[MAX_NUM_SEN];		//Array of pointers to sensor obiects

void setup(void){
	OUT.begin(BAUDRATE);
	OUT.setTimeout(SERIAL_TIMEOUT);
	readMagistralConfig();
	OUT.setTimeout(SERIAL_TIMEOUT1);
	
} 

void loop(void){
	for(int i =0 ;i<MAX_NUM_SEN;i++){
		obPointArr[i]->readMeasurments();
	}
}

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
		}else{
			obPointArr[i] = new sensorExecutorObiect;
			obPointArr[i]->init(0,i);
			
			DB1("Obiect nr." +String(i)+" created with config: "+String(obPointArr[i]->config));
		}
		
	}
	
	for(int i = 0 ; i < MAX_NUM_SEN ; i++){
		DB1("Size ob."+String(i)+": " + String(obPointArr[i]->sizeOfClass()));
	}
	DB1("ESP free heap: "+String(ESP.getFreeHeap()));
}