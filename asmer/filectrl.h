#ifndef FILECTRL_H
#define FILECTRL_H

#include "basic.h"

FILE*  openInputFile    (char *inputName);
FILE*  createOutputFile (char *outputName);
void   closeFiles       (void);
uint32 getLineWithNum   (char *line);
uint32 getCurLineNum    (void);
BOOL   isFileEnd        (void);
void   writeMachineCode (MachineCode machineCode);

#endif
