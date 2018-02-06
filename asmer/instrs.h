#ifndef INSTRS_H
#define INSTRS_H

#include "basic.h"

void         storeInstr     (AsmCode instr, uint32 oriLineNum);
uint32       getRealLineNum (void);
BOOL         isExistInstr   (AsmCode instr);
AsmRecorder* getInstrPtr    (uint32 num); // num begin from 0.

#endif
