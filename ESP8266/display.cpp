#include "display.h"
#include "externalProbes.h"
#include "ajax-requests.h"
#include <time.h>
unsigned int disp_counter = 0;
int externalProbesCounter = 0;
SSD1306  display(0x3c, OLED_SDA, OLED_SCL);

void displayOled(void){
	if(disp_counter < 10){
		display_time();
		//disp_counter++;
	}else if(disp_counter < 20){
		displayTempAndHum();
		//disp_counter++;
	}else if(disp_counter < 22){
		if(obPointArr[0]->config == 0) disp_counter += 10; else
		displayExternalTitle(0);
		//disp_counter++;
	}else if(disp_counter < 30){
		displayExternalValues(0);
		//disp_counter++;
	}else if(disp_counter < 32){
		if(obPointArr[1]->config == 0) disp_counter += 10; else
		displayExternalTitle(1);
		//disp_counter++;
	}else if(disp_counter < 40){
		displayExternalValues(1);
		//disp_counter++;
	}else if(disp_counter < 42){
		if(obPointArr[2]->config == 0) disp_counter += 10; else
		displayExternalTitle(2);
		//disp_counter++;
	}else if(disp_counter < 50){
		displayExternalValues(2);
		//disp_counter++;
	}else if(disp_counter < 52){
		if(obPointArr[3]->config == 0) disp_counter += 10; else
		displayExternalTitle(3);
		//disp_counter++;
	}else if(disp_counter < 60){
		displayExternalValues(3);
		//disp_counter++;
	}else if(disp_counter < 62){
		if(obPointArr[4]->config == 0) disp_counter += 10; else
		displayExternalTitle(4);
		//disp_counter++;
	}else if(disp_counter < 70){
		displayExternalValues(4);
		//disp_counter++;
	}else if(disp_counter < 72){
		if(obPointArr[5]->config == 0) disp_counter += 10; else
		displayExternalTitle(5);
		//disp_counter++;
	}else if(disp_counter < 80){
		displayExternalValues(5);
		//disp_counter++;
	}else if(disp_counter < 82){
		if(obPointArr[6]->config == 0) disp_counter += 10; else
		displayExternalTitle(6);
		//disp_counter++;
	}else if(disp_counter < 90){
		displayExternalValues(6);
		//disp_counter++;
	}else if(disp_counter < 92){
		if(obPointArr[7]->config == 0) disp_counter += 10; else
		displayExternalTitle(7);
		//disp_counter++;
	}else if(disp_counter < 100){
		displayExternalValues(7);
		//disp_counter++;
	}else if(disp_counter < 102){
		if(obPointArr[8]->config == 0) disp_counter += 10; else
		displayExternalTitle(8);
		//disp_counter++;
	}else if(disp_counter < 110){
		displayExternalValues(8);
		//disp_counter++;
	}else if(disp_counter < 112){
		if(obPointArr[9]->config == 0) disp_counter += 10; else
		displayExternalTitle(9);
		//disp_counter++;
	}else if(disp_counter < 120){
		displayExternalValues(9);
		//disp_counter++;
	}
	if(disp_counter == 19){
		externalProbesCounter = 0;
		for(int i = 0;i<=9;i++){
			if(obPointArr[i]->config > 0) externalProbesCounter += 10;
		}
	}
	if(disp_counter > 20){
		externalProbesCounter--;
		if(externalProbesCounter < 1) disp_counter = 0;
	}
	disp_counter++;
}
void displayExternalValues(char measNum){
	display.clear();
	if(obPointArr[measNum]->config & 1){
		displayExternalTemp(measNum);
	}
	if(obPointArr[measNum]->config & 2){
		displayExternalHum(measNum);
	}
	if((unsigned int)obPointArr[measNum]->config & 4){
		displayExternalSoil(measNum);
	}
	display.display();
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
	char time[9];
	char date[11];
	sprintf(time,"%02d:%02d:%02d",hour(),minute(),second());
	sprintf(date,"%02d-%02d-%04d",day(),month(),year());
	drawCenter((
		String(time)
	).c_str(),24,25,13);
	drawCenter((
		String(date)
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
		String((int)ethernet.ip0)+"."+
		String((int)ethernet.ip1)+"."+
		String((int)ethernet.ip2)+"."+
		String((int)ethernet.ip3)
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