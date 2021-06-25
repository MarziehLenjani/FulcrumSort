#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include "debug.hpp"
#include "memoryArray.hpp"

using namespace std;

/*
#define PP_SUC	0
#define PP_ERR	-1
#define SIZE_OF_SUB_ARRAY	(32 * 64 * 4096) // 32 bit world * 64 col * 4096 columns
#define NUMBER_OF_GB	4

#define 	MAX_ADDRESS	 (uint64_t)NUMBER_OF_GB * 1024 * 1024 * 1024

class memory_pool_obj {
	public: 

		memory_pool_obj(void) {
			debug_printf("Im in the constructor\n");
			memory_pool = (unsigned char*) calloc(MAX_ADDRESS, sizeof(unsigned char));
		}

	char is_address_translation_correct(unsigned long memory_address) {
		if(memory_address > MAX_ADDRESS)
			return PP_ERR;
		else
			return PP_SUC;
	}
	char memory_read(uint64_t memory_address, unsigned int read_size, unsigned char *read_data)
	{
		debug_printf("Point 1\n");

		// Check for out of bounds condition
		if(PP_ERR == is_address_translation_correct(memory_address+read_size))
			return PP_ERR;
		
		debug_printf("Point 2\n");

		if(memory_address+read_size > MAX_ADDRESS)
			return PP_ERR;
	
		debug_printf("Point 3\n");
	
		for(uint64_t increment=0; increment<read_size; increment++) {
			debug_printf("Address %ld = %d \n", increment, *(memory_pool + memory_address + increment));
			*read_data = *(memory_pool + memory_address + increment);
			read_data++;
		}
		debug_printf("Point 4\n");

		return PP_SUC;
	}
	
	char memory_write(uint64_t memory_address, unsigned int read_size, unsigned char *read_data)
	{
		debug_printf("Point 1\n");

		// Check for out of bounds condition
		if(PP_ERR == is_address_translation_correct(memory_address+read_size))
			return PP_ERR;
		
		debug_printf("Point 2\n");

		if(memory_address+read_size > MAX_ADDRESS)
			return PP_ERR;
	
		debug_printf("Point 3\n");
	
		for(uint64_t increment=0; increment<read_size; increment++) {
			debug_printf("Address %ld = %d \n", increment, *(memory_pool + memory_address + increment));
			*(memory_pool + memory_address + increment) = *read_data;
			read_data++;
		}
		debug_printf("Point 4\n");

		return PP_SUC;

	}

	private:
	unsigned char *memory_pool; 	
	
};*/
#ifdef USE_MAIN_IN_COMMON_MEMORY_POOL
#define		READ_SIZE		16
int main(void) {
	unsigned char 	*read_data_set;
	memory_pool_obj *memory_4gb_pool= new memory_pool_obj;
	unsigned char *read_array = (unsigned char*) calloc(READ_SIZE, sizeof(unsigned char));

	srand(time(0));
	uint64_t	probe_address = rand() % MAX_ADDRESS;
	char ch = getc(stdin);

	debug_printf("Value of Probe Address = %ld \n", probe_address);
	memory_4gb_pool->memory_read(probe_address, READ_SIZE, read_array); /*, read_data_set);
	for(int i=0;i<8;i++)
	{
		debug_printf("read data %d = %d \n", i, *(read_data_set+i));
		*(read_data_set+i) = i;
	}
	
	memory_4gb_pool->memory_write(probe_address, 8, read_data_set);
	
	memory_4gb_pool->memory_read(probe_address, 8, read_data_set);
	for(int i=0;i<8;i++)
	{
		debug_printf("read data %d = %d \n", i, *(read_data_set+i));
	}*/
	ch = getc(stdin);

	delete(memory_4gb_pool);
	debug_printf("Point 5\n");
	ch = getc(stdin);
	return 0;	
}
#endif
