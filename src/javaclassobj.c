#include <Python.h>
#include "membuff.h"
#include "access.h"
#include "constant_pool.h"
#include "interfaces.h"
#include "fields.h"
#include "methods.h"
#include "attributes.h"
#include "javaclassobj.h"


void JavaClass_free(JavaClass *this) {
    JavaClassConstantPool_free(this->constant_pool);
    JavaClassInterfaces_free(this->interfaces);
    JavaClassFields_free(this->fields);
    JavaClassMethods_free(this->methods);
    JavaClassAttributes_free(this->attributes);
    PyMem_Free(this);
}

JavaClass *JavaClass_from_MemReader(MemReader *r) {
    JavaClass *class = PyMem_Malloc(sizeof(*class));

    class->magic_number = MemReader_next_uint32(r);
    class->minor_version = MemReader_next_uint16(r);
    class->major_version = MemReader_next_uint16(r);

    constant_pool_parse(r, &class->constant_pool);

    class->access_flags = MemReader_next_uint16(r);
    class->this_class = MemReader_next_uint16(r);
    class->super_class = MemReader_next_uint16(r);

    interfaces_parse(r, &class->interfaces);
    fields_parse(r, &class->fields);
    parse_methods(r, &class->methods);
    attributes_parse(r, &class->attributes);

    return class;
}

void JavaClass_print(JavaClass *class) {
    printf("Magic Number: %X\n", class->magic_number);
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
