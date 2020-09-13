#ifndef INTERFACES_H
#define INTERFACES_H

typedef struct {
    uint16_t interfaces_count;
    uint16_t interfaces[];
} JavaClassInterfaces;


size_t interfaces_parse(uint8_t *data, JavaClassInterfaces **obj);
void interfaces_print(JavaClassInterfaces *this);
void JavaClassInterfaces_free(JavaClassInterfaces *this);

#endif
