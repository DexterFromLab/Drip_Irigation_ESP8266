#define OUT	Serial

//#define DEBUG2				//Czy debug jest włączony?

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
#define	FREE_RAM				10000		//wolny ram po przydzieleniu danych do tablic pomiarowych

//Pinmap

#define RELAY0	17//relay on board
#define ApClientPin 16 //jesli 1 to lacze sie z siecia jesli 0 tworze wlasna

#define DHTPIN 13     // what digital pin we're connected to
// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
// Set pins:  CE, IO,CLK
#define RTC_RESET	12
#define RTC_CLK 5
#define RTC_DATA 4

// D3 -> SDA
// D5 -> SCL
#define OLED_SDA	D3
#define OLED_SCL	D5
