#define OUT	Serial

#define DEBUG2				//Czy debug jest włączony?

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

#define MAX_NUM_SEN				10			//Maximum number of sensor
#define BAUDRATE				9600
#define SERIAL_TIMEOUT			300
#define SERIAL_TIMEOUT1			30        //Delay timeouts between sensors asking
#define	FREE_RAM				8000		//wolny ram po przydzieleniu danych do tablic pomiarowych