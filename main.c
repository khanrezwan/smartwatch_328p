/*
 * main.c
 *
 *  Created on: Apr 14, 2017
 *      Author: rezwan
 */
#include "u8g.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <compat/twi.h>
#include "i2c.h"


//////////////ADXL345/////////////////////
#define ADXL345_I2C_Address    0x53        // I2C ADXL345 Device 7bit
#define ADXL345_Dev_ID 0b11100101
#define ADXL345_TO_READ (6) //read 6 bytes from x_low
char buffer[6];

#define BAUD 57600 /**< Baud Rate in bps. refer page 179 of 328p datasheet. */
#define MYUBRR F_CPU/16/BAUD-1 /**< UBRR = (F_CPU/(16*Baud))-1 for asynch USART page 179 328p datasheet. Baud rate 9600bps, assuming	16MHz clock UBRR0 becomes 0x0067*/
////////////////USART Start///////////////////////////////////////
void USART_init(unsigned int ubrr) {

	UCSR0C = (0 << USBS0) | (3 << UCSZ00); /// Step 1. Set UCSR0C in Asynchronous mode, no parity, 1 stop bit, 8 data bits
	UCSR0A = 0b00000000; /// Step 2. Set UCSR0A in Normal speed, disable multi-proc

	UBRR0H = (unsigned char) (ubrr >> 8); /// Step 3. Load ubrr into UBRR0H and UBRR0L
	UBRR0L = (unsigned char) ubrr;

	UCSR0B = 0b00011000; /// Step 4. Enable Tx Rx and disable interrupt in UCSR0B
}
int USART_send(char c, FILE *stream) {

	while (!( UCSR0A & (1 << UDRE0))) /// Step 1.  Wait until UDRE0 flag is high. Busy Waitinig
	{
		;
	}

	UDR0 = c; /// Step 2. Write char to UDR0 for transmission
}
int USART_receive( FILE *stream) {

	while (!(UCSR0A & (1 << RXC0)))
		/// Step 1. Wait for Receive Complete Flag is high. Busy waiting
		;

	return (UDR0); /// Step 2. Get and return received data from buffer
}
//////////////USART END


///////////OLED 1306 128 x 64 software SPI//////////////////////
/// PORTA = 0, PORTB = 1, PORTC = 2, PORTD = 3
#define OLED_CS PN(2,3)  // PC3
#define OLED_SCK PN(2,2) //PC2
#define OLED_MOSI PN(2,1) //PC1
#define OLED_A0 PN(2,0)  // PC0 D/C
#define OLED_RESET PN(3,4) // PN(3,4) //PD4  or U8G_PIN_NONE

u8g_t u8g;
int yPos = 0;
void draw(void) {

	u8g_SetFont(&u8g, u8g_font_unifont);
	u8g_DrawStr(&u8g, 0, yPos, "Hello World!");
}

void OLED_init() {
	u8g_InitSPI(&u8g, &u8g_dev_ssd1306_128x64_2x_sw_spi, OLED_SCK, OLED_MOSI,
	OLED_CS, OLED_A0, OLED_RESET);
	u8g_SetContrast(&u8g, 55);
}


////////////ADXL345 Functions start//////////////////////////////
void init_ADXL345()
{
	i2c_start(ADXL345_I2C_Address,TW_WRITE);
	i2c_write(0x02D);
	i2c_write(0);
	i2c_stop();

	i2c_start(ADXL345_I2C_Address,TW_WRITE);
	i2c_write(0x02D);
	i2c_write(16);
	i2c_stop();

	i2c_start(ADXL345_I2C_Address,TW_WRITE);
	i2c_write(0x02D);
	i2c_write(8);
	i2c_stop();

	i2c_start(ADXL345_I2C_Address,TW_WRITE);
	i2c_write(0x31);
	i2c_write(0x08);//full range +/-2g
	i2c_stop();

}

char check_ADXL345()
{
	char data;
	i2c_start(ADXL345_I2C_Address,TW_WRITE);
	i2c_write(0x00);
	i2c_stop();
	i2c_start(ADXL345_I2C_Address,TW_READ);
	i2c_read(&data,NACK);
	return data;

}

void read_ADXL345()
{
	i2c_start(ADXL345_I2C_Address,TW_WRITE);
	i2c_write(0x32);
	i2c_stop();
	i2c_start(ADXL345_I2C_Address,TW_READ);
	i2c_read(&buffer[0],ACK);
	i2c_read(&buffer[1],ACK);
	i2c_read(&buffer[2],ACK);
	i2c_read(&buffer[3],ACK);
	i2c_read(&buffer[4],ACK);
	i2c_read(&buffer[5],NACK);
}
////////////ADXL345 Functions end//////////////////////////////
int main(void) {
	int dataX,dataY,dataZ;
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
	USART_init(MYUBRR);
	// Initialise the standard IO handlers
	stdout = fdevopen(USART_send, NULL);
	stdin = fdevopen(NULL, USART_receive);
	if(check_ADXL345()==ADXL345_Dev_ID)
		{
	//		printf("ready\r\n");
			init_ADXL345();
			printf("initialized %d\r\n");
		}
		else
		{
	//		printf("error\r\n");
			while(1)
			{

			}
		}


	for (;;) {
		u8g_FirstPage(&u8g);
		do {
			draw();
		} while (u8g_NextPage(&u8g));
		if (yPos < 83) {
			// if it's too slow, you could increment y by a greater number
			yPos++;
		} else {
			// When the yPos is off the screen, reset to 0.
			yPos = 0;
		}

		read_ADXL345();
				dataX= buffer[1]<<8|buffer[0];
				dataY= buffer[3]<<8|buffer[2];
				dataZ= buffer[5]<<8|buffer[4];
				printf("%d %d %d\r\n",dataX,dataY,dataZ);
//		GY521_read();
//		sprintf(buffer,"%f",&roll);
//        u8g_Delay(100);
	}
}

