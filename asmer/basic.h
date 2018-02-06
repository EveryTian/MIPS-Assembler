#ifndef BASIC_H
#define BASIC_H

#define _CRT_SECURE_NO_WARNINGS

#define DEBUG_MODE 0

#define PSEDO_MODE 0

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "error.h"

#define MAX_SYMNUM 512
#define MAX_LINENUM 2048
#define MAX_MNEMONIC 8
#define MAX_SYMBOLCHARS 16
#define MAX_LINECHARS 256
#define ANNOTATION_SIGN '#'

#define DEFAULT_ORIGIN 0x3000

typedef uint32_t MachineCode;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef int32_t  int32;
typedef int64_t  int64;

typedef char BOOL;
#ifdef TRUE
#undef TRUE
#endif
#define TRUE 1
#ifdef FALSE
#undef FALSE
#endif
#define FALSE 0

typedef char* AsmCode;
typedef char* Symbol;
typedef char* Mnemonic;

typedef struct ASM_RECORDER AsmRecorder;
struct ASM_RECORDER
{
    uint32 oriLineNum;
    uint32 realLineNum;
    AsmCode instruction;
    MachineCode code;
};

typedef struct SYMBOL_RECORDER SymbolRecorder;
struct SYMBOL_RECORDER
{
    uint32 oriLineNum;
    uint32 realLineNum;
    Symbol symbol;
};

#endif
