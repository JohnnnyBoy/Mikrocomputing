#include "STM32F4xx.h"
#include "_mcpr_stm32f407.h"
#include <inttypes.h>




void InitTIM7(void){
	RCC->APB1ENR |= RCC_APB1ENR_TIM7EN; //Timer 7 einschalten
	
	TIM7->PSC = 83; // PSC+1 = Prescaler
	TIM7->ARR = 999; // ARR+1 Größe des Autoreload Registers
	TIM7->CNT = 0x0000; // 16 Bit Countergöße auf 0 setzen
	
	TIM7->CR1 = 0x0005; // CEN (Counter Enable gesetzt) CNT mit Zähltakt verbunden + Interupt nur bei Overflow
	TIM1->EGR = 0x0001; // Interruptanforderung bei Overflow für TIM7 (UpdateEvent)
	
	TIM7->DIER = 0x0001; //Overflow Interrupt bei (UpdateEvent)
	
	NVIC_EnableIRQ(TIM7_IRQn); //Interrupt für TIM7 (55) im NVIC enablen
	NVIC_SetPriority(TIM7_IRQn, 4); //Priorität des Interrupt 8Bit Priority = 4*16 = 64
}

void InitTIM12(void){ 
	
	GPIOB->AFR[1] |= 0x99000000; //für PB14 und 15 die alternative function auswählen
	GPIOB->MODER  |= 0xA0000000; //Alternative Funktion für PB14 und 15 aktivieren
	
	RCC->APB1ENR |= RCC_APB1ENR_TIM12EN; //Timer 12 aktivieren
	
	TIM12->PSC = 83; //Für 84MHz auf 1us
	TIM12->ARR = 0xFFFF; //1us pro tick Auflösung -> Overflow in 65536 Bit * 1us pro Bit = 65,536ms
	
	TIM12->CNT = 0x0000; //Counter auf Null
	TIM12->SMCR = 0x0000; //keine externe Clock
	TIM12->CR1 = 0x0005; //Interrupt bei Overflow (Bit 2) und Run (Bit 0)
	
	TIM12->CCMR1 = 0x0101; //Capture Register für Ch1 und Ch2 aktiviert
	TIM12->CCER = 0x0033; //fallende Flanke für Capture
	
	TIM12->CCR1 = 0x0000; //Capture/Compare Register 1 löschen
  TIM12->CCR2 = 0x0000; //Capture/Compare Register 2 löschen
	
	TIM12->EGR = 0x0001; //Interruptanfrage um prescaler zu aktivieren
	TIM12->DIER = 0x0007; //CH1 und CH2 sowie Overflow können Interrupt auslösen
	
	NVIC_EnableIRQ(TIM8_BRK_TIM12_IRQn); //Interrupt für TIM12 (43) im NVIC enablen
	NVIC_SetPriority(TIM8_BRK_TIM12_IRQn, 5); // Priorität 5, 5*16 = 80

}

void InitTIM8(void){ 
	
	RCC->AHB1ENR |= (1<<2); //Port C aktivieren
	//GPIOB->AFR[1] |= 0x33000000; //für PB14 und 15 die alternative function auswählen
	GPIOB->MODER  |= 0x40000000; //Ausgang Funktion für PB14 und 15 aktivieren
	GPIOC->AFR[1] |= 0x00000033; //PC8 und PC9 mit alternative Funktion
	//GPIOC->AFR[0] |= 0x33000000; //PC7 und 6 mit alternativen Funktion 3
	GPIOC->MODER |= 0x000A0000; //PC8 und PC9 Alternative Funktion aktiviert
	
	RCC->APB2ENR |= RCC_APB2ENR_TIM8EN; //Timer 8 aktivieren
	
	TIM8->PSC = 167; //Für 168MHz auf 1us
	TIM8->ARR = 0xFFFF; //1us pro tick Auflösung -> Overflow in 65536 Bit * 1us pro Bit = 65,536ms
	
	TIM8->CNT = 0x0000; //Counter auf Null
	TIM8->SMCR = 0x0000; //keine externe Clock
	TIM8->CR1 = 0x0005; //Interrupt bei Overflow (Bit 2) und Run (Bit 0)
	
	//TIM8->CCMR1 = 0x0101; //Capture Register für Ch1 und Ch2 aktiviert
	TIM8->CCMR2 = 0x0101; //Capture Registere für CH3 und Ch4 aktiviert
	TIM8->CCER = 0x3300; //fallende Flanke für Capture
	
	//TIM8->CCR1 = 0x0000; //Capture/Compare Register 1 löschen
  //TIM8->CCR2 = 0x0000; //Capture/Compare Register 2 löschen
	TIM8->CCR3 = 0x0000; //Capture/Compare Register 3 löschen
  TIM8->CCR4 = 0x0000; //Capture/Compare Register 4 löschen
	
	TIM8->EGR = 0x0001; //Interruptanfrage um prescaler zu aktivieren
	TIM8->DIER = 0x0019; //Ch3, Ch4 sowie Overflow können Interrupt auslösen
	
	NVIC_EnableIRQ(TIM8_CC_IRQn); //Interrupt für TIM8 im NVIC enablen
	NVIC_SetPriority(TIM8_CC_IRQn, 5); // Priorität 5, 5*16 = 80

}
