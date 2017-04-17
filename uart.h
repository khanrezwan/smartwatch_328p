#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "circular_buffer.h"
#ifndef UART_H_
#define UART_H_

//circBuf_t RX_Q;


#define BAUD 57600 /**< Baud Rate in bps. refer page 179 of 328p datasheet. */
#define MYUBRR F_CPU/16/BAUD-1 /**< UBRR = (F_CPU/(16*Baud))-1 for asynch USART page 179 328p datasheet. Baud rate 9600bps, assuming	16MHz clock UBRR0 becomes 0x0067*/

#define BufferMax 128


//#define UART_STATUS_New_Line 0
#define UART_STATUS_Carriage_Retrun 0
#define UART_STATUS_Command_Complete 1
#define UART_STATUS_OVERRUN 2



void USART_init(unsigned int ubrr, circBuf_t *rx_Q) ;
int USART_send(char c, FILE *stream) ;
void clear_USART_STAUS_REG(uint8_t bit);
void status_USART_STAUS_REG(uint8_t bit);
uint8_t check_USART_Complete(void);
//int USART_receive( FILE *stream) ;
#endif
