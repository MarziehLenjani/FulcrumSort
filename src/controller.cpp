#include <iostream>
#include <stdlib.h>

#include "instruction_buffer.hpp"
#include "walkers.hpp"
#include "temp_registers.hpp"
#include "ALU.hpp"
#include "debug.hpp"

using namespace std;

/*
class controller{
	// Counter Operations
	controller(void) {
		row_counter = 0;
		col_counter = 0;
		wait_counter = 0;
	}

	// initialize counter
	void init_counter(char str[], unsigned short value) {
		if(strcmpi(str, "ROW") == 0)
			row_counter = value;

		if(strcmpi(str, "COL") == 0)
			col_counter = value;

		if(strcmpi(str == "WAIT") == 0)
			wait_counter = 0;
	}

	void decrement_counters(char str[]) {
		if(strcmpi(str, "ROW") == 0)
			row_counter--;
		
		if(strcmpi(str, "ROW") == 0)
			col_counter--;

		if(strcmpi(str, "ROW") == 0)
			wait_counter--;
	}

	unsigned short read_current_counter_values(char str[]) {
		if(strcmpi(str, "ROW") == 0)
			return row_counter;

		if(strcmpi(str, "ROW") == 0)
			return col_counter;

		if(strcmpi(str, "ROW") == 0)
			return wait_counter;
	}


	unsigned char return
	private:
		unsigned short row_counter;
		unsigned short col_counter;
		unsigned short wait_counter;
};
*/

#ifdef USE_MAIN_IN_CONTROLLER
int main(void) {
	instruction		current_instruction, read_back_instr;
	instruction_buffer	fulcrum_instruction_buffer;
	walker 			walker1, walker2, walker3;
	temp_registers		fulcrum_registers;
	ALU			fulcrum_alu;

	unsigned int inputA, inputB;
	unsigned short row_counter[3];
	unsigned short col_counter[3];
	unsigned short wait_counter;

	unsigned int columnar_data[NUM_OF_COLUMNS];
	for(int i=0; i<NUM_OF_COLUMNS; i++) {
		columnar_data[i] = i;
	}
	current_instruction.nextPC1=1; //
	current_instruction.nextPc2=2;
	current_instruction.nextPC_Cond=IF_EQUAL_NEXT2; //(when pc1=pc2=> reserve for operation)
	current_instruction.opCode1=ADD_OPCODE;
	current_instruction.opCode2=XOR_OPCODE;
	current_instruction.src1Op1=SRC_ROW1;
	current_instruction.src2Op1=SRC_ROW2;
	current_instruction.src1Op2=SRC_ROW1;
	current_instruction.src2Op2=SRC_ROW2;
	current_instruction.shiftCond1=IF_LESS_OR_EQUAL_SHIFT;
	current_instruction.shiftCond2=IF_GREAT_OR_EQUAL_SHIFT;
	current_instruction.shiftCond3=ALWAYS_SHIFT;
	current_instruction.shiftDir1=READ;
	current_instruction.shiftDir2=READ;
	current_instruction.shiftDir3=WRITE;
	current_instruction.repeat=3;
	current_instruction.outSRC=OUT_SRC_1_OR_2_IF_SHIFTED;

	// initializing the counters
	row_counter[0] = 50;
	row_counter[1] = 100;
	row_counter[2] = 150;

	srand(time(0));
	col_counter[0] = rand() % 64;
	col_counter[1] = rand() % 64;
	col_counter[2] = rand() % 64;

	debug_printf("walker-1 column %d and walker-2 column %d selected\n", col_counter[0], col_counter[1]);

	wait_counter = 9;

	fulcrum_instruction_buffer.write_into_instr_buffer(0, current_instruction);

	// TODO Right now while testing, Im just writing 5 elements into walker 1 and walker 2 ... Change this and remove the following statement
	walker1.writeColumnData(columnar_data);
	walker2.writeColumnData(columnar_data);
	
	unsigned char next_pc;
	unsigned char i=0;  
	unsigned int op1_output = 0;
	unsigned int op2_output = 0;

	while(i < 1) {	// i < INSTRUCTION_BUFFER_LENGTH
		// NextPC1 and NextPC2
		
		// retrieve the i-th instruction from the instruction buffer
		fulcrum_instruction_buffer.read_from_instr_buffer(i, &current_instruction);

		switch (current_instruction.nextPC_Cond) {
			case ALWAYS_NEXT:
				next_pc = i++;
				break;
			case IF_EQUAL_NEXT2:
			case IF_LESS_NEXT2:
			case IF_GREAT_NEXT2:
			case IF_REPEAT_ENDS_NEXT2:
			default:	break;			// TODO fill this
		}
		
		switch(current_instruction.src1Op1) {
			case SRC_ROW1:	inputA = walker1.read_columnData(col_counter[0]);
					break;
			case SRC_ROW2:	inputA = walker2.read_columnData(col_counter[1]);
					break;
			case SRC_GLLOBAL_BUS:
			case SRC_TEMP_REG_A: 	inputA = fulcrum_registers.read_reg(0);
						break;
			case SRC_TEMP_REG_B:	inputA = fulcrum_registers.read_reg(1);
						break;
			case SRC_TEMP_REG_C:	inputA = fulcrum_registers.read_reg(2);
						break;
			case SRC_OPERATION1_OUTPUT1:	inputA = op1_output;
							break;
			case SRC_OPERATION2_OUTPUT1:	inputA = op2_output;
							break;
		}

		switch(current_instruction.src2Op1) {
			case SRC_ROW1:	inputB = walker1.read_columnData(col_counter[0]);
					break;
			case SRC_ROW2:	inputB = walker2.read_columnData(col_counter[1]);
					break;
			case SRC_GLLOBAL_BUS:
			case SRC_TEMP_REG_A: 	inputB = fulcrum_registers.read_reg(0);
						break;
			case SRC_TEMP_REG_B:	inputB = fulcrum_registers.read_reg(1);
						break;
			case SRC_TEMP_REG_C:	inputB = fulcrum_registers.read_reg(2);
						break;
			case SRC_OPERATION1_OUTPUT1:	inputB = op1_output;
							break;
			case SRC_OPERATION2_OUTPUT1:	inputB = op2_output;
							break;
		}


		switch(current_instruction.opCode1) {
			case ADD_OPCODE:	op1_output = fulcrum_alu.add(inputA, inputB);
						break;
			case MULT_OPCODE:	op1_output = fulcrum_alu.multiply(inputA, inputB);
						break;
			case XOR_OPCODE:	op1_output = fulcrum_alu.bitwise_xor(inputA, inputB);
						break;
			case NOR_OPCODE:	op1_output = fulcrum_alu.bitwise_nor(inputA, inputB);
						break;
			case AND_OPCODE:	op1_output = fulcrum_alu.bitwise_and(inputA, inputB);
						break;
			case OR_OPCODE:		op1_output = fulcrum_alu.bitwise_or(inputA, inputB);
						break;
			case SHIFT_OPCODE:
			default : 		op1_output = 0;
		}

		// Operation 2 or second opcode is being executed starting from here
		switch(current_instruction.src1Op2) {
			case SRC_ROW1:	inputA = walker1.read_columnData(col_counter[0]);
					break;
			case SRC_ROW2:	inputA = walker2.read_columnData(col_counter[1]);
					break;
			case SRC_GLLOBAL_BUS:
			case SRC_TEMP_REG_A: 	inputA = fulcrum_registers.read_reg(0);
						break;
			case SRC_TEMP_REG_B:	inputA = fulcrum_registers.read_reg(1);
						break;
			case SRC_TEMP_REG_C:	inputA = fulcrum_registers.read_reg(2);
						break;
			case SRC_OPERATION1_OUTPUT1:	inputA = op1_output;
							break;
			case SRC_OPERATION2_OUTPUT1:	inputA = op2_output;
							break;
		}

		switch(current_instruction.src2Op2) {
			case SRC_ROW1:	inputB = walker1.read_columnData(col_counter[0]);
					break;
			case SRC_ROW2:	inputB = walker2.read_columnData(col_counter[1]);
					break;
			case SRC_GLLOBAL_BUS:
			case SRC_TEMP_REG_A: 	inputB = fulcrum_registers.read_reg(0);
						break;
			case SRC_TEMP_REG_B:	inputB = fulcrum_registers.read_reg(1);
						break;
			case SRC_TEMP_REG_C:	inputB = fulcrum_registers.read_reg(2);
						break;
			case SRC_OPERATION1_OUTPUT1:	inputB = op1_output;
							break;
			case SRC_OPERATION2_OUTPUT1:	inputB = op2_output;
							break;
		}


		switch(current_instruction.opCode2) {
			case ADD_OPCODE:	op2_output = fulcrum_alu.add(inputA, inputB);
						break;
			case MULT_OPCODE:	op2_output = fulcrum_alu.multiply(inputA, inputB);
						break;
			case XOR_OPCODE:	op2_output = fulcrum_alu.bitwise_xor(inputA, inputB);
						break;
			case NOR_OPCODE:	op2_output = fulcrum_alu.bitwise_nor(inputA, inputB);
						break;
			case AND_OPCODE:	op2_output = fulcrum_alu.bitwise_and(inputA, inputB);
						break;
			case OR_OPCODE:		op2_output = fulcrum_alu.bitwise_or(inputA, inputB);
						break;
			case SHIFT_OPCODE:
			default : 		op2_output = 0;
		}

		debug_printf("inputA=%d inputB=%d \n", inputA, inputB);
		debug_printf("op1_output = %d \n", op1_output);
		debug_printf("op2_output = %d \n", op2_output);

		//loaded the address of the next instruction
		//i=next_pc;	// TODO change this
		i++;
	}
}
#endif
