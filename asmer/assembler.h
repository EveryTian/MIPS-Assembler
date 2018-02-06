#ifndef ASMER_H
#define ASMER_H

#include "basic.h"

void   showHelp        (char* asmName);
void   assemble        (void);
void   pass1           (void);
void   pass2           (void);
void   genMachineCode  (AsmRecorder* asmRecorderPtr);
int32  getImmeOrOffset (char* labelOrImme, AsmRecorder* asmRecorderPtr);
uint32 getTarget       (char* targetStr, AsmRecorder* asmRecorderPtr);
int32  getImme         (char* labelOrImme, AsmRecorder* asmRecorderPtr);
void   writeToFile     (void);

#endif
