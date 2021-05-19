#include <iostream>
#include "instruction.hpp"

#define INSTRUCTION_BUFFER_LENGTH	8
using namespace std;

#define 	PP_SUC		1
#define 	PP_ERR		0
class instruction_buffer{
	public:
		// read a member of the instruction buffer
		unsigned char read_from_instr_buffer(unsigned char instr_num, instruction *read_from_buffer) {
			if(instr_num < INSTRUCTION_BUFFER_LENGTH) {
				*read_from_buffer = instr_buff[instr_num];
				return PP_SUC;
			}
			else
				return PP_ERR;
		}

		unsigned char write_into_instr_buffer(unsigned char instr_num,instruction new_instr) {
			if(instr_num >= INSTRUCTION_BUFFER_LENGTH)
				return PP_ERR;

			instr_buff[instr_num] = new_instr;
			return PP_SUC;
		}

	private: 
		instruction instr_buff[INSTRUCTION_BUFFER_LENGTH]; 
};

/*
int main(void) {
	instruction		sample_instruction, read_back_instr;
	instruction_buffer	fulcrum_instruction_buffer;

	sample_instruction.nextPC1=1; //
	sample_instruction.nextPc2=2;
	sample_instruction.nextPC_Cond=IF_EQUAL_NEXT2; //(when pc1=pc2=> reserve for operation)
	sample_instruction.opCode1=COMPARE_OPCODE;
	sample_instruction.opCode2=NULL_OPCODE_KEEP_OUTPUT;
	sample_instruction.src1Op1=SRC_ROW1;
	sample_instruction.src2Op1=SRC_ROW2;
	sample_instruction.src1Op2=SRC_ROW1;
	sample_instruction.src2Op2=SRC_ROW2;
	sample_instruction.shiftCond1=IF_LESS_OR_EQUAL_SHIFT;
	sample_instruction.shiftCond2=IF_GREAT_OR_EQUAL_SHIFT;
	sample_instruction.shiftCond3=ALWAYS_SHIFT;
	sample_instruction.shiftDir1=READ;
	sample_instruction.shiftDir2=READ;
	sample_instruction.shiftDir3=WRITE;
	sample_instruction.repeat=3;
	sample_instruction.outSRC=OUT_SRC_1_OR_2_IF_SHIFTED;

	fulcrum_instruction_buffer.write_into_instr_buffer(0, sample_instruction);

	fulcrum_instruction_buffer.read_from_instr_buffer(0, &read_back_instr);
	cout << "Instruction read back ad SHIFT DIR-1 = " << sample_instruction.shiftDir3 << endl;
	return 1;

}
*/
