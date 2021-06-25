#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "debug.hpp"

using namespace std;

#define PP_ERROR	-1
#define PP_SUCCESS	0

#define SIZE_OF_SUB_ARRAY	(32 * 64 * 4096) // 32 bit world * 64 col * 4096 columns
#define NUMBER_OF_GB	4

#define 	MAX_ADDRESS	 (uint64_t)NUMBER_OF_GB * 1024 * 1024 * 1024

class memory_pool_obj {
	public: 

		memory_pool_obj(void) {
			debug_printf("Im in the constructor\n");
			memory_pool = (unsigned char*) calloc(MAX_ADDRESS, sizeof(unsigned char));
		}
		/*
		~memory_pool_obj(void) {
			debug_printf("Im in the destructor\n");
			free(memory_pool);
		}*/

	char is_address_translation_correct(unsigned long memory_address) {
		if(memory_address > MAX_ADDRESS)
			return PP_ERROR;
		else
			return PP_SUCCESS;
	}
	char memory_read(uint64_t memory_address, unsigned int read_size, unsigned char *read_data)
	{
		debug_printf("Point 1\n");

		// Check for out of bounds condition
		if(PP_ERROR == is_address_translation_correct(memory_address+read_size))
			return PP_ERROR;
		
		debug_printf("Point 2\n");

		if(memory_address+read_size > MAX_ADDRESS)
			return PP_ERROR;
	
		debug_printf("Point 3\n");
	
		for(uint64_t increment=0; increment<read_size; increment++) {
			debug_printf("Address %ld = %d \n", increment, *(memory_pool + memory_address + increment));
			*read_data = *(memory_pool + memory_address + increment);
			read_data++;
		}
		debug_printf("Point 4\n");

		return PP_SUCCESS;
	}
	
	char memory_write(uint64_t memory_address, unsigned int read_size, unsigned char *read_data)
	{
		debug_printf("Point 1\n");

		// Check for out of bounds condition
		if(PP_ERROR == is_address_translation_correct(memory_address+read_size))
			return PP_ERROR;
		
		debug_printf("Point 2\n");

		if(memory_address+read_size > MAX_ADDRESS)
			return PP_ERROR;
	
		debug_printf("Point 3\n");
	
		for(uint64_t increment=0; increment<read_size; increment++) {
			debug_printf("Address %ld = %d \n", increment, *(memory_pool + memory_address + increment));
			*(memory_pool + memory_address + increment) = *read_data;
			read_data++;
		}
		debug_printf("Point 4\n");

		return PP_SUCCESS;

	}

	private:
	unsigned char *memory_pool; 	
	
};

