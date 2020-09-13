#include "access.h"

void access_flags_print(uint16_t flags) {
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
