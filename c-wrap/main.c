
#include <stdarg.h>
#include <stdio.h>

#include "args.h"

void _variad(size_t argc, ...) {
    va_list ap;
    va_start(ap, argc);
    for (int i = 0; i < (int) argc; i++) {
        printf("%d ", va_arg(ap, int));
    }
    printf("\n");
    va_end(ap);
}

void _variad(size_t argc, ...);
#define variad(...) _variad(PP_NARG(__VA_ARGS__), __VA_ARGS__)


int main()
{
    variad(2, 4, 6, 8, 10);
    return 0;
}

