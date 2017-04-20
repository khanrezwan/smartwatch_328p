/*
 * main.c
 *
 *  Created on: Apr 14, 2017
 *      Author: rezwan
 */
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "u8g.h"

#include "adxl345.h"
#include "i2c.h"
#include "bitmap.h"
#include "uart.h"
#include "circular_buffer.h"
#include "ds1307.h"
circBuf_t RX_Q;
Time_t time;
///////////////PIN Mapping///////
//OLED
#define OLED_CS PN(2,3)  // PC3 == arduino A3
#define OLED_SCK PN(2,2) //PC2 == arduino A2
#define OLED_MOSI PN(2,1) //PC1 == arduino A1
#define OLED_A0 PN(2,0)  // PC0 --> D/C of OLED === arduino A0
#define OLED_RESET PN(3,4) // PN(3,4) //PD4  or U8G_PIN_NONE == == arduino 4
//I2C
#define SDA PC4 //== arduino A4
#define SCL PC5 //== arduino A5
//Bluetooth
#define BT_RX PD0 // == arduino 0
#define BT_TX PD1 // == arduino 1

// ADXL Interrupt pin using  ADXL INT1 --> 328p INT0
#define ADXL345_INT1 PD2 // == arduino 2
#define ADXL345_INT1_PIN_REG PIND
#define ADXL345_INT1_DDR_REG DDRD

// Rotary encoder channel A --> 328p INT1
#define Encoder_Channel_A PD3 // == arduino 3
#define Encoder_Channel_B PD5 // == arduino 5
#define Encoder_PORT_REG PORTD
#define Encoder_DDR_REG DDRD

//Vibrating Motor to OC0A PWM
#define Vibrating_motor_Max_Duty_Cycle 127
#define Vibrating_motor_Out PD6 // == arduino 6
#define Vibrating_motor_OC_Reg  OCR0A
#define Vibrating_motor_DDR_REG DDRD

//Button(s)
#define Button_OK PB0 // == arduino 12
#define Button_Cancel PB4 // == arduino 8
#define Button_PIN_REG PINB
#define Button_DDR_REG DDRB


///////////OLED 1306 128 x 64 software SPI//////////////////////
/// PORTA = 0, PORTB = 1, PORTC = 2, PORTD = 3

u8g_t u8g;
int yPos = 0;
uint8_t Index = 0;
void draw(void) {

	//u8g_SetFont(&u8g, u8g_font_unifont);
	u8g_DrawBitmapP(&u8g, 10, 15, 2, 16, ICON_BITMAP_2);
}

void OLED_init() {
	u8g_InitSPI(&u8g, &u8g_dev_ssd1306_128x64_2x_sw_spi, OLED_SCK, OLED_MOSI,
	OLED_CS, OLED_A0, OLED_RESET);
	u8g_SetContrast(&u8g, 55);
}


int main(void) {
	int dataX, dataY, dataZ;
	/*
	 CS: PORTC, Bit 3 --> PN(2,1)
	 A0: PORTC, Bit 0 --> PN(2,0)
	 SCK: PORTC, Bit 2--> PN(2,0)
	 MOSI: PORTC, Bit 1 --> PN(2,1)
	 REST: PORTD, Bit4 --> PN(3,4)
	 */
	//u8g_InitSPI(&u8g, &u8g_dev_ssd1306_128x64_hw_spi, PN(1, 5)/*PB5 SCK*/, PN(1, 3)/*PB5 MOSI*/, PN(1, 2)/*PB5 SCK*/, PN(1, 1)/*PB1 CS*/, U8G_PIN_NONE);
	OLED_init();
	init_I2C();

	DDRB |= (1 << PB0);
	USART_init(MYUBRR, &RX_Q);
	// Initialise the standard IO handlers
	stdout = fdevopen(USART_send, NULL);
	if (check_ADXL345() == ADXL345_Dev_ID) {
		//		printf("ready\r\n");
		init_ADXL345();
		printf("initialized ADXL\r\n");
	} else {
		//		printf("error\r\n");
		while (1) {

		}
	}
	if(DS1307_check()){
		printf("initialized DS1307\r\n");
		time.second = 0;
		time.date = 3;
		time.hour = 3;
		time.month = 4;
		time.year = 17;
		time.weekday = 6;
		if(!DS1307_Write(&time)){
			printf("DS1307 write error");
		}
	}
	else{
		while (1) {

				}
	}
	sei();
	for (;;) {
		u8g_FirstPage(&u8g);
		do {
			draw();
		} while (u8g_NextPage(&u8g));
		if (Index < ICON_ARRAY_SIZE) {
			Index++;

		} else {
			Index = 0;
		}
		if (yPos < 83) {
			// if it's too slow, you could increment y by a greater number
			yPos++;
		} else {
			// When the yPos is off the screen, reset to 0.
			yPos = 0;
		}

		read_ADXL345(&dataX,&dataY, &dataZ);

		//printf("%s",RX_Q.buffer);
		printf("%d %d %d\r\n", dataX, dataY, dataZ);
		PORTB &= ~(1 << PB0);
		if (check_USART_Complete()) {

			clear_USART_STAUS_REG(UART_STATUS_Command_Complete);
			printf("%d", check_USART_Complete());

		}
		u8g_Delay(100);
		if(DS1307_Read(&time)){
			printf("Second = %d",time.second);
		}
		else{
			printf("DS1307 read error");

		}
	}
}

