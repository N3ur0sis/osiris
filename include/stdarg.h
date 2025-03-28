#ifndef _STDARG_H
#define _STDARG_H

typedef char* va_list;

#define VA_ALIGN(t) (((sizeof(t) + sizeof(int) - 1) / sizeof(int)) * sizeof(int))

#define va_start(ap, last_arg) (ap = (va_list)(&last_arg + 1))
#define va_arg(ap, type) (*(type *)((ap += VA_ALIGN(type)) - VA_ALIGN(type)))
#define va_end(ap) (ap = (va_list)0)

#endif
