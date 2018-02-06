#include "instrs.h"

AsmRecorder instrs[MAX_LINENUM];
uint32 realLineNum = 0;

void storeInstr(AsmCode instr, uint32 oriLineNum)
{
	if (!isExistInstr(instr))
	{
		return;
	}
    if (realLineNum + 1 == MAX_LINENUM)
    {
        raiseError("Too many lines.");
        exit(-1);
    }
    if (!(instrs[realLineNum].instruction = (AsmCode)malloc(sizeof(char) * (strlen(instr) + 1))))
    {
        raiseError("Mallocating failed.");
        exit(-1);
    }
    strcpy(instrs[realLineNum].instruction, instr);
    instrs[realLineNum].oriLineNum = oriLineNum;
    instrs[realLineNum].realLineNum = realLineNum + 1;
	++realLineNum;
	return;
}

uint32 getRealLineNum(void)
{
    return realLineNum;
}

BOOL isExistInstr(AsmCode instr)
{
	while (' ' == *instr)
	{
		++instr;
	}
	if ('\0' != *instr)
	{
		return TRUE;
	}
	return FALSE;
}

AsmRecorder* getInstrPtr(uint32 num)
{
	return instrs + num;
}