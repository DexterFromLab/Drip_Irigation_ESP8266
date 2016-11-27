#ifndef _Ajax_req_
#define _Ajax_req_

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
	char timeMod; //0-czas z serwera 1- rÄ™czny
	String NTP_addr;
	unsigned int year;
	char month;
	char day;
	char hour;
	char minute;
	char second;
	
	char timeZone;
	unsigned int measInt;
	
	String workingScript;
	unsigned int relayScriptTime;
}system_struct;

extern ethernet_struct ethernet;
extern system_struct System_s;
#endif

