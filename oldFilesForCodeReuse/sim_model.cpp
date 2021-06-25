#include <stdio.h>
#include <iostream>
#include "instruction_buffer.hpp"
#include "ALU.hpp"
#include "temp_registers.hpp"
#include "walkers.hpp"
#include "debug.hpp"

#include <omp.h>
#include "memoryArray.hpp"

#define READ_SIZE	16
using namespace std;


class sim_model {
	public:
		void model_setup(void) {
			for(int i=0; i<NUM_OF_COLUMNS; i++) {
				columnar_data[i] = i;
			}

			walker1.writeColumnData(columnar_data);
	         	walker2.writeColumnData(columnar_data);

			for(int i=0; i<6 ; i++) {
			current_instruction.nextPC1=1; 
			current_instruction.nextPc2=2;
			current_instruction.nextPC_Cond=IF_EQUAL_NEXT2; //(when pc1=pc2=> reserve for operation)
			current_instruction.opCode1=(OP_CODE_Enum)(rand() % 6);
			current_instruction.opCode2=(OP_CODE_Enum)(rand() % 6);
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

			fulcrum_instruction_buffer.write_into_instr_buffer(i, current_instruction);	
			}

		}

		// arg 1 --> col_counter_1
		// arg 2 --> col_counter_2

		void execute(unsigned int num_1, unsigned int num_2) { 

			//srand(time(0));
			col_counter[0] = num_1;
			col_counter[1] = num_2;
			col_counter[2] = rand() % 64;

			debug_printf("walker-1 column %d and walker-2 column %d selected\n", col_counter[0], col_counter[1]);

			wait_counter = 9;

			while(i < 6) {	// i < INSTRUCTION_BUFFER_LENGTH
			// NextPC1 and NextPC2
	
			/*
			if(i%2 == 0) // If its an even numbered instruction then perform a memory read // TODO delete this later
			{
				uint64_t	probe_address = rand() % MAX_ADDRESS;
				debug_printf("probe_address = %ld \n", probe_address);
				memory_obj->memory_read((uint64_t)probe_address, 8, read_data);
			}	
			*/

			// retrieve the i-th instruction from the instruction buffer
			debug_printf("\t---------- Iteration %d --------\n", i);

			fulcrum_instruction_buffer.read_from_instr_buffer(i, &current_instruction);

				switch (current_instruction.nextPC_Cond) {
					case ALWAYS_NEXT:
						//next_pc = i++;
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

				col_counter[0]++;
				col_counter[1]++;

				//loaded the address of the next instruction
				//i=next_pc;	// TODO change this
				i++;
			}
			//free(memory_obj);
		}

		// TODO Perform a check if the ID is already taken

        /***** COMMENTED BY MARZIEH DUE TO A COMPILE ERROR
		// This function is the translation layer. It translates the subarray address (~1mb) into common pool address (~8gb)
		// Suppose subarray-3 wants to reads 512th kb, then this function translates that into common pool address which would be to read 3.5th MB, memory address within the common pool
		char perform_sub_array_memory_access(memory_pool_obj* memory_pool, uint64_t address, uint64_t read_size, unsigned char *read_data) {
			uint64_t	pool_address = (static_cast<uint64_t>(ALPU_ID)*SIZE_OF_SUB_ARRAY) + static_cast<uint64_t>(address);
			debug_printf("sim_model: address=%ld read_size=%ld ALPU_ID=%d SIZE_OF_SUB_ARRAY=%d\n", address, read_size, ALPU_ID, SIZE_OF_SUB_ARRAY);
			debug_printf("sim_model: The pool address being probed = %ld\n", pool_address);
			//cout << "sim_model: address="<< address <<" read_size=" << read_size << " ALPU_ID= " << ALPU_ID << " SIZE_OF_SUB_ARRAY= "<< SIZE_OF_SUB_ARRAY << endl;
			//cout << "sim_model: The pool address being probed = "<< pool_address << endl;

			return memory_pool->memory_read(pool_address, READ_SIZE, read_data);
		}
		*/
		void set_ALPU_ID (unsigned int id_num) {
			ALPU_ID = id_num;
		}
	private:	
			unsigned int columnar_data[NUM_OF_COLUMNS];
			instruction		current_instruction, read_back_instr;
			instruction_buffer	fulcrum_instruction_buffer;
			walker 			walker1, walker2, walker3;
			temp_registers		fulcrum_registers;
			ALU			fulcrum_alu;

			unsigned int inputA, inputB;
			unsigned short row_counter[3];
			unsigned short col_counter[3];
			unsigned short wait_counter;

			unsigned char next_pc;
			unsigned char i=0;  
			unsigned int op1_output = 0;
			unsigned int op2_output = 0;
			unsigned int ALPU_ID = 0;
};

#define NUM_SIMULATIONS	6500
#ifdef USE_MAIN_IN_SIM_MODE

int main()
{
	sim_model	sim[NUM_SIMULATIONS];
	memory_pool_obj *memory_4_gb_pool = new memory_pool_obj();
	
	srand(time(0));

	for(int j=0; j<NUM_SIMULATIONS; j++) {
		sim[j].model_setup();
		sim[j].set_ALPU_ID(j);
	}

	unsigned char *read_data = (unsigned char*) calloc(READ_SIZE, sizeof(unsigned char));
	uint64_t	probe_address;
	//omp_set_num_threads(15);
#pragma omp parallel
#pragma omp loop
	for(int j=0; j<NUM_SIMULATIONS; j++)
	{
		debug_printf(" ********* Start of Simulation %d *********** \n", j);
		sim[j].execute((rand()%55), (rand()%55)); 

			{
				probe_address = rand() % SIZE_OF_SUB_ARRAY; 
				sim[j].perform_sub_array_memory_access(memory_4_gb_pool, probe_address, READ_SIZE, read_data);
			}	

	}
	//char ch=getc(stdin);
	delete memory_4_gb_pool;
	//ch = getc(stdin);

	return 0;
}
#endif
