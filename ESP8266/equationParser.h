#ifndef _EquationParser_
#define _EquationParser_

#include <ESP8266WiFi.h>
#include<math.h>
#include<stdio.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#define DBG_OUTPUT_PORT Serial
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
	char systemVirName[ZMIENNE_ZWRACANE][10] = {"P1","P2","P3","P4","P5","P6","P7","P8","P9","P10"};
	double wyniki[ZMIENNE_ZWRACANE];
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
	}
	
	//Check variables in equation string
	double CheckVirables(EVAL_CHAR* expr){
		//deklaracje nazw zmiennych w passerze, późńiej należy przypisać odpowiednia tablice z danymi
		String virName[] = {"Test1","Test2","Test3","P1","P2","P3","P4","P5","P6","P7","P8","P9","P10"};
		//przypisanie wartości do zmiennych
		double value[] = {1,2,3,wyniki[0],wyniki[1],wyniki[2],wyniki[3],wyniki[4],wyniki[5],wyniki[6],wyniki[7],wyniki[8],wyniki[9]};
		
		char liczba[20];
		char str_temp[6];
		unsigned int i;//counter
		double wynik;
		
		sprintf(wyrazenie,expr);
		
		DBG_OUTPUT_PORT.println("Rownanie i zmienne: " + String(wyrazenie));
		
		for(i=0;i<sizeof(value)/sizeof(value[0]);i++){
			if(strstr(wyrazenie,virName[i].c_str()) > 0){
				dtostrf(value[i], 4, 2, str_temp);
				sprintf(liczba,"%s",str_temp);
				sprintf(wyrazenie,replace(wyrazenie,virName[i].c_str(),liczba));
			}
		}

		DBG_OUTPUT_PORT.println("Liczby w równaniu: "+ String(wyrazenie));
		
		wynik = Eval(wyrazenie);
		if(wynik > 1000000) wynik = 1000000;
		if(wynik < -1000000) wynik = -1000000;
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
		String virNameS;
		char* wsk;
		wsk = strchr(expr,'=');
		if(wsk == NULL){
			return CheckVirables(expr);
		}
		*wsk = 0;
		virNameS = String(wsk+1);
		int i;
		
		for(i =0; i<ZMIENNE_ZWRACANE;i++){
			DBG_OUTPUT_PORT.println(systemVirName[i]);
			DBG_OUTPUT_PORT.println(virNameS);
			if(String(systemVirName[i]) == virNameS){
				//DBG_OUTPUT_PORT.println("Równanie: " + String(expr) + " wynik:" + String(CheckVirables(expr)) +" i:" +String(i));
				wyniki[i] = CheckVirables(expr);
				
				//DBG_OUTPUT_PORT.println(wyniki[i]);
				return wyniki[i];
			}
		}

		 return 0;
	}
	
	EXPR_EVAL_ERR GetErr() {
		return _err;
	}
	EVAL_CHAR* GetErrPos() {
		return _err_pos;
	}
};

#endif

