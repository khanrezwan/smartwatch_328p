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
#include "bitmap.h"
#include "uart.h"
#include "circular_buffer.h"
circBuf_t RX_Q;
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

//////////////ADXL345/////////////////////
#define ADXL345_I2C_Address    0x53        // I2C ADXL345 Device 7bit
#define ADXL345_Dev_ID 0b11100101
#define ADXL345_TO_READ (6) //read 6 bytes from x_low
char buffer[6];

//////////////USART END

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

////////////ADXL345 Functions start//////////////////////////////
void init_ADXL345_double_tap(void) {
	i2c_start(ADXL345_I2C_Address, TW_WRITE);
	i2c_write(0x01D); // THRESH_TAP Register
	i2c_write(0x50); //5g
	i2c_stop();

	i2c_start(ADXL345_I2C_Address, TW_WRITE);
	i2c_write(0x022); //Latent register
	i2c_write(0x05);
	i2c_stop();

	i2c_start(ADXL345_I2C_Address, TW_WRITE);
	i2c_write(0x023); //window register
	i2c_write(0xFF);
	i2c_stop();

	i2c_start(ADXL345_I2C_Address, TW_WRITE);
	i2c_write(0x021); //duration register
	i2c_write(0x10);
	i2c_stop();

	i2c_start(ADXL345_I2C_Address, TW_WRITE);
	i2c_write(0x02A); //tap axis register
	i2c_write(0b00000111); // all axes
	i2c_stop();

	i2c_start(ADXL345_I2C_Address, TW_WRITE);
	i2c_write(0x02E); //Interrupt Enable
	i2c_write(0b00100000); // all axes
	i2c_stop();

}
void init_ADXL345() {
	init_ADXL345_double_tap();
	i2c_start(ADXL345_I2C_Address, TW_WRITE);
	i2c_write(0x02D);
	i2c_write(0);
	i2c_stop();

	i2c_start(ADXL345_I2C_Address, TW_WRITE);
	i2c_write(0x02D);
	i2c_write(16);
	i2c_stop();

	i2c_start(ADXL345_I2C_Address, TW_WRITE);
	i2c_write(0x02D);
	i2c_write(8);
	i2c_stop();

	i2c_start(ADXL345_I2C_Address, TW_WRITE);
	i2c_write(0x31);
	i2c_write(0x08); //full range +/-2g
	i2c_stop();

}

char check_ADXL345() {
	char data;
	i2c_start(ADXL345_I2C_Address, TW_WRITE);
	i2c_write(0x00);
	i2c_stop();
	i2c_start(ADXL345_I2C_Address, TW_READ);
	i2c_read(&data, NACK);
	return data;

}

void read_ADXL345() {
	i2c_start(ADXL345_I2C_Address, TW_WRITE);
	i2c_write(0x32);
	i2c_stop();
	i2c_start(ADXL345_I2C_Address, TW_READ);
	i2c_read(&buffer[0], ACK);
	i2c_read(&buffer[1], ACK);
	i2c_read(&buffer[2], ACK);
	i2c_read(&buffer[3], ACK);
	i2c_read(&buffer[4], ACK);
	i2c_read(&buffer[5], NACK);
}
void read_interrupt_source_ADXL345() {
	i2c_start(ADXL345_I2C_Address, TW_WRITE);
	i2c_write(0x30); //Source of interrupts.
	i2c_stop();
	i2c_start(ADXL345_I2C_Address, TW_READ);
	i2c_read(NULL, NACK);
}
////////////ADXL345 Functions end//////////////////////////////
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
		printf("initialized\r\n");
	} else {
		//		printf("error\r\n");
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

		read_ADXL345();
		dataX = buffer[1] << 8 | buffer[0];
		dataY = buffer[3] << 8 | buffer[2];
		dataZ = buffer[5] << 8 | buffer[4];
		//printf("%s",RX_Q.buffer);
		//printf("%d %d %d\r\n", dataX, dataY, dataZ);
		PORTB &= ~(1 << PB0);
		if (check_USART_Complete()) {

			clear_USART_STAUS_REG(UART_STATUS_Command_Complete);
			printf("%d", check_USART_Complete());
			//printf(RX_Q.buffer);
		}
//		GY521_read();
//		sprintf(buffer,"%f",&roll);
		u8g_Delay(100);
	}
}

