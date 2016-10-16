#include "display.h"

#ifdef DEBUG
unsigned int disp_counter = 0;
#endif

SSD1306  display(0x3c, OLED_SDA, OLED_SCL);

void displayOled(void){
	display_time();
}
void display_time(void){
	display.clear();
	drawCenter((String("http://")+
		String(0xFF & WiFi.localIP())+"."+
		String((0xFF00 & WiFi.localIP()) >> 8)+"."+
		String((0xFF0000 & WiFi.localIP()) >> 16)+"."+
		String((0xFF000000 & WiFi.localIP()) >> 24)
	).c_str(),10,0,0);
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
	disp_counter++;
	#endif
	display.display();
	//drawCenter("10-09-2016",16,20,35);	
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