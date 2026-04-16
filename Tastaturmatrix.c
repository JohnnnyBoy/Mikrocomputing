//Initialprogramm der Tastaturmatrix

#include "STM32F4xx.h"
#include "_mcpr_stm32f407.h"
#include <inttypes.h>


void TastMatInit (void);
uint16_t ReadKeys(void);
void Short2BitString(char *taste, uint16_t keys);


// GPIOA und GPIOB bereits definiert
// GPIOB == Ausgang (PB4 -7) PIOA == Eingang (PA3-6)

#define PORTA_IDR *(uint16_t volatile *) 0x40020010
#define PORTB_ODR *(uint16_t volatile* ) 0x40020414
	
void wait_ud (int us) {
  for (int i=0; i<18*us; i++){}
}

void TastMatInit (void) {
	RCC->AHB1ENR |= (3 << 0); //Aktivierung der Ports A und B
	//MODER vorinitialisieren mit 0 ?
	GPIOA->MODER &= ~(0xFF << 2*3) ; //PA3-6 zu Eingänge (00) pro Hex 2 Pins
	GPIOB->MODER &= ~(0xFF << 2*4) ; //Vorinintialisierung mit 0
	GPIOB->MODER |= (0b01 << 2*4) | (0b01 << 2*5) | (0b01 << 2*6) | (0b01 << 2*7); //PB4-7 zu Ausgänge (01)
	GPIOB->OTYPER |= 0x00F0; // Open-Drain, damit Low Pegel aktiv nur geschaltet werden kann
	PORTB_ODR &= ~(0x00F0); // Ports auf LOW schalten
	GPIOA->PUPDR |= (0b01 << 2*3) | (0b01 << 2*4) | (0b01 << 2*5) | (0b01 << 2*6); // Pull-up Widerstände
	// GPIOA muss LOW standartmäßig sein
}

uint16_t ReadKeys(void){
	//Pin gedrückt = 1 -> HightPegel am Eingang und beim Ausgang der aktivierten Zeile
	
	uint16_t keys = 0;
	unsigned int iPB;
	
	for (iPB=4; iPB <=7; iPB++){
		PORTB_ODR |= (0x00F0); // Alle Ausgänge LOW
		PORTB_ODR &= ~(1 << iPB);
		
		wait_ud(10);
		
		keys = keys << 4; //Zeilentasten nach links schieben
		keys |= ((~PORTA_IDR & 0x0078)>>3);  //7 = 0111 8 = 1000 //Überprüfen ob die Reihenfolge stimmt bzw. Bitposition
																				 // >>3 damit rechtsbündig ausgegeben (Bit 0), sonst beginnend ab Bit 4
	}																		
	PORTB_ODR |= (0x00F0); // Alle Ausgänge LOW
	//keys = ~keys;
	return keys;
}

void Short2BitString(char taste[], uint16_t keys){
	for (int i = 0; i<=15; i++){
		if (1<<i & keys){
			taste[i] = '1';
		}
		else{
			taste[i] = '0';
		}
	}
	taste[16] = '\0';
}
