/*
 * circular_buffer.c
 *
 *  Created on: Apr 17, 2017
 *      Author: rezwan
 */

#include "circular_buffer.h"

uint8_t circBufPush(circBuf_t *c, uint8_t data)
{
    // next is where head will point to after this write.
    int next = c->head + 1;
    if (next >= c->maxLen)
        next = 0;

    if (next == c->tail) // check if circular buffer is full
        return 0;       // and return with an error.

    c->buffer[c->head] = data; // Load data and then move
    c->head = next;            // head to next data offset.
    return 1;  // return success to indicate successful push.
}

uint8_t circBufPop(circBuf_t *c, uint8_t *data)
{
    // if the head isn't ahead of the tail, we don't have any characters
    if (c->head == c->tail) // check if circular buffer is empty
        return 0;          // and return with an error

    // next is where tail will point to after this read.
    int next = c->tail + 1;
    if(next >= c->maxLen)
        next = 0;

    *data = c->buffer[c->tail]; // Read data and then move
    c->tail = next;             // tail to next data offset.
    return 1;  // return success to indicate successful push.
}
