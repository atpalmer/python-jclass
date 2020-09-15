#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

typedef struct {
    uint16_t attribute_name_index;
    uint32_t attribute_length;
    uint8_t info[];
} JavaClassAttribute;

typedef struct {
    uint16_t attributes_count;
    JavaClassAttribute *attributes[];
} JavaClassAttributes;

size_t attributes_parse(uint8_t *data, JavaClassAttributes **obj);
void attributes_print(JavaClassAttributes *this);
void JavaClassAttributes_free(JavaClassAttributes *this);

#endif
