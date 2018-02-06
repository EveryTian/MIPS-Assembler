#include "translate.h"

const char* mnemonicTable[] =
{
#define R_ITEM(a, b) a,
	R_TABLE
#undef R_ITEM
#define IJ_ITEM(a, b) a,
	IJ_TABLE
#undef IJ_ITEM
	""
};

const uint32 funcOrOpcodeTable[] =
{
#define R_ITEM(a, b) b,
	R_TABLE
#undef R_ITEM
#define IJ_ITEM(a, b) b,
	IJ_TABLE
#undef IJ_TABLE
	0x0
};

const char* psedoTable[] =
{
#define PSEDO_ITEM(a, b) a,
	PSEDO_TABLE
#undef PSEDO_ITEM
	""
};

const uint32 psedoRealLengthTable[] =
{
#define PSEDO_ITEM(a, b) b,
	PSEDO_TABLE
#undef PSEDO_ITEM
	0
};

char* registerTable[] =
{
	"zero",
	"at",
	"v0", "v1",
	"a0", "a1", "a2", "a3",
	"t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
	"s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
	"t8", "t9",
	"k0", "k1",
	"gp",
	"sp",
	"fp",
	"ra",
	""
};

uint32 origin = DEFAULT_ORIGIN;

void setOrigin(uint32 userOrigin)
{
	origin = userOrigin;
	return;
}

uint32 getOrigin(void)
{
	return origin;
}

void clearAnnotation(char* line)
{
    uint32 i;
    for (i = 0; line[i] != '\0'; ++i)
    {
        if (ANNOTATION_SIGN == line[i] || '\n' == line[i])
        {
            line[i] = '\0';
            return;
        }
    }
    return;
}

void clearUselessSpace(char* line)
{
	static char* uselessSpaces = " \t\n\r\b\v\f\a";
	uint32 lineLen = strlen(line);
	int32 i = lineLen - 1;
	while (i >= 0 && strchr(uselessSpaces, line[i]))
	{
		--i;
	}
	line[i + 1] = '\0';
	i = 0;
	while (((uint32)i) < lineLen && strchr(uselessSpaces, line[i]))
	{
		++i;
	}
	strcpy(line, line + i);
	return;
}

Symbol getSymbol(char* line, uint32 curLineNum)
{
    static char symbol[MAX_SYMBOLCHARS];
    char* tmps;
    uint32 i;
    if (!(tmps = strchr(line, ':')))
    {
        return NULL;
    }
    *tmps = '\0';
    line[0] = getReasonableChr(line[0]);
    if ('\0' == line[0] ||('0' <= line[0] && line[0] <= '9'))
    {
        raiseError("Line %d: Wrong symbol format.", curLineNum);
        exit(-1);
    }
    symbol[0] = line[0];
    for (i = 1; line[i] != '\0'; ++i)
    {
        line[i] = getReasonableChr(line[i]);
        if ('\0' == line[i])
        {
            raiseError("Line %d: Wrong symbol format.", curLineNum);
            exit(-1);
        }
        symbol[i] = line[i];
        if (i + 1 == MAX_SYMBOLCHARS)
        {
            if ('\0' == line[i])
            {
                raiseError("Line %d: Symbol is too long.", curLineNum);
                exit(-1);
            }
        }
    }
    symbol[i] = '\0';
    strcpy(line, tmps + 1);
    return symbol;
}

char getReasonableChr(char c)
{
    if ('_' == c || ('0' <= c && c <= '9'))
    {
        return c;
    }
    c |= 0x20;
    if ('a' <= c && c <= 'z')
    {
        return c;
    }
    return '\0';
}

void strTolower(char* str)
{
	while (*str != '\0')
	{
		*str = tolower(*str);
		++str;
	}
	return;
}

Mnemonic getMnemonic(AsmCode instr)
{
	static char mnemonic[MAX_MNEMONIC];
	uint32 i;
	for (i = 0; i < MAX_MNEMONIC && instr[i] != '\0'; ++i)
	{
		if (' ' == instr[i])
		{
			instr[i] = '\0';
			strcpy(mnemonic, instr);
			instr[i] = ' ';
			return mnemonic;
		}
	}
	return NULL;
}

int32 psedoSerNum = -1;
uint32 psedoRealLength = 0;

int32 getPsedoSerNum(Mnemonic mnemonic)
{
	uint32 i;
	for (i = 0; strcmp(psedoTable[i], "") != 0; ++i)
	{
		if (strcmp(psedoTable[i], mnemonic) == 0)
		{
			psedoSerNum = i;
			psedoRealLength = psedoRealLengthTable[i];
			return i;
		}
	}
	psedoSerNum = -1;
	psedoRealLength = 0;
	return -1;
}

AsmCode getRealInstr(void)
{
	static uint32 curPsedoRealLineNum = 0;
	if (++curPsedoRealLineNum > psedoRealLengthTable[psedoSerNum])
	{
		curPsedoRealLineNum = 0;
		psedoSerNum = -1;
		psedoRealLength = 0;
		return NULL;
	}
	AsmCode realInstr = getRealInstrLine(psedoSerNum, curPsedoRealLineNum);
	if (!realInstr)
	{
		curPsedoRealLineNum = 0;
		psedoSerNum = -1;
		psedoRealLength = 0;
		return NULL;
	}
	return realInstr;
}

// Write this function to implement the transation of psedoinstructions.
AsmCode getRealInstrLine(int32 psedoSerNum, uint32 lineNum)
{
	return NULL;
}

int32 getMnemonicSerNum(Mnemonic mnemonic)
{
	int32 i;
	for (i = 0; strcmp(mnemonicTable[i], "") != 0; ++i)
	{
		if (strcmp(mnemonic, mnemonicTable[i]) == 0)
		{
			return i;
		}
	}
	return -1;
}

uint32 getFuncOrOpcode(int32 serNum)
{
	return funcOrOpcodeTable[serNum];
}

int32 getRegisterSerNum(char* registerName)
{
	static char tmp[MAX_MNEMONIC];
	if (strlen(registerName) >= MAX_MNEMONIC || (*registerName != '$' && !('(' == *registerName && '$' == *(registerName + 1))))
	{
		return -1;
	}
	if (*registerName == '(')
	{
		strcpy(tmp, registerName + 2);
		tmp[strlen(tmp) - 1] = '\0';
	}
	else
	{
		strcpy(tmp, registerName + 1);
	}
	int32 i;
	for (i = 0; strcmp(registerTable[i], "") != 0; ++i)
	{
		if (strcmp(tmp, registerTable[i]) == 0)
		{
			return i;
		}
	}
	int32 registerNum = i;
	for (i = 0; i < registerNum; ++i)
	{
		char s[4];
		sprintf(s, "%d", i);
		if (strcmp(tmp, s) == 0)
		{
			return i;
		}
	}
	return -1;
}

AsmCode clearInBracSpace(AsmCode line)
{
	static char tmp[MAX_LINECHARS];
	uint32 i, j;
	uint32 notPairedLeftBracketsNum = 0;
	for (i = j = 0; line[i] != '\0'; ++i)
	{
		switch (line[i])
		{
		case '(':
			++notPairedLeftBracketsNum;
			tmp[j++] = line[i];
			break;
		case ')':
			--notPairedLeftBracketsNum;
			tmp[j++] = line[i];
			break;
		case ' ':
			if (notPairedLeftBracketsNum == 0)
			{
				tmp[j++] = ' ';
			}
			else if (notPairedLeftBracketsNum < 0)
			{
				return NULL;
			}
			break;
		case '$':
			
		default:
			tmp[j++] = line[i];
			break;
		}
	}
	tmp[j] = '\0';
	strcpy(line, tmp);
	return line;
}
