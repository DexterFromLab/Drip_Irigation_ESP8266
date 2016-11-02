#ifndef _EquationParser_
#define _EquationParser_

//#include <ESP8266WiFi.h>
#include <math.h>
#include <stdio.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "./definicje.h"
#include "./externalProbes.h"
#include <TimeLib.h>

//void reloadVirValues(void);


//#define DBG_OUTPUT_PORT Serial
enum EXPR_EVAL_ERR {
	EEE_NO_ERROR = 0,
	EEE_PARENTHESIS = 1,
	EEE_WRONG_CHAR = 2,
	EEE_DIVIDE_BY_ZERO = 3
};

typedef char EVAL_CHAR;
#define ZMIENNE_ZWRACANE 10  //wielkość tablicy zawierająca nazwy zmiennych zwracanych
class ExprEval {
private:
	EXPR_EVAL_ERR _err;
	EVAL_CHAR* _err_pos;
	int _paren_count;
	char wyrazenie[300];

	//zmienne systemowe do których mogą być zwracane wartości
	//char systemVirName[ZMIENNE_ZWRACANE][10] = {"P1","P2","P3","P4","P5","P6","P7","P8","P9","P10"};
	//double wyniki[ZMIENNE_ZWRACANE];
	// Parse a number or an expression in parenthesis
	double ParseAtom(EVAL_CHAR*& expr) {
		// Skip spaces
		while(*expr == ' ')
			expr++;

		// Handle the sign before parenthesis (or before number)
		bool negative = false;
		if(*expr == '-') {
			negative = true;
			expr++;
		}
		if(*expr == '+') {
			expr++;
		}

		// Check if there is parenthesis
		if(*expr == '(') {
			expr++;
			_paren_count++;
			double res = ParseSummands(expr);
			if(*expr != ')') {
				// Unmatched opening parenthesis
				_err = EEE_PARENTHESIS;
				_err_pos = expr;
				return 0;
			}
			expr++;
			_paren_count--;
			return negative ? -res : res;
		}

		// It should be a number; convert it to double
		char* end_ptr;
		double res = strtod(expr, &end_ptr);
		if(end_ptr == expr) {
			// Report error
			_err = EEE_WRONG_CHAR;
			_err_pos = expr;
			return 0;
		}
		// Advance the pointer and return the result
		expr = end_ptr;
		return negative ? -res : res;
	}

	// Parse multiplication, division, and, or , more, less, negation
	double ParseFactors(EVAL_CHAR*& expr) {
		double num1 = ParseAtom(expr);
		for(;;) {
			// Skip spaces
			while(*expr == ' ')
				expr++;
			// Save the operation and position
			EVAL_CHAR op = *expr;
			EVAL_CHAR* pos = expr;
			if(op != '/' && op != '*' && op != '&' && op != '|' && op != '>' && op != '<' && op != '!')
				return num1;
			expr++;
			if(op != '!'){	//czy istnieje negacja?
				double num2 = ParseAtom(expr);
				// Perform the saved operation
				if(op == '/') {
					// Handle division by zero
					if(num2 == 0) {
						_err = EEE_DIVIDE_BY_ZERO;
						_err_pos = pos;
						return 0;
					}
					num1 /= num2;
				}
				if(op == '*'){
					num1 *= num2;
				}
				if(op == '&'){
					if((num1>0) & (num2>0)){
						num1 = 1;
					}else{
						num1 = 0;
					}
				}
				if(op == '|'){
					if((num1>0) | (num2>0)){
						num1 = 1;
					}else{
						num1 = 0;
					}
				}
				if(op == '>'){
					if(num1>num2){
						num1 = 1;
					}else{
						num1 = 0;
					}
				}
				if(op == '<'){
					if(num1<num2){
						num1 = 1;
					}else{
						num1 = 0;
					}
				}
			}else{
				EVAL_CHAR op = *expr;
				EVAL_CHAR* pos = expr;
				if(op == '0' | op == '1' | op == '2'  | op == '3'  | op == '4'  | op == '5'  | op == '6' | op == '7' | op == '8'| op == '9'){
					//expr++;
					double num2 = ParseAtom(expr);
					if(num2>0) num2 = 0;
					else num2 = 1;
					num1 *= num2;
					return num1;
				}
				else if(op != '&' && op != '|')
					return num1;
				expr++;
				double num2 = ParseAtom(expr);
				if(op == '&'){
					if((num1>0) & (num2>0)){
						num1 = 0;
					}else{
						num1 = 1;
					}
				}
			}
		}
		return 0;
	}

	// Parse addition and subtraction
	double ParseSummands(EVAL_CHAR*& expr) {
		double num1 = ParseFactors(expr);
		for(;;) {
			// Skip spaces
			while(*expr == ' ')
				expr++;
			EVAL_CHAR op = *expr;
			if(op != '-' && op != '+')
				return num1;
			expr++;
			double num2 = ParseFactors(expr);
			if(op == '-')
				num1 -= num2;
			else
				num1 += num2;
		}
		return 0;
	}
	char * replace(
		char const * const original, 
		char const * const pattern, 
		char const * const replacement
	) {
	  size_t const replen = strlen(replacement);
	  size_t const patlen = strlen(pattern);
	  size_t const orilen = strlen(original);

	  size_t patcnt = 0;
	  const char * oriptr;
	  const char * patloc;

	  // find how many times the pattern occurs in the original string
	  for (oriptr = original; patloc = strstr(oriptr, pattern); oriptr = patloc + patlen)
	  {
		patcnt++;
	  }

	  {
		// allocate memory for the new string
		size_t const retlen = orilen + patcnt * (replen - patlen);
		char * const returned = (char *) malloc( sizeof(char) * (retlen + 1) );

		if (returned != NULL)
		{
		  // copy the original string, 
		  // replacing all the instances of the pattern
		  char * retptr = returned;
		  for (oriptr = original; patloc = strstr(oriptr, pattern); oriptr = patloc + patlen)
		  {
			size_t const skplen = patloc - oriptr;
			// copy the section until the occurence of the pattern
			strncpy(retptr, oriptr, skplen);
			retptr += skplen;
			// copy the replacement 
			strncpy(retptr, replacement, replen);
			retptr += replen;
		  }
		  // copy the rest of the string.
		  strcpy(retptr, oriptr);
		}
		return returned;
	  }
	  return 0;
	}
	//Check variables in equation string
	double CheckVirables(EVAL_CHAR* expr){
		//deklaracje nazw zmiennych w passerze, późńiej należy przypisać odpowiednia tablice z danymi
		//String virName[] = {"Test1","Test2","Test3","P1","P2","P3","P4","P5","P6","P7","P8","P9","P10"};
		//przypisanie wartości do zmiennych
		//double value[] = {1,2,3,wyniki[0],wyniki[1],wyniki[2],wyniki[3],wyniki[4],wyniki[5],wyniki[6],wyniki[7],wyniki[8],wyniki[9]};
		
		char liczba[20];
		char str_temp[6];
		unsigned int i;//counter
		double wynik;
		String bufor_rownania;
		
		DB2("Przed podstawieniem zmiennych do równania: "+String(ESP.getFreeHeap()));
		sprintf(wyrazenie,expr);
		DB2("Po sprintfie:                              "+String(ESP.getFreeHeap()));
		DB1("Rownanie i zmienne:                        " + String(wyrazenie));
		
		for(i=0;i<inputVirablesNames.size();i++){
			if(strstr(wyrazenie,inputVirablesNames[i].c_str()) > 0){
				dtostrf(inputVirablesValues[i], 4, 2, str_temp);//tutaj cos mi bruzdzi!
				sprintf(liczba,"%s",str_temp);
				
				bufor_rownania = String(wyrazenie);
				
				bufor_rownania.replace(String(inputVirablesNames[i]),String(liczba));
				
				//replace1(bufor_rownania,String(inputVirablesNames[i]),String(liczba));
				
				sprintf(wyrazenie,bufor_rownania.c_str());
				//sprintf(wyrazenie,bufor_rownania.c_str());
				//sprintf(wyrazenie,replace(wyrazenie,inputVirablesNames[i].c_str(),liczba));
			}
		}
		//DB2("Po podstawieniu zmiennych do równania "+String(ESP.getFreeHeap()));
		DB1("Liczby w równaniu: "+ String(wyrazenie));
		DB2("Po podstawieniu zmiennych do równania:     "+String(ESP.getFreeHeap()));
		wynik = Eval(wyrazenie);
		if(wynik > 1000000) wynik = 1000000;
		if(wynik < -1000000) wynik = -1000000;
		DB2("Po wykonaniu obliczeń:                     "+String(ESP.getFreeHeap()));
		return wynik;
		//return &wyrazenie[0];
		//*expr = *wyrazenie;
	}
	
	double Eval(EVAL_CHAR* expr) {
		_paren_count = 0;
		_err = EEE_NO_ERROR;
		
		
		double res = ParseSummands(expr);
		// Now, expr should point to '\0', and _paren_count should be zero
		if(_paren_count != 0 || *expr == ')') {
			_err = EEE_PARENTHESIS;
			_err_pos = expr;
			return 0;
		}
		if(*expr != '\0') {
			_err = EEE_WRONG_CHAR;
			_err_pos = expr;
			return 0;
		}
		return res;
	};
	public:
	
	double Count(EVAL_CHAR* expr){
		DB2("Po wej. w count: "+String(ESP.getFreeHeap()));
		String virNameS;
		char* wsk;
		wsk = strchr(expr,'=');
		if(wsk == NULL){
			return CheckVirables(expr);
		}
		*wsk = 0;
		virNameS = String(wsk+1);
		int i;
		DB2("Przed forem: "+String(ESP.getFreeHeap()));
		for(i =0; i<outputVirablesNames.size();i++){
			DB1(outputVirablesNames[i]);
			DB1(virNameS);
			if(String(outputVirablesNames[i]) == virNameS){
				//DB1("Równanie: " + String(expr) + " wynik:" + String(CheckVirables(expr)) +" i:" +String(i));
				outputVirablesValues[i] = CheckVirables(expr);
				
				//DB1(outputVirablesValues[i]);
				return outputVirablesValues[i];
			}
		}
		DB2("Po forze: "+String(ESP.getFreeHeap()));

		 return 0;
	}
	
	EXPR_EVAL_ERR GetErr() {
		return _err;
	}
	EVAL_CHAR* GetErrPos() {
		return _err_pos;
	}
};

class externalVirables{
	unsigned int beginSize = 0;
	public:
	void initSizeOfProbesValues(void){
		beginSize = inputVirablesNames.size();
		return;
	}
	void initInputVirablesNames(){
		//Dodawanie nazw zmiennych do tablicy
		inputVirablesNames.push_back("hour");		//nazwa
		inputVirablesValues.push_back((float)hour());			//init value
		inputVirablesNames.push_back("minute");		//nazwa
		inputVirablesValues.push_back((float)minute());			//init value
		inputVirablesNames.push_back("second");		//nazwa
		inputVirablesValues.push_back((float)second());			//init value
		inputVirablesNames.push_back("day");		//nazwa
		inputVirablesValues.push_back((float)day());			//init value
		inputVirablesNames.push_back("weekday");		//nazwa
		inputVirablesValues.push_back((float)weekday());			//init value
		inputVirablesNames.push_back("month");		//nazwa
		inputVirablesValues.push_back((float)month());			//init value
		inputVirablesNames.push_back("year");		//nazwa
		inputVirablesValues.push_back((float)year());			//init value
		inputVirablesNames.push_back("DaySec");		//nazwa
		inputVirablesValues.push_back(((float)hour()*3600)+((float)minute()*60)+(float)second());			//init value
		inputVirablesNames.push_back("Relay0");		//Relay on board
		inputVirablesValues.push_back(0);			//init value
		inputVirablesNames.push_back("Tmp1");		//nazwa
		inputVirablesValues.push_back(0);			//init value
		inputVirablesNames.push_back("Tmp2");
		inputVirablesValues.push_back(0);
		return;
	}
	void initOutputVirablesNames(void){
		outputVirablesNames.push_back("Relay0");	//Relay on board
		outputVirablesValues.push_back(0);			//init value
		outputVirablesNames.push_back("Tmp1");		//nazwa
		outputVirablesValues.push_back(0);			//init value
		outputVirablesNames.push_back("Tmp2");		
		outputVirablesValues.push_back(0);	
		return;
	}
	void reloadVirValues(void){
		//DB2("Przed przeladowaniem: "+String(ESP.getFreeHeap()));
		for(int i = 0; i<MAX_NUM_SEN + NUM_OF_INTERNAL_SENSOR;i++){
			obPointArr[i]->loadVirableValue();
		}
		//DB2("Po przeladowaniu: "+String(ESP.getFreeHeap()));
		//DB2("Przed przypisaniem 1234: "+String(ESP.getFreeHeap()));
		//Wczytywanie wskazanych danych do tablicy
		inputVirablesValues[beginSize+0] = (float)hour();
		inputVirablesValues[beginSize+1] = (float)minute();
		inputVirablesValues[beginSize+2] = (float)second();
		inputVirablesValues[beginSize+3] = (float)day();
		inputVirablesValues[beginSize+4] = (float)weekday();
		inputVirablesValues[beginSize+5] = (float)month();
		inputVirablesValues[beginSize+6] = (float)year();
		inputVirablesValues[beginSize+7] = (float)(((float)hour()*3600)+((float)minute()*60)+(float)second()); //sekunda dnia

		inputVirablesValues[beginSize+8] = outputVirablesValues[0];//ręczne przypisanie wartosci do nazwy zaraz po wartościach odczytanych z sond
		inputVirablesValues[beginSize+9] = outputVirablesValues[1];
		inputVirablesValues[beginSize+10] = outputVirablesValues[2];
		//Zmienne wyjsciowe
		
		digitalWrite(RELAY0, outputVirablesValues[0]);
		
	#ifdef DEBUG2
		DB2("Lista nazw zmiennych");
		for(int i = 0;i<inputVirablesNames.size();i++){
			DB2(String(inputVirablesNames.at(i)));
		}
		DB2("Wartosci zmiennych");
		for(int i = 0;i<inputVirablesValues.size();i++){
			DB2(inputVirablesValues[i]);
		}
	#endif
	//DB2("Po przypisaniu 1234: "+String(ESP.getFreeHeap()));
	return;
	}
	String generateValNamePairString(){
		reloadVirValues();
		String nameAndValues = "[";
		for(int i = 0;i<inputVirablesNames.size();i++){
			if(i != 0) nameAndValues += ",";
			nameAndValues += "{\"n\":\"";
			nameAndValues += String(inputVirablesNames[i]) +"\""; 
			nameAndValues += ",\"v\":\"" + String(inputVirablesValues[i]) +"\"}";
		}
		nameAndValues += "]";
		return nameAndValues;
	}
	String generateOutValNamePairString(){
		reloadVirValues();
		String nameAndValues = "[";
		for(int i = 0;i<outputVirablesNames.size();i++){
			if(i != 0) nameAndValues += ",";
			nameAndValues += "{\"n\":\"";
			nameAndValues += String(outputVirablesNames[i]) +"\""; 
			nameAndValues += ",\"v\":\"" + String(outputVirablesValues[i]) +"\"}";
		}
		nameAndValues += "]";
		return nameAndValues;
	}
};

extern externalVirables externVir;

#endif

