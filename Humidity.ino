#include <DHT.h>
//definiowanie stałych
#define DHTPIN 2
#define DHTTYPE DHT11
//definicja czujnika
DHT dht11(DHTPIN, DHTTYPE);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  dht11.begin();
}
//zmienne
int Wilgotnosc_gleba;
int Temperatura;
int Wilgotnosc;
char Komenda[2];
void loop() {
  //sprawdzam czy cos moge odczytac
  if (Serial.available() > 0)
  {
    //czytam dwa bity rozkazu
    Serial.readBytes(Komenda, 2);
    if (Komenda[0] == '%')
    {
      if (Komenda[1] == 'g')
      {
        Odczyt_gleba();
      }
      else if (Komenda[1] == 't')
      {
        Odczyt_temperatura();
      }
      else if (Komenda[1] == 'h')
      {
        Odczyt_wilgotnosc();
      }
    }
    else
    {
      Serial.println("Niepoprawna komenda");
    }
  }
}
//funkcja odpowiedzialna za odczyt wilgoci z czujnika gleby
void Odczyt_gleba(void) {
  Wilgotnosc_gleba = analogRead(A7);
  Serial.print("Wilgotnosc gleby wynosi: ");
  Serial.println(Wilgotnosc_gleba);
}
//funkcja odpowiedzialna za odczyt temperatury z czujnika dht11
void Odczyt_temperatura(void) {
  Temperatura = dht11.readTemperature();
  Serial.print("Aktualna temperatura wynosi: ");
  Serial.print(Temperatura);
  Serial.println(" st.C.");
}
//funkcja odpowiedzialna za odczyt wilgoci z czujnika dht11
void Odczyt_wilgotnosc(void) {
  Wilgotnosc = dht11.readHumidity();
  Serial.print("Aktualna wilgotnosc wynosi: ");
  Serial.print(Wilgotnosc);
  Serial.println(" %");
}


