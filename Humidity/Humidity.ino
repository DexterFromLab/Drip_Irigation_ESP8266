/*
Odczyt danych pomiarowych z sondy po RS-485

Odczyt danych rozpoczyna się od pojawienia się na magistrali znaku #
Po tym znaku, należy podać adres urządzenia odpytywanego na magistralii.
W wypadku braku pozostałych danych lub niezgodności zwrócona zostanie
konfiguracja w jakiej sonda pracuje. 

Example:
#7 - adres urządzenia na magistrali
7 - konfiguracja urządzenia, podpięte czujniki t,h,g

#7$ - Pytanie o wszystkie pomiary
33  - Temperatura
45	- Wilg pow
23	- wilg gleby

#7$t - Pytanie o pojedyńczy pomiarowych (t,h,g - temp powietrza, wilg. pow., wilg. gleb.)
33 - Temp powietrza

Konfiguracja urządzenia

Do czujnika podpięte mogą być różne urządzenia. Ich konfiguracja oznacza liczba binarna
której poszczególne bity odpowiadają czujnikom:

bit0 - czujnik temperatury
bit1 - czujnik wilg pow
bit2 - czujnik wilg gleb
bit16 - przekaźnik P0
bit17 - przekaźnik P1
bit18 - przekaźnik P2
bit19 - przekaźnik P3
Mapa konfiguracj podlega ciągłej zmianie w zależności od ilości dostęnych modułów 
rozszerzeń.

Konfiguracja wyjść przekaźnikowych.
Konfiguracja wyjść przekaźnikowych zawarta jest w liczbie po znaku %.
Example:
#7$a%16
Spowoduje odczytanie wszystkich pomiarów z sondy o adresie 7, i ustawienie
4 przekaźników w stan wysoki 

DEC(16) = 0b00001111   - P3 P2 P1 P0
*/
#define BAUDRATE 9600		//prędkość transmisji
#define DEBUG				//Czy debug jest włączony?

#ifdef DEBUG
	#define DB1(x)	Serial.println(x)
#endif

#ifndef DEBUG
	#define DB1(x)
#endif;

#include <DHT.h>
//definiowanie stałych
#define DHTPIN 2
#define DHTTYPE DHT11
//definicja czujnika
DHT dht11(DHTPIN, DHTTYPE);
//zmienne
int Wilgotnosc_gleba;
int Temperatura;
int Wilgotnosc;
char Rozkaz[20];
int Adres;
//char Adres_sonda[4];
char Komenda;
char *wsk;
int i;
int licznik = 0;
void setup() {
  //Piny do konfiguracji adresu na magistrsli RS-485
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  //Piny do odczytu konfiguracji sondy
  pinMode(6, INPUT);// - Pomiar temperatury
  pinMode(7, INPUT);// - Pomiar wilg. pow.
  pinMode(8, INPUT);// - Pomiar wilg. gl.
  Serial.begin(BAUDRATE);
  dht11.begin();
  DB1("Debug 1 dziala!");
  //sprawdz jaki jest adres sondy na zworce i napisz w serialu
  //Sprawdz_adres();
}

void loop() {
  //sprawdzam czy cos moge odczytac
  if (Serial.available() > 0)
  {
	//Zerowanie tablicy znaków z rozkazami
	for(i = 0;i<sizeof(Rozkaz);i++){
		Rozkaz[i] = 0;
	}
	if(Serial.read() == '#'){
		Serial.readBytes(Rozkaz, 20);
		Adres = atoi(Rozkaz);
		DB1("Adres odebrany: " + String(Adres));
		if(Adres == Sprawdz_adres()){
			wsk = strstr(Rozkaz,"$");
			if(wsk>0){
			wsk += 1;
				if((*wsk)=='t'){
					Odczyt_temperatura();
				}
				else if((*wsk)=='h'){
					Odczyt_wilgotnosc();
				}
				else if((*wsk)=='g'){
					Odczyt_gleba();
				}
				else if((*wsk)=='%' || (*wsk)==0){
					Odczyt_wszystko();
				}
				wsk = strstr(Rozkaz,"%");
				if(wsk>0){
					wsk += 1;
					DB1("Ustaw przekazniki w stan: " + String(atoi(wsk))); 
					//Tutaj przypisać odpowiedni stan przekaźnikom
					
				}
			}else{
				Serial.println(Sprawdz_konfig());
			}
		  }else{
			  DB1("Zly adres czujnika");
		  }
		}else{
			
		}
	}
}
//funkcja odpowiedzialna za odczyt wilgoci z czujnika gleby
void Odczyt_gleba(void) {
	//Sprawdzam czy urządzenie skonfigurowano do pomiaru, jeśli nie nie podejmój próby
	if(digitalRead(8)){
	  Wilgotnosc_gleba = analogRead(A7);

	  int Wilgotnosc_procenty = map(Wilgotnosc_gleba,0,1023,0,100);
	  DB1("Wilgotnosc gleby wynosi: ");
	  Serial.println(Wilgotnosc_procenty);
	  DB1("");
	  licznik = 0;
	}
}
//funkcja odpowiedzialna za odczyt temperatury z czujnika dht11
void Odczyt_temperatura(void) {
	if(digitalRead(6)){
	  Temperatura = dht11.readTemperature();
	  DB1("Aktualna temperatura wynosi: ");
	  Serial.println(Temperatura);
	  DB1(" st.C.");
	  licznik = 0;
	}
}
//funkcja odpowiedzialna za odczyt wilgoci z czujnika dht11
void Odczyt_wilgotnosc(void) {
	if(digitalRead(7)){
	  Wilgotnosc = dht11.readHumidity();
	  DB1("Wilgotnosc powietrza wynosi: ");
	  Serial.println(Wilgotnosc);
	  DB1(" %");
	  licznik = 0;
	}
}
//odczyt danych ze wszystkiego na raz
void Odczyt_wszystko(void) {
  Odczyt_temperatura();
  delay(50);
  Odczyt_wilgotnosc();
  delay(50);
  Odczyt_gleba();
  delay(50);
}
int Sprawdz_adres(void) {
  int numerPortu;
  
  numerPortu = digitalRead(3) + (digitalRead(4)<<1) + (digitalRead(5)<<2);
  DB1("Adres sondy to: "+ String(numerPortu));

  return numerPortu;
}
int Sprawdz_konfig(void) {
  int konfig;
  
  konfig = digitalRead(6) + (digitalRead(7)<<1) + (digitalRead(8)<<2);
  DB1("Konfiguracja sondy: "+ String(konfig));

  return konfig;
}
