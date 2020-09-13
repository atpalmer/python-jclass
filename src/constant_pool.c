#include <Python.h>
#include "parse.h"
#include "constant_pool.h"


static JavaClassConstant *_JavaClassUtf8Constant_from_data(uint8_t *data) {
    JavaClassUtf8Constant *new = PyMem_Malloc(sizeof(*new) + Utf8_length(data));
    new->tag = Constant_tag(data);
    new->length = Utf8_length(data);
    memcpy(new->bytes, Utf8_bytes(data), Utf8_length(data));
    return (JavaClassConstant *)new;
}

static void _JavaClassUtf8Constant_print(JavaClassUtf8Constant *this) {
    printf("Utf8=tag(%u), length(%u), bytes(\"%.*s\")\n",
        this->tag, this->length, this->length, this->bytes);
}


static JavaClassConstant *_JavaClassClassConstant_from_data(uint8_t *data) {
    JavaClassClassConstant *new = PyMem_Malloc(sizeof(*new));
    new->tag = Constant_tag(data);
    new->name_index = Class_name_index(data);
    return (JavaClassConstant *)new;
}

static void _JavaClassClassConstant_print(JavaClassClassConstant *this) {
    printf("Class=tag(%u), name_index(%u)\n",
        this->tag, this->name_index);
}


static JavaClassConstant *_JavaClassStringConstant_from_data(uint8_t *data) {
    JavaClassStringConstant *new = PyMem_Malloc(sizeof(*new));
    new->tag = Constant_tag(data);
    new->string_index = String_string_index(data);
    return (JavaClassConstant *)new;
}

static void _JavaClassStringConstant_print(JavaClassStringConstant *this) {
    printf("String=tag(%u), string_index(%u)\n",
        this->tag, this->string_index);
}


static JavaClassConstant *_JavaClassFieldrefConstant_from_data(uint8_t *data) {
    JavaClassFieldrefConstant *new = PyMem_Malloc(sizeof(*new));
    new->tag = Constant_tag(data);
    new->class_index = Fieldref_class_index(data);
    new->name_and_type_index = Fieldref_name_and_type_index(data);
    return (JavaClassConstant *)new;
}

static void _JavaClassFieldrefConstant_print(JavaClassFieldrefConstant *this) {
    printf("Fieldref=tag(%u), class_index(%u), name_and_type_index(%u)\n",
        this->tag, this->class_index, this->name_and_type_index);
}


static JavaClassConstant *_JavaClassMethodrefConstant_from_data(uint8_t *data) {
    JavaClassMethodrefConstant *new = (JavaClassMethodrefConstant *)_JavaClassFieldrefConstant_from_data(data);
    return (JavaClassConstant *)new;
}

static void _JavaClassMethodrefConstant_print(JavaClassMethodrefConstant *this) {
    printf("Methodref=tag(%u), class_index(%u), name_and_type_index(%u)\n",
        this->tag, this->class_index, this->name_and_type_index);
}


static JavaClassConstant *_JavaClassInterfaceMethodrefConstant_from_data(uint8_t *data) {
    JavaClassInterfaceMethodrefConstant *new = (JavaClassInterfaceMethodrefConstant *)_JavaClassFieldrefConstant_from_data(data);
    return (JavaClassConstant *)new;
}

static void _JavaClassInterfaceMethodrefConstant_print(JavaClassInterfaceMethodrefConstant *this) {
    printf("InterfaceMethodref=tag(%u), class_index(%u), name_and_type_index(%u)\n",
        this->tag, this->class_index, this->name_and_type_index);
}


static JavaClassConstant *_JavaClassNameAndTypeConstant_from_data(uint8_t *data) {
    JavaClassNameAndTypeConstant *new = PyMem_Malloc(sizeof(*new));
    new->tag = Constant_tag(data);
    new->name_index = NameAndType_name_index(data);
    new->descriptor_index = NameAndType_descriptor_index(data);
    return (JavaClassConstant *)new;
}

static void _JavaClassNameAndTypeConstant_print(JavaClassNameAndTypeConstant *this) {
    printf("NameAndType=tag(%u), name_index(%u), descriptor_index(%u)\n",
        this->tag, this->name_index, this->descriptor_index);
}


void constant_pool_print(JavaClassConstant **constants, int count) {
    printf("Constant Pool Count: %d\n", count);

    for(int i = 1; i < count; ++i) {
        JavaClassConstant *c = constants[i - 1];

        printf("  **%4d.) ", i);
        if(!c) {
            /* TODO: this branch should be deleted when all types are implemented */
            printf("Not implemented.\n");
            continue;
        }

        switch(c->tag) {
        case CONSTANT_TYPE_Utf8:
            _JavaClassUtf8Constant_print(c);
            break;
        case CONSTANT_TYPE_Class:
            _JavaClassClassConstant_print(c);
            break;
        case CONSTANT_TYPE_String:
            _JavaClassStringConstant_print(c);
            break;
        case CONSTANT_TYPE_Fieldref:
            _JavaClassFieldrefConstant_print(c);
            break;
        case CONSTANT_TYPE_Methodref:
            _JavaClassMethodrefConstant_print(c);
            break;
        case CONSTANT_TYPE_InterfaceMethodref:
            _JavaClassInterfaceMethodrefConstant_print(c);
            break;
        case CONSTANT_TYPE_NameAndType:
            _JavaClassNameAndTypeConstant_print(c);
            break;
        default:
            break;
        }
    }
}

size_t constant_pool_parse(uint8_t *pool, int count, JavaClassConstant ***obj) {
    *obj = PyMem_Malloc(sizeof(JavaClassConstant *) * count);

    size_t pool_bytes = 0;
    for(int i = 1; i < count; ++i) {
        uint8_t *p = &pool[pool_bytes];
        JavaClassConstant **c = &((*obj)[i - 1]);

        switch(Constant_tag(p)) {
        case CONSTANT_TYPE_Utf8:
            *c = _JavaClassUtf8Constant_from_data(p);
            pool_bytes += 3 + Utf8_length(p);
            break;

        case CONSTANT_TYPE_Class:
            *c = _JavaClassClassConstant_from_data(p);
            pool_bytes += 3;
            break;

        case CONSTANT_TYPE_String:
            *c = _JavaClassStringConstant_from_data(p);
            pool_bytes += 3;
            break;

        case CONSTANT_TYPE_Fieldref:
            *c = _JavaClassFieldrefConstant_from_data(p);
            pool_bytes += 5;
            break;

        case CONSTANT_TYPE_Methodref:
            *c = _JavaClassMethodrefConstant_from_data(p);
            pool_bytes += 5;
            break;

        case CONSTANT_TYPE_InterfaceMethodref:
            *c = _JavaClassInterfaceMethodrefConstant_from_data(p);
            pool_bytes += 5;
            break;

        case CONSTANT_TYPE_NameAndType:
            *c = _JavaClassNameAndTypeConstant_from_data(p);
            pool_bytes += 5;
            break;

        default:
            *c = NULL;
            break;
        }
    }

    return pool_bytes;
}
