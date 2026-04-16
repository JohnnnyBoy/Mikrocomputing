
// Initialisation for SSD2119
// 320 RGB x 240 TFT LCD Driver
// Draft, 03.05.2020
//
// LCD_WriteReg has to be completed!!!
#include "STM32F4xx.h"
#include "_mcpr_stm32f407.h"
#include "fonts.h"
#include <inttypes.h>


#define addr_cmd *(unsigned short volatile*) 0x60000000 //Adresse für LCD-Kommandozugriff
#define addr_data *(unsigned short volatile*) 0x60100000 //Adresse für LCD-Datenzugriff

void LCD_WriteData(unsigned short data){
	addr_data = data;
}

void LCD_WriteCommand(unsigned short cmd){
	addr_cmd = cmd;
}

void LCD_WriteReg (unsigned short command, unsigned short data){
  LCD_WriteCommand(command); // Hardware access to command register of LCD, e.g.: LCD_WriteCommand(command);
  LCD_WriteData(data); // Hardware access to data    register of LCD, e.g.: LCD_WriteData(data);
}

void LCD_SetCursor (uint32_t x, uint32_t y){
	LCD_WriteReg (0x004E,x); // xPos wird beschrieben
	LCD_WriteReg (0x004F,y); // yPos wird beschrieben
}

void LCD_DrawPixel (uint16_t color){
	LCD_WriteReg (0x0022,color);
}

void LCD_ClearDisplay (uint16_t color){
	for (int x=0;x<320; x++){ //Spalte
		for (int y=0;y<=240;y++){ //Zeile
			LCD_SetCursor(x,y);
			LCD_DrawPixel(color);
		}
	}
}

void LCD_WriteLetter(char zeichen, uint16_t fgcol, uint16_t bgcol, uint32_t xpos, uint32_t ypos){
	signed short i, j;
	unsigned short offset, zeile;
	
	offset = (short) zeichen*32; //Index des ersten Bytes des ASCII-Zeichen im Font
	
	for (i=0;i<16;i++){
		zeile = (console_font_12x16[offset+2*i]<<8) | console_font_12x16[offset+2*i+1];
		LCD_SetCursor(xpos,ypos+i);
		for (j=15; j>3; j--){
			if (zeile & (1<<j)){
				LCD_DrawPixel(fgcol);
			}
			else{
				LCD_DrawPixel(bgcol);
			}
		}
	}
}

void LCD_WriteString (char string[], uint16_t fgcol, uint16_t bgcol, uint32_t xpos, uint32_t ypos){
	
	uint32_t startxpos = xpos;
	
	int i = 0;
	while (string[i] != '\0'){
		LCD_WriteLetter(string[i], fgcol, bgcol, startxpos+i*12, ypos);
		i++;
	}
	
}


void LCD_Init (void){
  unsigned int wait = 0;
	
//  RCC->AHB1ENR |= 0x0000001B;	  // Takt (clock) fuer Port E D B A   0000000000011011  Debugger "RCC" (Seite 178)
  RCC->AHB1ENR |= 0x00000008;	    // Takt (clock) fuer Port D   0000000000001000  Debugger "RCC" (Seite 178)
  GPIOD->ODR   |= 0x0008; 		    // PD3 (reset) auf 1 ???

	GPIOD->MODER |= 0x04000040;	    // OUT 01: PD 3 Reset, PD13 Backlight
//	GPIOB->MODER |= 0x00050000;	  // OUT: PB 8,9 I2C Touch ???
  GPIOD->ODR   |= 0x2000;         // Backlight PD13 on  <================== hier die normale Stelle ========
	
//  Display aufwecken mit Low-Impuls (15us) an Reset (PD3) danach Wartezeit (einige) ms
//  Funktioniert auch ohne den folgenden Block!
//  ****************************************
	GPIOD->ODR |= 0x0008;      // PD3 ein
	for (wait=0; wait < 2000; wait++){}   // ist immer noch recht lange mit echten ca. 40µs
	GPIOD->ODR &= ~0x0008;      // PD3 aus
	for (wait=0; wait < 2000; wait++){}   
	GPIOD->ODR |= 0x0008;      // PD3 ein	
//  ****************************************
	for (wait=0; wait < 2000000; wait++){}  // > 10ms
	LCD_WriteReg(0x0010, 0x0001); /* Enter sleep mode */
  LCD_WriteReg(0x001E, 0x00B2); /* Set initial power parameters. */
  LCD_WriteReg(0x0028, 0x0006); /* Set initial power parameters. */
  LCD_WriteReg(0x0000, 0x0001); /* Start the oscillator.*/
	// LCD_WriteReg(0x0001, 0x30EF); /* Set pixel format and basic display orientation */
	LCD_WriteReg(0x0001, 0x72EF); /* Set pixel format and basic display orientation */	
  LCD_WriteReg(0x0002, 0x0600);
  LCD_WriteReg(0x0010, 0x0000); /* Exit sleep mode.*/
	for (wait=0; wait < 4000000; wait++){}  // > 20ms	eigentlich 30ms, weniger geht meist auch		
	// LCD_WriteReg(0x0011, 0x6870); /* Configure pixel color format and MCU interface parameters.*/
  LCD_WriteReg(0x0011, 0x6870); /* Configure pixel color format and MCU interface parameters.*/	
  LCD_WriteReg(0x0012, 0x0999); /* Set analog parameters */
  LCD_WriteReg(0x0026, 0x3800);
  LCD_WriteReg(0x0007, 0x0033); /* Enable the display */
  LCD_WriteReg(0x000C, 0x0005); /* Set VCIX2 voltage to 6.1V.*/
  LCD_WriteReg(0x000D, 0x000A); /* Configure Vlcd63 and VCOMl */
  LCD_WriteReg(0x000E, 0x2E00);
  LCD_WriteReg(0x0044, (240-1) << 8); /* Set the display size and ensure that the GRAM window
                                          is set to allow access to the full display buffer.*/
  LCD_WriteReg(0x0045, 0x0000);
  LCD_WriteReg(0x0046, 320-1);
// GPIOD->ODR |= 0x2000;         // Backlight PD13 on   <================== Test für Logikanalysator 5MS/sec 200ns
  LCD_WriteReg(0x004E, 0x0000); /*Set cursor to 0,0 */
  LCD_WriteReg(0x004F, 0x0000);

}

