#ifndef JCLASS_STREAM_H
#define JCLASS_STREAM_H

#include <stdio.h>
#include <stdint.h>

uint32_t stream_next_uint32(FILE *this);
uint16_t stream_next_uint16(FILE *this);
uint8_t stream_next_uint8(FILE *this);
uint8_t stream_peek_uint8(FILE *this);

#endif
