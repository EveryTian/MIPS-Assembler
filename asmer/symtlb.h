#ifndef SYMTLB_H
#define SYMTLB_H

#include "basic.h"

int32           getSymbolSerNum (Symbol symbol);
void            storeSymbol     (Symbol symbol, uint32 curLineNum, uint32 realLineNum);
SymbolRecorder* getSymbolPtr    (uint32 num);

#endif
