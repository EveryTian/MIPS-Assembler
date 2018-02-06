#include "symtlb.h"

SymbolRecorder symtbl[MAX_SYMNUM];
uint32 symnum = 0;

int32 getSymbolSerNum(Symbol symbol)
{
    uint32 i;
    for (i = 0; i < symnum; ++i)
    {
        if (strcmp(symbol, symtbl[i].symbol) == 0)
        {
            return i;
        }
    }
    return -1;
}

void storeSymbol(Symbol symbol, uint32 curLineNum, uint32 realLineNum)
{
    if (symnum + 1 == MAX_SYMNUM)
    {
        raiseError("Line %d: Too many symbols.", curLineNum);
        exit(-1);
    }
    symtbl[symnum].oriLineNum = curLineNum;
    symtbl[symnum].realLineNum = realLineNum + 1;
    if (!(symtbl[symnum].symbol = (Symbol)malloc(sizeof(char) * (strlen(symbol) + 1))))
    {
        raiseError("Mallocating failed.");
        exit(-1);
    }
    strcpy(symtbl[symnum].symbol, symbol);
    ++symnum;
    return;
}

SymbolRecorder* getSymbolPtr(uint32 num)
{
	return symtbl + num;
}