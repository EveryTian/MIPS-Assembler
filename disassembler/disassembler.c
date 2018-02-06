// 3150104714@zju.edu.cn Haotian Ren

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define DEBUG 0

#define SHOWALL_MACHINECODE 0

#define R_TABLE \
        R_ITEM("add"    , 0x20) \
        R_ITEM("addu"   , 0x21) \
        R_ITEM("and"    , 0x24) \
        R_ITEM("break"  , 0x0D) \
        R_ITEM("div"    , 0x1A) \
        R_ITEM("divu"   , 0x1B) \
        R_ITEM("jalr"   , 0x09) \
        R_ITEM("jr"     , 0x08) \
        R_ITEM("mfhi"   , 0x10) \
        R_ITEM("mflo"   , 0x12) \
        R_ITEM("mthi"   , 0x11) \
        R_ITEM("mtlo"   , 0x13) \
        R_ITEM("mult"   , 0x18) \
        R_ITEM("multu"  , 0x19) \
        R_ITEM("nor"    , 0x27) \
        R_ITEM("or"     , 0x25) \
        R_ITEM("sll"    , 0x00) \
        R_ITEM("sllv"   , 0x04) \
        R_ITEM("slt"    , 0x2A) \
        R_ITEM("sltu"   , 0x2B) \
        R_ITEM("sra"    , 0x03) \
        R_ITEM("srav"   , 0x07) \
        R_ITEM("srl"    , 0x02) \
        R_ITEM("srlv"   , 0x06) \
        R_ITEM("sub"    , 0x22) \
        R_ITEM("subu"   , 0x23) \
        R_ITEM("syscall", 0x0C) \
        R_ITEM("xor"    , 0x26)
//     mnemonic   func

#define R_NUM 28

#define IJ_TABLE \
        IJ_ITEM("addi"  , 0x08) \
        IJ_ITEM("addiu" , 0x09) \
        IJ_ITEM("andi"  , 0x0C) \
        IJ_ITEM("beq"   , 0x04) \
        IJ_ITEM("bgez"  , 0x01) \
        IJ_ITEM("bgtz"  , 0x07) \
        IJ_ITEM("blez"  , 0x06) \
        IJ_ITEM("bltz"  , 0x01) \
        IJ_ITEM("bne"   , 0x05) \
        IJ_ITEM("lb"    , 0x20) \
        IJ_ITEM("lbu"   , 0x24) \
        IJ_ITEM("lh"    , 0x21) \
        IJ_ITEM("lhu"   , 0x25) \
        IJ_ITEM("lui"   , 0x0F) \
        IJ_ITEM("lw"    , 0x23) \
        IJ_ITEM("lwc1"  , 0x31) \
        IJ_ITEM("ori"   , 0x0D) \
        IJ_ITEM("sb"    , 0x28) \
        IJ_ITEM("slti"  , 0x0A) \
        IJ_ITEM("sltiu" , 0x0B) \
        IJ_ITEM("sh"    , 0x29) \
        IJ_ITEM("sw"    , 0x2B) \
        IJ_ITEM("swc1"  , 0x39) \
        IJ_ITEM("xori"  , 0x0E) \
        IJ_ITEM("j"     , 0x02) \
        IJ_ITEM("jal"   , 0x03)
//      mnemonic  opcode

#define I_NUM 24
#define J_NUM 2

char* mnemonicTable[] =
{
#define R_ITEM(a, b) a,
    R_TABLE
#undef R_ITEM
#define IJ_ITEM(a, b) a,
    IJ_TABLE
#undef IJ_ITEM
    ""
};

const uint32_t funcOrOpcodeTable[] =
{
#define R_ITEM(a, b) b,
    R_TABLE
#undef R_ITEM
#define IJ_ITEM(a, b) b,
    IJ_TABLE
#undef IJ_TABLE
    0x0
};

const char* registerTable[] =
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

struct INSTR_NODE
{
    char mnemonic[8];
    char block[3][16];
    int usedBlocksNum;
    uint32_t machineCode;
};
typedef struct INSTR_NODE InstrNode;

long openInputFile(const char* inputFileName, FILE** inputFile);
void readInputFile(InstrNode* instrNodes, FILE* inputFile, long fileLen);
void disassembleNodes(InstrNode* instrNodes, long fileLen);
void disassembleNode(InstrNode* instrNode);
void genRegisterName(char* str, uint32_t registerSerNum);
void genImme(char* str, uint32_t imme);
void writeOutputFile(FILE* outputFile, InstrNode* instrNodes, long fileLen);
char* getRMnemonic(uint32_t func);
char* getIJMnemonic(uint32_t opcode);

int main(int argc, char const *argv[])
{
#if !DEBUG
    if (argc != 3)
    {
        printf("Disassembler Usage:\n");
        printf("  %s <filename1> <filename2>\n", argv[0]);
        printf("  filename1: the name/address of the input file.\n");
        printf("  filename2: the name/address of the output file.\n");
        printf("  e.g. %s input.bin output.asm\n", argv[0]);
        printf("              By: Haotian Ren (3150104714@zju.edu.cn)\n");
        return 0;
    }
#endif
    FILE* inputFile;
#if DEBUG
    long fileLen = openInputFile("test.bin", &inputFile);
#else
    long fileLen = openInputFile(argv[1], &inputFile);
#endif

    if (-1 == fileLen)
    {
        fprintf(stderr, "Error: Can not open the file '%s'.\n", argv[1]);
        return -1;
    }
    if (fileLen % sizeof(uint32_t) != 0)
    {
        fprintf(stderr, "Error: Wrong input file.\n");
        fclose(inputFile);
        return -1;
    }
    fileLen /= sizeof(uint32_t);
    InstrNode* instrNodes = (InstrNode*)malloc(sizeof(InstrNode) * fileLen);
    if (!instrNodes)
    {
        fprintf(stderr, "Error: Meet wrong when mallocate the memory.\n");
        fclose(inputFile);
        return -1;
    }
    readInputFile(instrNodes, inputFile, fileLen);
    disassembleNodes(instrNodes, fileLen);
#if DEBUG
    FILE* outputFile = stdout;
#else
    FILE* outputFile = fopen(argv[2], "w");
#endif
    if (!outputFile)
    {
        fprintf(stderr, "Error: Can not create the file '%s'.\n", argv[2]);
        fclose(inputFile);
        free(instrNodes);
        return -1;
    }
    writeOutputFile(outputFile, instrNodes, fileLen);
    fclose(outputFile);
    fclose(inputFile);
    free(instrNodes);
#if !DEBUG
    puts("Done!");
#endif
    return 0;
}

long openInputFile(const char* inputFileName, FILE** inputFilePtr)
{
    *inputFilePtr = fopen(inputFileName, "rb");
    if (!*inputFilePtr)
    {
        return -1;
    }
    fseek(*inputFilePtr, 0L, SEEK_END);
    long fileLen = ftell(*inputFilePtr);
    fseek(*inputFilePtr, 0L, SEEK_SET);
    return fileLen;
}

void readInputFile(InstrNode* instrNodes, FILE* inputFile, long fileLen)
{
    long i;
    for (i = 0; i < fileLen; ++i)
    {
        fread(&instrNodes[i].machineCode, 1, sizeof(uint32_t), inputFile);
    }
    return;
}

void disassembleNodes(InstrNode* instrNodes, long fileLen)
{
    long i;
    for (i = 0; i < fileLen; ++i)
    {
        disassembleNode(instrNodes + i);
    }
    return;
}

char* getRMnemonic(uint32_t func)
{
    int i;
    for (i = 0; i < R_NUM; ++i)
    {
        if (funcOrOpcodeTable[i] == func)
        {
            return mnemonicTable[i];
        }
    }
    return NULL;
}

char* getIJMnemonic(uint32_t opcode)
{
    int i;
    for (i = R_NUM; i < R_NUM + I_NUM + J_NUM; ++i)
    {
        if (funcOrOpcodeTable[i] == opcode)
        {
            return mnemonicTable[i];
        }
    }
    return NULL;
}

void disassembleNode(InstrNode* instrNode)
{
    uint32_t machineCode = instrNode->machineCode;
    uint32_t opcode = (machineCode & 0xFC000000) >> 26;
    uint32_t func, rd, rs, rt, shamt;
    int32_t imme, target;
    rd = (machineCode & 0xF800) >> 11;
    rt = (machineCode & 0x1F0000) >> 16;
    rs = (machineCode & 0x3E00000) >> 21;
    shamt = (machineCode & 0x3E0) >> 6;
    func = machineCode & 0x3F;
    imme = machineCode & 0xFFFF;
    imme = (imme << 16) >> 16;
    target = machineCode & 0x3FFFFFF;
    target = (target << 6) >> 6;
    if (0x0 == opcode) // R
    {
        strcpy(instrNode->mnemonic, getRMnemonic(func));
        switch (func)
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
            genRegisterName(instrNode->block[0], rd);
            genRegisterName(instrNode->block[1], rs);
            genRegisterName(instrNode->block[2], rt);
            instrNode->usedBlocksNum = 3;
            break;
        case 0x00:
        case 0x03:
        case 0x02: // dta
            genRegisterName(instrNode->block[0], rd);
            genRegisterName(instrNode->block[1], rt);
            genImme(instrNode->block[2], shamt);
            instrNode->usedBlocksNum = 3;
            break;
        case 0x04:
        case 0x07:
        case 0x06: // dts
            genRegisterName(instrNode->block[0], rd);
            genRegisterName(instrNode->block[1], rt);
            genRegisterName(instrNode->block[2], rs);
            instrNode->usedBlocksNum = 3;
            break;
        case 0x09: // ds
            genRegisterName(instrNode->block[0], rd);
            genRegisterName(instrNode->block[1], rs);
            instrNode->usedBlocksNum = 2;
            break;
        case 0x1A:
        case 0x1B:
        case 0x18:
        case 0x19: // st
            genRegisterName(instrNode->block[0], rs);
            genRegisterName(instrNode->block[1], rt);
            instrNode->usedBlocksNum = 2;
            break;
        case 0x08:
        case 0x11:
        case 0x13: // s
            genRegisterName(instrNode->block[0], rs);
            instrNode->usedBlocksNum = 1;
            break;
        case 0x10:
        case 0x12: // d
            genRegisterName(instrNode->block[0], rd);
            instrNode->usedBlocksNum = 1;
            break;
        case 0x0D:
        case 0x0C: // 0
            instrNode->usedBlocksNum = 0;
            break;
        default:
            instrNode->usedBlocksNum = -1;
            break;
        }
    }
    else
    {
        strcpy(instrNode->mnemonic, getIJMnemonic(opcode));
        if (0x1 == opcode && 0x0 == rt)
        {
            strcpy(instrNode->mnemonic, "bltz");
        }
        else if (0x1 == opcode && 0x1 == rt)
        {
            strcpy(instrNode->mnemonic, "bgez");
        } // Actually this part can be delted.
        switch (opcode)
        {
        case 0x02:
        case 0x03: // J
            genImme(instrNode->block[0], target);
            instrNode->usedBlocksNum = 1;
            break;
        case 0x08:
        case 0x09:
        case 0x0C:
        case 0x0D:
        case 0x0A:
        case 0x0B:
        case 0x0E: // rt, rs, immediate
            genRegisterName(instrNode->block[0], rt);
            genRegisterName(instrNode->block[1], rs);
            genImme(instrNode->block[2], imme);
            instrNode->usedBlocksNum = 3;
            break;
        case 0x04:
        case 0x05: // rs, rt, label
            genRegisterName(instrNode->block[0], rs);
            genRegisterName(instrNode->block[1], rt);
            genImme(instrNode->block[2], imme);
            instrNode->usedBlocksNum = 3;
            break;
        case 0x01:
        case 0x07:
        case 0x06: // rs, label
            genRegisterName(instrNode->block[0], rs);
            genImme(instrNode->block[1], imme);
            instrNode->usedBlocksNum = 2;
            break;
        case 0x20:
        case 0x24:
        case 0x21:
        case 0x25:
        case 0x23:
        case 0x31:
        case 0x28:
        case 0x29:
        case 0x2B:
        case 0x39: // rt, immediate(rs)
            genRegisterName(instrNode->block[0], rt);
            genImme(instrNode->block[1], imme);
            genRegisterName(instrNode->block[1] + strlen(instrNode->block[1]) + 1, rs);
            instrNode->block[1][strlen(instrNode->block[1])] = '(';
            instrNode->block[1][strlen(instrNode->block[1]) + 1] = '\0';
            instrNode->block[1][strlen(instrNode->block[1])] = ')';
            instrNode->usedBlocksNum = 2;
            break;
        case 0x0F: // rt, immediate
            genRegisterName(instrNode->block[0], rt);
            genImme(instrNode->block[1], imme);
            instrNode->usedBlocksNum = 2;
            break;
        }
    }
    return;
}

void genRegisterName(char* str, uint32_t registerSerNum)
{
    *str++ = '$';
    if (registerSerNum < 32)
    {
        strcpy(str, registerTable[registerSerNum]);
    }
    else
    {
        sprintf(str, "%d", registerSerNum);
    }
    return;
}

void genImme(char* str, uint32_t imme)
{
    sprintf(str, "%d", imme);
    return;
}

void writeOutputFile(FILE* outputFile, InstrNode* instrNodes, long fileLen)
{
    long i;
    for (i = 1; i < fileLen; ++i)
    {
        if (-1 == instrNodes[i].usedBlocksNum)
        {
            fprintf(outputFile, "# MachineCode: 0x%08X\n", instrNodes[i].machineCode);
            continue;
        }
        fprintf(outputFile, "%s ", instrNodes[i].mnemonic);
        int j;
        for (j = 0; j < instrNodes[i].usedBlocksNum; ++j)
        {
            fprintf(outputFile, "%s", instrNodes[i].block[j]);
            if (j != instrNodes[i].usedBlocksNum - 1)
            {
                fprintf(outputFile, ", ");
            }
        }
#if SHOWALL_MACHINECODE
        fprintf(outputFile, " \t# MachineCode: 0x%08X\n", instrNodes[i].machineCode);
#else
        fputc('\n', outputFile);
#endif
    }
    return;
}
