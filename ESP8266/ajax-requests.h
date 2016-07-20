#ifndef _Ajax_req_
#define _Ajax_req_

typedef struct times{
	int tim1Max;
	int tim1Min;
	char tim1;
	int tim2Max;
	int tim2Min;
	char tim2;
	int tim3Max;
	int tim3Min;
	char tim3;
}times_struct;

typedef struct temperature{
	char tempMax;
	char tempMaxOn;
	char tempMin;
	char tempMinOn;
	char DeltaT;
	unsigned short DeltaTim;
	unsigned short DeltaRelayTime;
	char DeltaTOn;
}temperature_struct;

typedef struct airhum{
	char wilgPowMax;
	char wilgPowMaxOn;
	char wilgPowMin;
	char wilgPowMinOn;
	char DeltaWilgPow;
	unsigned short DeltaWilgPowTim;
	unsigned short DeltaWilgPowRelayTim;
	char DeltaWilgPowOn;
}airHum_struct;

typedef struct hum{
	char wilgMax;
	char wilgMaxOn;
	char wilgMin;
	char wilgMinOn;
	char DeltaWilg;
	unsigned short DeltaWilgTim;
	unsigned short DeltaWilgRelayTim;
	char DeltaWilgOn;
}hum_struct;
//parametry ippobierane przy trarcie jeżeli dhcpOn = 0
typedef struct ethernet{
	char ip0;
	char ip1;
	char ip2;
	char ip3;
	char m0;
	char m1;
	char m2;
	char m3;
	char gat0;
	char gat1;
	char gat2;
	char gat3;
	char dns1_0;
	char dns1_1;
	char dns1_2;
	char dns1_3;
	char dns2_0;
	char dns2_1;
	char dns2_2;
	char dns2_3;
	char dhcpOn;
	String siec;
	String haslo;
}ethernet_struct;

typedef struct system{
	char workMod; //0-automatyczny, 1-zdefiniowany, 2-reczny
	char chandModeOff; //0-wylaczony, 1-po czasie, 2- wilg-max
	char chandModeHumTimeEnd; // jesli chandModeOff 1 to max 3600 [%] a jesli 2 to 100 [%]
	char timeMod; //0-czas z serwera 1- ręczny
	String NTP_addr;
	unsigned int year;
	char month;
	char day;
	char hour;
	char minute;
	char second;
}system_struct;

#endif