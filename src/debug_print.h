#ifndef DEBUG_PRINT_H
#define DEBUG_PRINT_H

#include "access.h"

static void _JavaClassUtf8Constant_print(JavaClassUtf8Constant *this) {
    printf("Utf8=tag(%u), length(%u), bytes(\"%.*s\")\n",
        this->tag, this->length, this->length, this->bytes);
}

static void _JavaClassClassConstant_print(JavaClassClassConstant *this) {
    printf("Class=tag(%u), name_index(%u)\n",
        this->tag, this->name_index);
}

static void _JavaClassStringConstant_print(JavaClassStringConstant *this) {
    printf("String=tag(%u), string_index(%u)\n",
        this->tag, this->string_index);
}

static void _JavaClassFieldrefConstant_print(JavaClassFieldrefConstant *this) {
    printf("Fieldref=tag(%u), class_index(%u), name_and_type_index(%u)\n",
        this->tag, this->class_index, this->name_and_type_index);
}

static void _JavaClassMethodrefConstant_print(JavaClassMethodrefConstant *this) {
    printf("Methodref=tag(%u), class_index(%u), name_and_type_index(%u)\n",
        this->tag, this->class_index, this->name_and_type_index);
}

static void _JavaClassInterfaceMethodrefConstant_print(JavaClassInterfaceMethodrefConstant *this) {
    printf("InterfaceMethodref=tag(%u), class_index(%u), name_and_type_index(%u)\n",
        this->tag, this->class_index, this->name_and_type_index);
}

static void _JavaClassNameAndTypeConstant_print(JavaClassNameAndTypeConstant *this) {
    printf("NameAndType=tag(%u), name_index(%u), descriptor_index(%u)\n",
        this->tag, this->name_index, this->descriptor_index);
}

static void constant_pool_print(JavaClassConstantPool *this) {
    printf("Constant Pool Count: %d\n", this->count);

    for(int i = 1; i < this->count; ++i) {
        void *c = this->constants[i - 1];

        printf("  **%4d.) ", i);
        if(!c) {
            /* TODO: this branch should be deleted when all types are implemented */
            printf("Not implemented.\n");
            continue;
        }

        switch(((JavaClassConstant *)c)->tag) {
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

static void access_flags_print(uint16_t flags) {
    printf("Access Flags: %u\n", flags);
    printf("* PUBLIC Flag: %x\n", flags & ACC_PUBLIC);
    printf("* FINAL Flag: %x\n", flags & ACC_FINAL);
    printf("* SUPER Flag: %x\n", flags & ACC_SUPER);
    printf("* INTERFACE Flag: %x\n", flags & ACC_INTERFACE);
    printf("* ABSTRACT Flag: %x\n", flags & ACC_ABSTRACT);
    printf("* SYNTHETIC Flag: %x\n", flags & ACC_SYNTHETIC);
    printf("* ANNOTATION Flag: %x\n", flags & ACC_ANNOTATION);
    printf("* ENUM Flag: %x\n", flags & ACC_ENUM);
}

void interfaces_print(JavaClassInterfaces *this) {
    printf("Interfaces count: %d\n", this->count);
    for(uint16_t i = 0; i < this->count; ++i) {
        printf("  * interface: %u\n", this->indexes[i]);
    }
}

void attributes_print(JavaClassAttributes *this) {
    printf("attr count: %u\n", this->attributes_count);
    for(uint16_t i = 0; i < this->attributes_count; ++i) {
        JavaClassAttribute *attr = this->attributes[i];
        printf("** Attr name index: %u\n", attr->attribute_name_index);
        printf("** Attr length: %u\n", attr->attribute_length);
        printf("** Attr info: %.*s\n", attr->attribute_length, attr->info);
    }
}

void fields_print(JavaClassFields *this) {
    printf("Fields count: %u\n", this->count);
    for(uint16_t i = 0; i < this->count; ++i) {
        JavaClassField *field = this->items[i];
        printf("* Field access flags: %u\n", field->access_flags);
        printf("* Field name index: %u\n", field->name_index);
        printf("* Field descriptor index: %u\n", field->descriptor_index);
        printf("* Field attributes:\n");
        attributes_print(field->attributes);
    }
}

void methods_print(JavaClassMethods *this) {
    printf("Methods count: %d\n", this->count);
    for(uint16_t i = 0; i < this->count; ++i) {
        JavaClassMethod *m = this->items[i];
        printf("* Method access_flags: %u\n", m->access_flags);
        printf("* Method name_index: %u\n", m->name_index);
        printf("* Method descriptor_index: %u\n", m->descriptor_index);
        attributes_print(m->attributes);
    }
}

static void JavaClass_print(JavaClass *class) {
    printf("Magic Number: %X\n", class->magic);
    printf("Version: %u.%u\n", class->major_version, class->minor_version);
    constant_pool_print(class->constant_pool);
    access_flags_print(class->access_flags);
    printf("This Class Pool Index: %u\n", class->this_class);
    printf("Super Class Pool Index: %u\n", class->super_class);
    interfaces_print(class->interfaces);
    fields_print(class->fields);
    methods_print(class->methods);
    printf("CLASS ATTRIBUTES:\n");
    attributes_print(class->attributes);
}

#endif
