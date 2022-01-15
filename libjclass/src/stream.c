#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>
#include <jclass/internal/stream.h>

uint32_t stream_next_uint32(FILE *f) {
    uint32_t data;
    fread(&data, 1, 4, f);
    return ntohl(data);
}

uint16_t stream_next_uint16(FILE *f) {
    uint16_t data;
    fread(&data, 1, 2, f);
    return ntohs(data);
}

uint8_t stream_next_uint8(FILE *f) {
    uint8_t data;
    fread(&data, 1, 1, f);
    return data;
}

uint8_t stream_peek_uint8(FILE *f) {
    uint8_t data;
    fread(&data, 1, 1, f);
    fseek(f, -1, SEEK_CUR);
    return data;
}
