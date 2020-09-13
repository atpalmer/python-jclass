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

#endif
