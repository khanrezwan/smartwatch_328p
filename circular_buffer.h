/*
 * circular_buffer.h
 *
 *  Created on: Apr 17, 2017
 *      Author: rezwan
 */

#ifndef CIRCULAR_BUFFER_H_
#define CIRCULAR_BUFFER_H_
#include <stdint.h>



typedef struct {
	uint8_t* buffer;
	uint8_t head;
	uint8_t tail;
	uint8_t maxLen;
} circBuf_t;

extern uint8_t circBufPush(circBuf_t *c, uint8_t data);
extern uint8_t circBufPop(circBuf_t *c, uint8_t *data);

#endif /* CIRCULAR_BUFFER_H_ */
