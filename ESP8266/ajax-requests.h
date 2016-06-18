#ifndef _Ajax_req_
#define _Ajax_req_

typedef struct wilg_ster{
	int tim1Max;
	int tim1Min;
	char tim1;
	int tim2Max;
	int tim2Min;
	char tim2;
	int tim3Max;
	int tim3Min;
	char tim3;
}wilg_ster_struct;

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

typedef struct ethernet{
	char ip0;
	char ip1;
	char ip2;
	char ip3;
	char m0;
	char m1;
	char m2;
	char m3;
	char mac0;
	char mac1;
	char mac2;
	char mac3;
	char mac4;
	char mac5;
	char dns0;
	char dns1;
	char dns2;
	char dns3;
	char dhcp0;
	char dhcp1;
	char dhcp2;
	char dhcp3;
	char dhcpOn;
}ethernet_struct;

typedef struct system{
	char workMod; //0-automatyczny, 1-zdefiniowany, 2-reczny
	char chandModeOff; //0-wylaczony, 1-po czasie, 2- wilg-max
	char chandModeHumTimeEnd; // jesli chandModeOff 1 to max 3600 [%] a jesli 2 to 100 [%]
	char NTP_addr[40];
	unsigned int POSIX_time; //Czas w formacie posix sekundy ktore minely od 1 stycznia 1970 roku
	
}system_struct;
#endif