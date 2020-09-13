#ifndef CONSTANT_POOL_H
#define CONSTANT_POOL_H

enum constant_type {
    CONSTANT_TYPE_Utf8= 1,
    CONSTANT_TYPE_Class = 7,
    CONSTANT_TYPE_String = 8,
    CONSTANT_TYPE_Fieldref = 9,
    CONSTANT_TYPE_Methodref = 10,
    CONSTANT_TYPE_InterfaceMethodref = 11,
    CONSTANT_TYPE_NameAndType = 12,
};

typedef struct {
    uint8_t tag;
} JavaClassConstant;

typedef struct {
    uint8_t tag;
    uint16_t length;
    char bytes[];
} JavaClassUtf8Constant;

typedef struct {
    uint8_t tag;
    uint16_t name_index;
} JavaClassClassConstant;

typedef struct {
    uint8_t tag;
    uint16_t string_index;
} JavaClassStringConstant;

typedef struct {
    uint8_t tag;
    uint16_t class_index;
    uint16_t name_and_type_index;
} JavaClassFieldrefConstant;

typedef JavaClassFieldrefConstant JavaClassMethodrefConstant;
typedef JavaClassFieldrefConstant JavaClassInterfaceMethodrefConstant;

typedef struct {
    uint8_t tag;
    uint16_t name_index;
    uint16_t descriptor_index;
} JavaClassNameAndTypeConstant;

void constant_pool_print(JavaClassConstant **constants, int count);
size_t constant_pool_parse(uint8_t *pool, uint16_t *count, JavaClassConstant ***obj);

#endif
