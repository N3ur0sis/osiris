#include "string.h"

void *memset(void *dest, int val, unsigned int len) {
    unsigned char *ptr = (unsigned char *) dest;
    while (len-- > 0)
        *ptr++ = (unsigned char) val;
    return dest;
}
