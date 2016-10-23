#include "display.h"
#include "externalProbes.h"
unsigned int disp_counter = 0;
unsigned int externalProbesCounter = 0;
SSD1306  display(0x3c, OLED_SDA, OLED_SCL);

void displayOled(void){
	if(disp_counter < 10){
		display_time();
	}else if(disp_counter < 20){
		externalProbesCounter = 30;
		displayTempAndHum();
	}else if((obPointArr[0]->config > 0) && ((disp_counter < (externalProbesCounter-8)))){
		displayExternalTitle(0);
	}else if((obPointArr[0]->config > 0) && ((disp_counter < externalProbesCounter))){
		display.clear();
		if(obPointArr[0]->config & 1){
			displayExternalTemp(0);
		}
		if(obPointArr[0]->config & 2){
			displayExternalHum(0);
		}
		if((unsigned int)obPointArr[0]->config & 4){
			displayExternalSoil(0);
		}
		display.display();
	}else{
		disp_counter = 0;
	}
	disp_counter++;
}
void displayExternalTitle(char measNum){
	display.clear();
	drawCenter((
		String("Sonda nr: ") + String((unsigned int)measNum)
	).c_str(),24,3,20);
	display.display();
}
void displayExternalSoil(char measNum){
	drawCenter((
		String("S:")+
		String((unsigned int)obPointArr[measNum]->hum_g)+
		String("%")
	).c_str(),24,3,40);
}
void displayExternalHum(char measNum){
	drawCenter((
		String("H:")+
		String((float)obPointArr[measNum]->hum/10)+
		String("%")
	).c_str(),24,3,20);
}
void displayExternalTemp(char measNum){

	drawCenter((
		String("T:")+
		String((float)obPointArr[measNum]->temp/10)+
		String("°C")
	).c_str(),24,3,0);

}

void displayTempAndHum(void){
	display.clear();
	displayAddres();
	drawCenter((
		String("T: ")+
		String((float)obPointArr[MAX_NUM_SEN]->temp/10)+
		String("°C")
	).c_str(),24,10,13);
	drawCenter((
		String("H: ")+
		String((float)obPointArr[MAX_NUM_SEN]->hum/10)+
		String("%")
	).c_str(),24,10,35);
	display.display();
}
void display_time(void){
	display.clear();
	displayAddres();
	drawCenter((
		String(hour())+":"+
		String(minute())+":"+
		String(second())
	).c_str(),24,25,13);
	drawCenter((
		String(day())+"-"+
		String(month())+"-"+
		String(year())
	).c_str(),16,28,35);
	#ifdef DEBUG
	drawCenter((
		String(disp_counter)
	).c_str(),10,0,13);
	#endif
	display.display();
	//drawCenter("10-09-2016",16,20,35);	
}
void displayAddres(void){
	drawCenter((String("http://")+
		String(0xFF & WiFi.localIP())+"."+
		String((0xFF00 & WiFi.localIP()) >> 8)+"."+
		String((0xFF0000 & WiFi.localIP()) >> 16)+"."+
		String((0xFF000000 & WiFi.localIP()) >> 24)
	).c_str(),10,0,0);
}
void drawCenter(const char * text, char size,char posx, char posy){
	display.setTextAlignment(TEXT_ALIGN_LEFT);
	if(size == 10){
		display.setFont(ArialMT_Plain_10);
	}
	if(size == 16){
		display.setFont(ArialMT_Plain_16);
	}
	if(size == 24){
		display.setFont(ArialMT_Plain_24);
	}
	display.drawString(0+posx, 0+posy, text);
	
}

void displayInit(void){
	//Oled
	display.init();
	display.displayOn();
	display.clear();
	display.flipScreenVertically();
	display.drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
	display.display();

}
void drawFoundSensor(const char * text){
	display.clear();
	drawCenter((
		String(text)
	).c_str(),24,10,13);
	display.display();
}