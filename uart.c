#include "uart.h"
//#include "circular_buffer.h"

circBuf_t *RX_Q;
uint8_t Rx_Data[BufferMax];
volatile uint8_t UART_STATUS_REG = 0x00;

////////////////USART Start///////////////////////////////////////
void USART_init(unsigned int ubrr, circBuf_t *rx_Q) {

	RX_Q = rx_Q;
	RX_Q->buffer = Rx_Data;
	RX_Q->head = 0;
	RX_Q->tail = 0;
	RX_Q->maxLen = BufferMax;
	UCSR0C = (0 << USBS0) | (3 << UCSZ00); /// Step 1. Set UCSR0C in Asynchronous mode, no parity, 1 stop bit, 8 data bits
	UCSR0A = 0b00000000; /// Step 2. Set UCSR0A in Normal speed, disable multi-proc

	UBRR0H = (unsigned char) (ubrr >> 8); /// Step 3. Load ubrr into UBRR0H and UBRR0L
	UBRR0L = (unsigned char) ubrr;

	UCSR0B = 0b10011000; /// Step 4. Enable Tx Rx and enable RXC interrupt in UCSR0B
}
int USART_send(char c, FILE *stream) {

	while (!( UCSR0A & (1 << UDRE0))) /// Step 1.  Wait until UDRE0 flag is high. Busy Waitinig
	{
		;
	}

	UDR0 = c; /// Step 2. Write char to UDR0 for transmission
}
//int USART_receive( FILE *stream) {
//
//	while (!(UCSR0A & (1 << RXC0)))
//		/// Step 1. Wait for Receive Complete Flag is high. Busy waiting
//		;
//
//	return (UDR0); /// Step 2. Get and return received data from buffer
//}

ISR(USART_RX_vect) {
	uint8_t recieved_Byte;
	recieved_Byte = UDR0;
	switch (recieved_Byte) {
	case '0':
		if (bit_is_set(UART_STATUS_REG, UART_STATUS_Carriage_Retrun)) {
			if(!circBufPush(RX_Q, recieved_Byte)){
				UART_STATUS_REG |= (1 << UART_STATUS_OVERRUN);
			}
			UART_STATUS_REG |= (1 << UART_STATUS_Carriage_Retrun);
		}
		else{
			UART_STATUS_REG |= (1 << UART_STATUS_Carriage_Retrun);
		}
		break;
	case '1':
		if (bit_is_set(UART_STATUS_REG, UART_STATUS_Carriage_Retrun)) {
			UART_STATUS_REG |= (1 << UART_STATUS_Command_Complete);
			UART_STATUS_REG &= ~(1 << UART_STATUS_Carriage_Retrun);
			circBufPush(RX_Q,'\0');
		} else {

		}
		break;
	default:
		if(!circBufPush(RX_Q, recieved_Byte)){
						UART_STATUS_REG |= (1 << UART_STATUS_OVERRUN);
					}
		break;
	}
	if ((recieved_Byte == '\r')) {
		UART_STATUS_REG |= 1 << UART_STATUS_Carriage_Retrun;
	}
}
void clear_USART_STAUS_REG(uint8_t bit){
	UART_STATUS_REG &= ~(1 << bit);
};
void status_USART_STAUS_REG(uint8_t bit){
	UART_STATUS_REG |= (1 << bit);
};
uint8_t check_USART_Complete(void){
	return bit_is_set(UART_STATUS_REG,UART_STATUS_Command_Complete);
}
