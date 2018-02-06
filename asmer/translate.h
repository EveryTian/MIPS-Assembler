#ifndef TRANSLATE_H
#define TRANSLATE_H

#include "basic.h"

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

#define PSEDO_TABLE \
        PSEDO_ITEM("move", 1) \
        PSEDO_ITEM("blt", 2) \
        PSEDO_ITEM("bgt", 2) \
		PSEDO_ITEM("ble", 2) \
        PSEDO_ITEM("bge", 2) \
        PSEDO_ITEM("abs", 3)
//         psedo  realLength

#define PSEDO_NUM 6

void     setOrigin         (uint32 userOrigin);
uint32   getOrigin         (void);
void     clearAnnotation   (char* line);
void     clearUselessSpace (char* line);
Symbol   getSymbol         (char* line, uint32 curLineNum); // curLineNum is used for exception handle.
char     getReasonableChr  (char c);
void     strTolower        (char* str);
Mnemonic getMnemonic       (AsmCode instr);
int32    getPsedoSerNum    (Mnemonic mnemonic);
AsmCode  getRealInstr      (void);
AsmCode  getRealInstrLine  (int32 psedoSerNum, uint32 lineNum);
int32    getMnemonicSerNum (Mnemonic mnemonic);
uint32   getFuncOrOpcode   (int32 serNum);
int32    getRegisterSerNum (char* registerName);
AsmCode  clearInBracSpace  (AsmCode line);

#endif
