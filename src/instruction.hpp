#include <iostream>

enum NextPC_Cond_Enum{
ALWAYS_NEXT,
IF_EQUAL_NEXT2,
IF_LESS_NEXT2,
IF_GREAT_NEXT2,
IF_REPEAT_ENDS_NEXT2
};


enum OP_CODE_Enum{
	NULL_OPCODE_KEEP_OUTPUT, //(UNUSED) TODO: make the value of this to zero also in the RTL code
	ADD_OPCODE,
    MULT_OPCODE,
    XOR_OPCODE,
    NOR_OPCODE,
	AND_OPCODE,
    OR_OPCODE,
	SHIFT_OPCODE,
	FLOAT_MULT_OPCODE,
	FLOAT_ADD_OPCODE,
	FLOAT_SUB_OPCODE,
	SUB_OPCODE,
	COMPARE_OPCODE,//Add OPCODE to the programms in the ALPU library
    PASS_INPUT_TO_OUTPUT_OPCODE //used only for storing broadcast values in SPMM

};

enum SRC_OP_Enum{
SRC_ROW1,//=0,
SRC_ROW2,//=1,
SRC_GLLOBAL_BUS,//=2,
SRC_TEMP_REG_A,//=3,
SRC_TEMP_REG_B,//=4,
SRC_TEMP_REG_C,//=5,
SRC_OPERATION1_OUTPUT1,//=6,
SRC_OPERATION2_OUTPUT1//=7,
};

enum ShiftCond_Enum{
NEVER_SHIFT,
ALWAYS_SHIFT,
IF_EQUAL_SHIFT,
IF_NOT_EQUAL_SHIFT,
IF_LESS_SHIFT,
IF_GREAT_OR_EQUAL_SHIFT, //TODO: increase the shift condition to support IF_GREAT_SHIFT
IF_LESS_OR_EQUAL_SHIFT,
IF_REPEAT_ENDS_SHIFT,
};

enum ShiftDir_Enum{
READ, //=1'b0
WRITE //=1'b1
};

enum OutSRC_Enum{
OUT_SRC_OPERATION1_OUT,//=0
OUT_SRC_OPERATION2_OUT,//=1
OUT_SRC_1,//=2
OUT_SRC_1_OR_2_IF_SHIFTED//=3 //TODO: add this to the RTL, use the value that is shifted , if both use SRC1
};

typedef struct inst_format_struct{
//int pc;		// NOTE : This has been deleted since PC will be held in Control Unit and is not part of the instruction buffer
int nextPC1;
int nextPc2;
NextPC_Cond_Enum nextPC_Cond; //(when pc1=pc2=> reserve for operation)
OP_CODE_Enum opCode1;
OP_CODE_Enum opCode2;
SRC_OP_Enum src1Op1;
SRC_OP_Enum src2Op1;
SRC_OP_Enum src1Op2;
SRC_OP_Enum src2Op2;
ShiftCond_Enum shiftCond1;
ShiftCond_Enum shiftCond2;
ShiftCond_Enum shiftCond3;
ShiftDir_Enum shiftDir1;
ShiftDir_Enum shiftDir2;
ShiftDir_Enum shiftDir3;
int repeat;
OutSRC_Enum outSRC;
} instruction;
