#include <arpa/inet.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jclass/internal/membuff.h>

uint32_t file_next_uint32(FILE *f) {
    uint32_t data;
    fread(&data, 1, 4, f);
    return ntohl(data);
}

uint16_t file_next_uint16(FILE *f) {
    uint16_t data;
    fread(&data, 1, 2, f);
    return ntohs(data);
}

uint8_t file_next_uint8(FILE *f) {
    uint8_t data;
    fread(&data, 1, 1, f);
    return data;
}

uint8_t file_peek_uint8(FILE *f) {
    uint8_t data;
    fread(&data, 1, 1, f);
    fseek(f, -1, SEEK_CUR);
    return data;
}

int membuff_has_error(struct membuff *this) {
    return ferror(this->fh);
}

uint32_t membuff_next_uint32(struct membuff *this) {
    return file_next_uint32(this->fh);
}

uint16_t membuff_next_uint16(struct membuff *this) {
    return file_next_uint16(this->fh);
}

uint8_t membuff_next_uint8(struct membuff *this) {
    return file_next_uint8(this->fh);
}

uint8_t membuff_peek_uint8(struct membuff *this) {
    return file_peek_uint8(this->fh);
}

void membuff_copy_next(struct membuff *this, size_t size, void *target) {
    fread(target, 1, size, this->fh);
}

int membuff_from_filename(const char *filename, struct membuff **r) {
    *r = NULL;
    FILE *fh = fopen(filename, "rb");
    if(!fh)
        return errno;
    *r = malloc(sizeof(struct membuff));
    (*r)->fh = fh;
    return 0;
}

void membuff_free(struct membuff *this) {
    if(!this)
        return;
    if(this->fh)
        fclose(this->fh);
    free(this);
}
