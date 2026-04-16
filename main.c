#include "STM32F4xx.h"
#include "_mcpr_stm32f407.h"
#include "stdio.h"
#include "timer.h"
#include <string.h>

/*---------------------------------------------------------
  MAIN function
  Praktikum: Blinky implementiert, 
	Lorenz Gaul, 14.03.2023
	Keil MDK-ARM µVision V5.38
 *---------------------------------------------------------*/


#define ZPOLLDELAY 50000                                  // Verzögerungszeit Polling (ca. 50000µs)
#define Z05s  500000/ZPOLLDELAY                           // 0.5s auf Basis Pollingdurchlauf
#define LEDS  *(unsigned short volatile *) 0x60120000     // Zugriff auf die 16 LEDs auf dem Praktikumsboard
#define SLAVEID 0x0C  																		// Tisch 12
	
void LCD_Init(void);
void LCD_SetCursor (uint32_t x, uint32_t y);
void LCD_DrawPixel (uint16_t color);
void LCD_ClearDisplay (uint16_t color);
void LCD_WriteLetter (char zeichen, uint16_t fgcol, uint16_t bgcol, uint32_t xpos, uint32_t ypos);
void LCD_WriteString (char string[], uint16_t fgcol, uint16_t bgcol, uint32_t xpos, uint32_t ypos);
void TastMatInit (void);
uint16_t ReadKeys(void);
void Short2BitString(char *taste, uint16_t keys);


// Globale Variablen
int ms = 0;
int sflag = 0;
int sflag2 = 0;
int poti_1 = 0;
int poti_2 = 0; 

uint16_t status = 0x0000;
uint16_t capture1 = 0x0000;
uint16_t capture2 = 0x0000;
uint16_t deltaT = 0;
char tempflag = 0;
uint16_t key; 
uint16_t temperatur = 0;

void wait_us (int);

//LIN-Bus Portkonfiguration
void LIN_Init(void){
	
	// LIN
	GPIOC->MODER |= (1<<12);//Config PC6 as an Output
	GPIOC->ODR |= (1<<6); // Set PC6(TXD) to High Voltage
	GPIOB->MODER |= (1<<4);// Config PB2 as an Output
	//GPIOB->ODR &= ~(1<<2); // Set PB2 to LowVoltage (NSLP)
	GPIOB->ODR |= (1<<2); // Set PB2 to HighVoltage
	//GPIOB->ODR &= ~(1<<2); // Reset PB2
	// Ports config
		//PC6, PC7 auf AF
	GPIOC->MODER |= (1<<13)|(1<<15);
	GPIOC->MODER &= ~(1<<12);
	GPIOC->AFR[0] |= 0x88000000; //(1<<27)|(1<<31); 
	// USART
	RCC-> APB2ENR |= (1<<5); // USART Takt enable
	USART6->BRR  = 0x1117; // Set Baudrate
	USART6->CR1 |= (0x206C); // Reciever, Tranciever enable | USART Enable
	USART6->CR2 |= (1<<14) | (1<<5) | (1<<6); // LIN enable | LBDL | LBDIE
	NVIC_SetPriority(USART6_IRQn, 9);
	NVIC_EnableIRQ(USART6_IRQn);
	
}

//globale berechnung der checksum
unsigned char LIN_checksum(unsigned char id, unsigned char *data, int len) {
    unsigned int sum = id;

    for (int i = 0; i < len; i++) {
        sum += data[i];
        // Carry auf untere 8 Bit addieren
        if (sum > 0xFF)
            sum = (sum & 0xFF) + 1;
    }

    return ~((unsigned char) sum);
}



//LIN-Bus Handler
void USART6_IRQHandler (void){
	typedef enum{TEMP, FREQ, KEYB} Abfrage;
	static Abfrage abfrage;
	typedef enum {IDLE,SYNCBREAK, SYNCFIELD, IDENT, RESPONSE} LINHEAD_type;
	static LINHEAD_type linstate = IDLE;
	static unsigned char linres[10];
	short status = USART6->SR;
	short DR = USART6->DR;
	USART6->SR = 0;
	static int index = 0;
	
	switch (linstate)
	{
		case IDLE:
			if (status & (1<<8)){
				linstate = SYNCBREAK;
			}
			break;
		case SYNCBREAK:
			if((status & 0x20)&&(DR == 0x55)){
				linstate=SYNCFIELD;
			}
			else{
				linstate = IDLE;
			}
			break;
		case SYNCFIELD:
			if((status & (1<<5))&& ((DR & 0x3F)== (0x10 | SLAVEID))){
				linstate=IDENT;
				abfrage=TEMP;
			}
			else if((status & 0x20) && ((DR & 0x3F)== (0x20 | SLAVEID))) {
				linstate = IDENT;
				abfrage = FREQ;
			}
			else if((status & 0x20) && ((DR & 0x3F)== (0x30 | SLAVEID))) {
				linstate = IDENT;
				abfrage = KEYB;
			}
			else{
					linstate=IDLE;
					break;
			}
			case IDENT:
			if(abfrage == TEMP){
				index = 2;
				linres[2] = (temperatur & 0xFF);     // LSB
				linres[1] = (temperatur >> 8);       // MSB
				unsigned char id = 0x10 | SLAVEID;
				linres[0] = LIN_checksum(id, &linres[1], 2);
				USART6->DR = linres[index];
				linstate = RESPONSE;
			}
			else if (abfrage == FREQ){
				index = 4;

				uint32_t freq = 0;

				if (deltaT > 0) {
					// deltaT ist in µs, daher Frequenz in Hz = 1.000.000 / deltaT
					freq = 1000000 / deltaT;
				}
				else {
					freq = 0; // Kein gültiger Messwert
				}

				// Frequenz auf 4 Bytes aufteilen (Little Endian, wie vom Master erwartet)
				linres[4] = (freq >> 24) & 0xFF;   // MSB
				linres[3] = (freq >> 16) & 0xFF;
				linres[2] = (freq >> 8)  & 0xFF;
				linres[1] = freq & 0xFF;          // LSB

				// Checksumme
				unsigned char id = 0x20 | SLAVEID;
				linres[0] = LIN_checksum(id, &linres[1], 4);

				USART6->DR = linres[index];
				linstate = RESPONSE;
			}
			else if(abfrage == KEYB){	
				index=2;
				linres[2] = (key >> 8) & 0xFF;
				linres[1] = key & 0xFF;
				unsigned char id = 0x30 | SLAVEID;
				linres[0] = LIN_checksum(id, &linres[1], 2);
				USART6->DR = linres[index];				
				linstate=RESPONSE;
			}
			break;
		case RESPONSE:
			if((status&0x40) && (index>0)){
				USART6->DR = linres[--index];
			}
			else if ((status&0x40) && (index ==0)){
				linstate=IDLE;
			}
			break;
	}
}

//Portkonfiguratuion Potentiometer 
void ADC_Init(){
	
	GPIOC->MODER |= 0xF;
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	ADC1 ->CR2 |= 0x1;
	ADC1 ->SQR3 |= (1<<10);
	ADC1 -> SQR3 &= ~0x5;
	ADC1 ->SQR3 |= (1<<11);
	ADC1 -> SQR3 &= ~0x4;
	ADC1 ->SQR1 &= 0xFF0FFFFF;
	ADC1 -> SQR1 |= (1<<20);
	
	ADC1 -> CR2 |= 0x40000002;
	ADC1 ->CR1 |= 0x01000000;
	ADC1 ->CR1 &= ~0xFDFFFFFF;
	ADC1 -> SMPR1 |= 0x1;
}


//Potentiometer Getter-Methode 
void ADC_get_value(void){
	ADC1 ->SQR3 |= 0xA;
	ADC1 -> SQR3 &= ~0x5;
	ADC1 ->CR2 |= 0x40000000;
	wait_us(100);
	while(!(ADC1->SR & 0x2)){
		
	}
	poti_1 = ADC1 ->DR;
	poti_1 = poti_1/40;
	
	ADC1->SQR3 |= 0xB;
	ADC1->SQR3 &= ~0x4;
	ADC1->CR2 |= 0x40000000;
	wait_us(100);
	while(!(ADC1->SR & 0x2)){
		
	}
	poti_2 = ADC1 ->DR;
	poti_2 = poti_2/40;
}


/* ================================================================================= */
/* main Funktion 																																		 */
/* ================================================================================= */
int main (void) {
	mcpr_SetSystemCoreClock();	// has to be first line in main! 
	
// Blinky Projekt
  int  zz05s = 0;                       // Zeitzähler 0.5s
  char KeyUser=0, KeyUserLa=0;          // Prozessabbilder User-Taste aktuell und vorher
  char Ledgruen=0;
  unsigned short leds=0;
	
	// Initialisieren der Portpins für grüne LED und blaue User-Taste auf dem Discovery-board 
  RCC->AHB1ENR |= (1 <<3) | (1 <<0);    // Clock für Port D und A einschalten
  GPIOD->MODER |= (0b01 << 2*12);       // Pin PD12 als Ausgang
//------------------
	
// LCD Projekt
	//Inittialisierung LCD_Init
	LCD_Init(); 
	LCD_ClearDisplay (0x8FE0);
	
	// Für Ansteigende Zahlen im Display
	int alt = 0;
	char *altstr;
//------------------

// Tastatur Matrix
	uint16_t keyStaAC=0, keyEvPr = 0, keyEvRe = 0;
	char taststr[17];
	char tastpos[17];
	char tastneg[17];
	//Initialisierung MatrixTastatur
	TastMatInit();
	
	// Timer 7
	InitTIM7(); //Initialisierung Timer7
	int sflagcnt = 0;



// Temperatur mit Timer12 muss TIM8 sein!
	InitTIM8(); //Initialisierung Timer12
	uint16_t R_KTY = 0;
	char stemp[17];
	
//LIN-Bus
LIN_Init(); //LIN-Bus initialisiern

//ADC + Poti 
ADC_Init(); 
char Pot1[10];
char Pot2[2];



	// Polling-Schleife für immer
	while (1) {		
    
// Blinky Projekt

    // Eingänge lesen
    KeyUserLa = KeyUser;
    if (GPIOA->IDR & (1 <<0) ) {        // Testen ob User-Taste gedrückt
      KeyUser = 1;     
    } else {
      KeyUser = 0;
    }
   
    // Funktion
    if (KeyUser && (!KeyUserLa)) {      // Flanke, gerade gedrückt, LED PD12 ein, Blinken starten
      Ledgruen = 1;
      leds     = 0x0001;
      zz05s   = Z05s;
    }
    if (KeyUser) {                      // Blinken, Lauflicht, solange User-Taste gedrückt
      if (!zz05s) {
        zz05s = Z05s;
        Ledgruen = !Ledgruen;           // Blinken
        if (leds & 0x8000) {            // 16 LEDs, Lauflicht volllaufen - leerlaufen nach oben
          leds = (leds << 1);
        } else {
          leds = (leds << 1) | 1;
        }
      }
    } 
		else {                            // alles aus ohne Taste
      Ledgruen = 0;
      //leds     = 0; //Auskommentiert da 10s nachlauf, durch Timer 7
      zz05s    = 0;
    }
		
    // Ausgänge schreiben
    if (Ledgruen) GPIOD->ODR |= (1 <<12); else GPIOD->ODR &= ~(1 <<12);
    LEDS = leds;

    
    // Zeitverhalten Polling
    wait_us(ZPOLLDELAY);               	// Verzögerungszeit der Pollingschleife
    if (zz05s) zz05s--;                 // Zeitzähler, für 0.5s

//------------------
		
//Display
		//LCD_WriteString("Bitfolge der Tastatur",0xF800, 0x8FE0,0,100);
		//sprintf(altstr,"%i",alt);
		//LCD_WriteString (altstr, 0xF800, 0x8FE0, 100, 120);
		//alt++;
//------------------
		
// Tastatur Matrix
		uint16_t keyStatlast = keyStaAC; 		//Speicher vom vorherigen Durchlauf
		keyStaAC = ReadKeys();							//Lesen der Keys
		keyEvPr = keyStaAC & (~keyStatlast);	//Positive Flanke
		keyEvRe = (~keyStaAC) & keyStatlast;	//Negative Flanke
		
		LCD_WriteString("Bitfolge der Tastatur",0xF800, 0x8FE0,40,0);
		Short2BitString(taststr, keyStaAC);
		LCD_WriteString (taststr, 0xF800, 0x8FE0, 40, 20);
		
		LCD_WriteString("Pos. Flanke",0xF800, 0x8FE0,40,40);
		Short2BitString(tastpos, keyEvPr);
		LCD_WriteString (tastpos, 0xF800, 0x8FE0, 40, 60);
		
		LCD_WriteString("Neg. Flanke",0xF800, 0x8FE0,40,80);
		Short2BitString(tastneg, keyEvRe);
		LCD_WriteString (tastneg, 0xF800, 0x8FE0, 40, 100);
		
//------------------
	

// Timer 7
		
		if (KeyUser && (!KeyUserLa)) {      // Flanke, gerade gedrückt, LED PD12 ein, Blinken starten
			GPIOD->ODR &= ~(1 << 13); //Backlight PD13 off
    }
		
		if (KeyUser) {                      // Blinken, Lauflicht, solange User-Taste gedrückt
      // Blinken des Displays
			if (sflag) {
        GPIOD->ODR |= (1 << 13); //0x2000 // Backlight PD13 on 
      }
			else{
				GPIOD->ODR &= ~(1 << 13); //Backlight PD13 off
			}
    } 
		else {                   // Grundzustand Backlight on
			GPIOD->ODR |= (1 << 13); // Backlight PD13 on
    }
    
		if (!KeyUser && (KeyUserLa)) {  // negative Flanke von KeyUser
			sflagcnt = 1; //Marker für Lauflicht für 10s nach loslassen
		}
			
    if (sflagcnt > 0 && sflag2 == 1){ // sflag counter wenn aktiviviert mit 1
			sflag2 = 0;
			if (leds & 0x8000) {            // 16 LEDs, Lauflicht volllaufen - leerlaufen nach oben
          leds = (leds << 1);
        } else {
          leds = (leds << 1) | 1;
        }
			
			sflagcnt++;
		}
		
		if (sflagcnt >= 7){ //Reset auf Ruhezustand
			sflagcnt = 0; 
			leds = 0;
			
		}
		
		if(strcmp(taststr, "0000000000000000") != 0){
			sflagcnt = 0;
			leds = 0;
			leds = keyStaAC;
			wait_us(5000);
		}
		
		if(strcmp(taststr, "0000000000000000") == 0 && sflagcnt == 0 && !KeyUser){
					leds = 0;
				}

			
		
// Temperaturmessung
		
		if (tempflag == 0 && deltaT != 0){
			R_KTY = (deltaT*1000)/1143; 
			temperatur = (((R_KTY - 1000)*10)/96) + 25;
			
			//Short2BitString(stemp, R_KTY);
			sprintf(altstr,"%i",temperatur);
			LCD_WriteString ("Temperatur:", 0xF800, 0x8FE0, 40, 120);
			LCD_WriteString (altstr, 0xF800, 0x8FE0, 40, 140);
			LCD_WriteString ("`C", 0xF800, 0x8FE0, 65, 140);
			
		}
		if (tempflag == 1) {
			LCD_WriteString ("NS", 0xF800, 0x8FE0, 40, 140);
		}
		
		GPIOB->ODR |= 0x8000; //ODR15 an
		wait_us(200);
		GPIOB->ODR &= ~(0x8000); //ODR15 aus


//LIN-Bus
		key=ReadKeys();
		
//Potentiometer
		ADC_get_value();
		sprintf(Pot1, "Poti1: %4u Prozent", poti_1); 
		LCD_WriteString(Pot1, 0xF800, 0x8FE0, 40, 160);
		sprintf(Pot2, "Poti2: %4u Prozent", poti_2);
		LCD_WriteString(Pot2, 0xF800, 0x8FE0, 40, 180); 	
		
	} // Polling-Schleife Ende


  
} //main Ende



// Timer 7 Interrupt Request Handler            																			 

/* --------------------------------------------------------------------------------- */
/* Verzögerungszeit von ca. 1µs (etwas unterschiedlich Flash/RAM-Target !)					 */
/* --------------------------------------------------------------------------------- */
void wait_us (int us) {
  for (int i=0; i<18*us; i++){}
}



void TIM7_IRQHandler (void) {
	TIM7->SR = 0x0000; //Servic-Request (Status Register) wird zurückgesetzt
	ms++;
	
	if (ms>=1000 && sflag == 1) { //Ausschalten
		ms = 0;
		sflag = 0;
	}
	
	if (ms>=1000){ //Einschalten
		ms = 0;
		sflag = 1;
		sflag2 = 1;
	}

}

void TIM8_CC_IRQHandler (void) {
	uint16_t static full = 0;
	
	status = TIM8->SR;
	TIM8->SR = 0x0000;
	
	if (status & 0x0008){ //CC1 Interrupt, C aufgeladen CH3
		capture1 = TIM8->CCR3; //mit TIM12->CCR1 lesen CC1IF clearen CCR1 = Stand des Zählers bei Interrupt
		full = 1; //C ist voll
		
	}
	
	if (status & 0x0010){ //CC2 Interrupt, C entladen CH4
		capture2 = TIM8->CCR4; //mit TIM12->CCR2 lesen CC2IF clearen
		
		// Wenn zuvor aufgeladen, deltaT bestimmen
		if (full == 1){
			deltaT = capture2 - capture1; //CNT von CCR2 -CCR2 = Tickdifferenz in us
			full = 0;
		}
	}
	
	if (status & 0x0001){ //Overflow Interrupt wenn R nicht angeschlossen
		tempflag = 1;
	}
	else{
		tempflag = 0;
	}
	
}
