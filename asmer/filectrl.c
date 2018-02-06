#include "filectrl.h"

FILE* inputFile = NULL;
FILE* outputFile = NULL;
uint32 curLineNum = 0;
long inputFileSize;

FILE* openInputFile(char *inputName)
{
    inputFile = fopen(inputName, "r");
    if (!inputFile)
    {
        return NULL;
    }
    fseek(inputFile, 0L, SEEK_END);
    inputFileSize = ftell(inputFile);
    fseek(inputFile, 0L, SEEK_SET);
    return inputFile;
}

FILE* createOutputFile(char *outputName)
{
    outputFile = fopen(outputName, "wb");
    return outputFile;
}

void closeFiles(void)
{
    fclose(inputFile);
    fclose(outputFile);
    return;
}

uint32 getLineWithNum(char *line)
{
    if (isFileEnd())
    {
        return 0;
    }
    ++curLineNum;
    fgets(line, MAX_LINECHARS, inputFile);
    if (!strchr(line, '\n'))
    {
        return 0;
    }
    return curLineNum;
}

uint32 getCurLineNum(void)
{
    return curLineNum;
}

BOOL isFileEnd(void)
{
    return ftell(inputFile) == inputFileSize;
}

void writeMachineCode(MachineCode machineCode)
{
	fwrite(&machineCode, sizeof(MachineCode), 1, outputFile);
	return;
}