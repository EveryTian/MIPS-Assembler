#include "assembler.h"
#include "filectrl.h"
#include "symtlb.h"
#include "translate.h"
#include "instrs.h"

int main(int argc, char* argv[])
{
#if !DEBUG_MODE
    if (argc != 3 && argc != 4)
    {
        showHelp(argv[0]);
        return 0;
    }
	if (4 == argc)
	{
		int origin = atoi(argv[3]);
		if (origin >= 0)
		{
			setOrigin(origin);
		}
	}
#endif
    FILE* inputFile = NULL;
    FILE* outputFile = NULL;
#if !DEBUG_MODE
    if (!(inputFile = openInputFile(argv[1])))
#else
	if (!(inputFile = openInputFile("in.txt")))
#endif
    {
        raiseError("Unable to open file %s", argv[1]);
        return -1;
	}
    assemble();
#if !DEBUG_MODE
	if (!(outputFile = createOutputFile(argv[2])))
#else
	if (!(inputFile = createOutputFile("out.bin")))
#endif
	{
		raiseError("Unable to create file %s", argv[2]);
		return -1;
	}
	writeToFile();
	puts("Done!");
	return 0;
}

void showHelp(char* asmName)
{
    printf("Usage:\n");
    printf("  %s <filename1> <filename2> [Origin(DEC)]\n", asmName);
    printf("  filename1: the name/address of the input file.\n");
    printf("  filename2: the name/address of the output file.\n");
	printf("  Origin: the load word address of the program.(Default: 12288)\n");
    printf("  e.g. %s input.asm output.bin 12288\n", asmName);
    printf("                      By: Haotian_Ren 3150104714@zju.edu.cn\n");
    return;
}

void assemble(void)
{
    pass1();
	pass2();
	return;
}

void pass1(void)
{
    char line[MAX_LINECHARS];
    while (getLineWithNum(line))
    {
        Symbol symbol;
		Mnemonic mnemonic;
		clearAnnotation(line);
		clearUselessSpace(line);
		strTolower(line);
		while (NULL != (symbol = getSymbol(line, getCurLineNum())))
        {
			int symbolLineNum;
			clearUselessSpace(line);
			symbolLineNum = getSymbolSerNum(symbol);
            if (symbolLineNum != -1)
            {
                raiseError("Line %d: Symbol '%s' has been defined in line %d.", getCurLineNum(), symbol, getSymbolPtr(getSymbolSerNum(symbol))->oriLineNum);
                exit(-1);
            }
            storeSymbol(symbol, getCurLineNum(), getRealLineNum());
        }
		clearUselessSpace(line);
		if (strcmp(line, "") == 0)
		{
			continue;
		}
		mnemonic = getMnemonic(line);
		if (!mnemonic)
		{
			raiseError("Line %d: Wrong mnemonic.", getCurLineNum());
			exit(-1);
		}
#if PSEDO_MODE
		if (checkPsedo(mnemonic))
		{
			AsmCode instr;
			instr = getRealInstr(line);
			while (instr)
			{
				storeInstr(instr, getCurLineNum());
			}
		}
		else
		{
#endif
			storeInstr(line, getCurLineNum());
#if PSEDO_MODE
		}
#endif
    }
    if (!isFileEnd())
    {
        raiseError("Line %d: The line is too long.", getCurLineNum());
        exit(-1);
    }
    return;
}

void pass2(void)
{
	uint32 i;
	AsmRecorder* instrPtr;
	for (i = 0; i < getRealLineNum(); ++i)
	{
		instrPtr = getInstrPtr(i);
		if (!clearInBracSpace(instrPtr->instruction))
		{
			raiseError("Line %d: Brackets not matched.", instrPtr->oriLineNum);
		}
		genMachineCode(instrPtr);
	}
	return;
}


void genMachineCode(AsmRecorder* asmRecorderPtr)
{
#define GET_REGISTER(rx) \
		registerName = strtok(NULL, delimiters); \
        rx = getRegisterSerNum(registerName); \
        if (-1 == rx) \
        { \
            raiseError("Line %d: Wrong register name '%s'.", asmRecorderPtr->oriLineNum, registerName); \
            exit(-1); \
        }
#define CHECKEND \
        if (!strtok(NULL, delimiters)) \
        { \
            if (-1 == rt) \
            { \
                raiseError("Line %d: Wrong instruction format.", asmRecorderPtr->oriLineNum); \
                exit(-1); \
            } \
        }
#define GET_LABELORINNE \
        labelOrImme = strtok(NULL, delimiters); \
        if (!labelOrImme) \
        { \
            raiseError("Line %d: Wrong instruction format.", asmRecorderPtr->oriLineNum); \
            exit(-1); \
        } \
        immeOrOffset = getImmeOrOffset(labelOrImme, asmRecorderPtr);
#define GET_IMME \
        labelOrImme = strtok(NULL, delimiters); \
        if (!labelOrImme) \
        { \
            raiseError("Line %d: Wrong instruction format.", asmRecorderPtr->oriLineNum); \
            exit(-1); \
        } \
        immeOrOffset = getImme(labelOrImme, asmRecorderPtr);

	static char* delimiters = " \t,()";
	if (!asmRecorderPtr)
	{
		return;
	}
	char* instr = asmRecorderPtr->instruction;
	Mnemonic mnemonic = strtok(instr, delimiters);
	int32 mnemonicSerNum = getMnemonicSerNum(mnemonic);
	char *registerName;
	if (-1 == mnemonicSerNum)
	{
		raiseError("Line %d: Unsupported mnemonic.", asmRecorderPtr->oriLineNum);
		exit(-1);
	}
	if (mnemonicSerNum < R_NUM)
	{
		uint32 rd, rs, rt;
		int32 schamt;
		rd = rs = rt = schamt = 0x0;
		asmRecorderPtr->code = getFuncOrOpcode(mnemonicSerNum);
		switch (asmRecorderPtr->code)
		{
		case 0x20:
		case 0x21:
		case 0x24:
		case 0x27:
		case 0x25:
		case 0x2A:
		case 0x2B:
		case 0x22:
		case 0x23:
		case 0x26: // dst
			GET_REGISTER(rd)
			GET_REGISTER(rs)
			GET_REGISTER(rt)
			CHECKEND
			break;
		case 0x00:
		case 0x03:
		case 0x02: // dta
			GET_REGISTER(rd)
			GET_REGISTER(rt)
			char* schamtStr = strtok(NULL, delimiters);
			if (!schamtStr)
			{
				raiseError("Line %d: Wrong instruction format.", asmRecorderPtr->oriLineNum);
				exit(-1);
			}
			schamt = getImme(schamtStr, asmRecorderPtr);
			schamt &= 0x1F;
			CHECKEND
			break;
		case 0x04:
		case 0x07:
		case 0x06: // dts
			GET_REGISTER(rd)
			GET_REGISTER(rt)
			GET_REGISTER(rs)
			CHECKEND
			break;
		case 0x09: // ds
			GET_REGISTER(rd)
			GET_REGISTER(rs)
			CHECKEND
			break;
		case 0x1A:
		case 0x1B:
		case 0x18:
		case 0x19: // st
			GET_REGISTER(rs)
			GET_REGISTER(rt)
			CHECKEND
			break;
		case 0x08:
		case 0x11:
		case 0x13: // s
			GET_REGISTER(rs)
			CHECKEND
			break;
		case 0x10:
		case 0x12: // d
			GET_REGISTER(rd)
			CHECKEND
			break;
		case 0x0D:
		case 0x0C: // 0
			CHECKEND
			break;
		}
		asmRecorderPtr->code |= (rs << 21);
		asmRecorderPtr->code |= (rt << 16);
		asmRecorderPtr->code |= (rd << 11);
		asmRecorderPtr->code |= (schamt << 6);
		return;
	}
	else
	{
		uint32 rt, rs, target;
		char* labelOrImme;
		int32 immeOrOffset;
		rt = rs = immeOrOffset = 0x0;
		asmRecorderPtr->code = getFuncOrOpcode(mnemonicSerNum) << 26;
		switch (asmRecorderPtr->code)
		{
		case 0x02 << 26:
		case 0x03 << 26: // J
			labelOrImme = strtok(NULL, delimiters);
			if (!labelOrImme) // Variable labelOrImme donates target now.
			{
				raiseError("Line %d: Wrong instruction format.", asmRecorderPtr->oriLineNum);
				exit(-1);
			}
			target = getTarget(labelOrImme, asmRecorderPtr);
			CHECKEND
			target &= 0x3FFFFFF;
			asmRecorderPtr->code |= target;
			return;
		case 0x08 << 26:
		case 0x09 << 26:
		case 0x0C << 26:
		case 0x0D << 26:
		case 0x0A << 26:
		case 0x0B << 26:
		case 0x0E << 26: // rt, rs, immediate
			GET_REGISTER(rt)
			GET_REGISTER(rs)
			GET_IMME
			CHECKEND
			immeOrOffset &= 0xFFFF;
			break;
		case 0x04 << 26:
		case 0x05 << 26: // rs, rt, label
			GET_REGISTER(rs)
			GET_REGISTER(rt)
			GET_LABELORINNE
			CHECKEND
			immeOrOffset &= 0xFFFF;
			break;
		case 0x01 << 26:
			if (strcmp(mnemonic, "bgez") == 0)
			{
				rt = 0x1;
			}
		case 0x07 << 26:
		case 0x06 << 26: // rs, label
			GET_REGISTER(rs)
			GET_LABELORINNE
			CHECKEND
			immeOrOffset &= 0xFFFF;
			break;
		case 0x20 << 26:
		case 0x24 << 26:
		case 0x21 << 26:
		case 0x25 << 26:
		case 0x23 << 26:
		case 0x31 << 26:
		case 0x28 << 26:
		case 0x29 << 26:
		case 0x2B << 26:
		case 0x39 << 26: // rt, immediate(rs)
			GET_REGISTER(rt)
			GET_IMME
			GET_REGISTER(rs)
			CHECKEND
			immeOrOffset &= 0xFFFF;
			break;
		case 0x0F << 26: // rt, immediate
			GET_REGISTER(rt)
			GET_IMME
			CHECKEND
			immeOrOffset &= 0xFFFF;
			break;
		}
		asmRecorderPtr->code |= immeOrOffset;
		asmRecorderPtr->code |= (rt << 16);
		asmRecorderPtr->code |= (rs << 21);
		return;
	}
#undef GET_REGISTER
#undef CHECKEND
#undef GET_SYMBOLORIMME
#undef GET_IMME
}


int32 getImmeOrOffset(char* labelOrImme, AsmRecorder* asmRecorderPtr)
{
	int32 symbolSerNum, immeOrOffset;
	if (('0' <= labelOrImme[0] && labelOrImme[0] <= '9') || '-' == labelOrImme[0])
	{
		immeOrOffset = getImme(labelOrImme, asmRecorderPtr);
	}
	else
	{
		symbolSerNum = getSymbolSerNum(labelOrImme);
		if (-1 == symbolSerNum)
		{
			raiseError("Line %d: Symbol '%s' is not defined.", asmRecorderPtr->oriLineNum, labelOrImme);
			exit(-1);
		}
		immeOrOffset = getSymbolPtr(symbolSerNum)->realLineNum - asmRecorderPtr->realLineNum - 1;
	}
	return immeOrOffset;
}

uint32 getTarget(char* targetStr, AsmRecorder* asmRecorderPtr)
{
	int32 symbolSerNum, target;
	if (('0' <= targetStr[0] && targetStr[0] <= '9') || '-' == targetStr[0])
	{
		target = getImme(targetStr, asmRecorderPtr);
	}
	else
	{
		symbolSerNum = getSymbolSerNum(targetStr);
		target = getSymbolPtr(symbolSerNum)->realLineNum - 1 + getOrigin();
		if (-1 == symbolSerNum)
		{
			raiseError("Line %d: Symbol '%s' is not defined.", asmRecorderPtr->oriLineNum, target);
			exit(-1);
		}
	}
	return target;
}

// Rewrite this function to support expression.
int32 getImme(char* labelOrImme, AsmRecorder* asmRecorderPtr)
{
	uint32 i;
	if ((labelOrImme[0] < '0' || labelOrImme[0] > '9') && !('-' == labelOrImme[0] && '0' <= labelOrImme[1] && labelOrImme[1] <= '9'))
	{
		raiseError("Line %d: '%s' is not an integer.", asmRecorderPtr->oriLineNum, labelOrImme);
		exit(-1);
	}
	for (i = 1; labelOrImme[i] != '\0'; ++i)
	{
		if (labelOrImme[i] < '0' || labelOrImme[i] > '9')
		{
			raiseError("Line %d: '%s' is not an integer.", asmRecorderPtr->oriLineNum, labelOrImme);
			exit(-1);
		}
	}
	return atoi(labelOrImme);
}

void writeToFile(void)
{
	uint32 i;
	writeMachineCode(getOrigin());
	for (i = 0; i < getRealLineNum(); ++i)
	{
		writeMachineCode(getInstrPtr(i)->code);
	}
	return;
}
