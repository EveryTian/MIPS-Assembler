#include "error.h"

void raiseError(char *errMsg, ...)
{
    va_list args;
    fprintf(stderr, "Error: ");
    va_start(args, errMsg);
    vfprintf(stderr, errMsg, args);
    va_end(args);
    fprintf(stderr, "\n");
    return;
}