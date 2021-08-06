#ifndef MEMORY_ARRAY_HPP_
#define MEMORY_ARRAY_HPP_
//TODO: seperate .hpp file and .cpp file for this class

//TODO: clean up all the unused variables and preprocessors
//TODO: cleanup all useless commneted lines
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "debug.hpp"
#include "physicalComponent.hpp"
#include "configAndStats.hpp"
#include "types.hpp"

using namespace std;


//#define SIZE_OF_SUB_ARRAY_IN_BITS	(32 * 64 * 4096) // 32 bit word * 64 col * 4096 columns
//#define SIZE_OF_SUB_ARRAY		SIZE_OF_SUB_ARRAY_IN_BITS / 8 	// <---- Size of Sub Array in bytes
									// ALways use this macro since C always addresses minimum of sizeof(unsigned char) at a time and doesnt allow bit level addressing
//#define NUMBER_OF_GB	8

//#define 	MAX_ADDRESS	 (uint64_t)NUMBER_OF_GB * 1024 * 1024 * 1024


class memoryArray:public physicalComponent {
	public: 

		LOCAL_ADDRESS_TYPE arraySize;
		memoryArray(ID_TYPE l_id, configAndStats * l_confObj, physicalComponent * l_firstDimOwner, physicalComponent * l_secondDimOwner, physicalComponent * l_thirdDimOwner)
	       :physicalComponent(l_id, l_confObj, l_firstDimOwner, l_secondDimOwner, l_thirdDimOwner){
			debug_printf("Im in the memory_pool_obj constructor\n");
			arraySize=l_confObj->getConfig<CONF_SIZE_OF_EACH_SUBARRAY_IN_BYTE_TYPE>(CONF_SIZE_OF_EACH_SUBARRAY_IN_BYTE_NAME);
			debug_printf("Size of memory getting initialized = %u\n",arraySize );
			memory_pool = (READ_DATA_TYPE_IN_MEMORY_ARRAY*) calloc(arraySize, sizeof(READ_DATA_TYPE_IN_MEMORY_ARRAY));
		}
		
		~memoryArray(void) {
			debug_printf("Im in the destructor\n");
			free(memory_pool);
			memory_pool = NULL;
		}

	ERROR_RETURN_TYPE is_address_translation_correct(LOCAL_ADDRESS_TYPE memory_address) {
		if(memory_address > arraySize) {
			cout << "Value of read_memory_address="<<memory_address << " MAX_ADDRESS=" << arraySize << endl;
			return PP_ERROR;
		}
		else
			return PP_SUCCESS;
	}
	//TODO: check compatibilty of uint64_t with PHYSICAL_EXTERNAL_ADDRESS_TYPE in types.hpp
	ERROR_RETURN_TYPE memory_read(LOCAL_ADDRESS_TYPE memory_address, LOCAL_ADDRESS_TYPE read_size, READ_DATA_TYPE_IN_MEMORY_ARRAY *read_data)
	{
		debug_printf("Point 1\n");
		debug_printf("memory_pool_obj:	memory_address being read = %u \n", memory_address);

		// Check for out of bounds condition
		if(PP_ERROR == is_address_translation_correct(memory_address+read_size))
			return PP_ERROR;
		
		debug_printf("Point 2\n");

		if(memory_address+read_size > arraySize)
			return PP_ERROR;
	
		debug_printf("Point 3\n");
		/*
				//replace by Marzieh  with memcopy for better performance
		for(uint64_t increment=0; increment<read_size; increment++) {
			debug_printf("Address %ld = %d \n", increment, *(memory_pool + memory_address + increment));
			//*read_data = *(memory_pool + memory_address + increment);
			//read_data++;
		}
		*/
		memcpy ( read_data, memory_pool + memory_address, read_size);
		debug_printf("Point 4\n");

		return PP_SUCCESS;
	}
	ERROR_RETURN_TYPE printData(LOCAL_ADDRESS_TYPE memory_address, LOCAL_ADDRESS_TYPE read_size)
	{
		debug_printf("Point 1\n");
		debug_printf("memory_pool_obj:	memory_address being read = %u \n", memory_address);

		// Check for out of bounds condition
		if(PP_ERROR == is_address_translation_correct(memory_address+read_size))
			return PP_ERROR;

		debug_printf("Point 2\n");

		if(memory_address+read_size > arraySize)
			return PP_ERROR;

		debug_printf("Point 3\n");
		std::cout<<"_________printing memory array conents in addresss:"<<memory_address <<std::endl;
       for(LOCAL_ADDRESS_TYPE increment=0; increment<read_size; increment++) {
			READ_DATA_TYPE_IN_MEMORY_ARRAY tRead=*(memory_pool + memory_address + increment);

			printf("%d , ", tRead);

			debug_printf("Address %u = %d \n", increment, *(memory_pool + memory_address + increment));
			//*read_data = ;
			//read_data++;
		}
		std::cout<<std::endl;


		debug_printf("Point 4\n");

		return PP_SUCCESS;
	}
	
	ERROR_RETURN_TYPE memory_write(LOCAL_ADDRESS_TYPE memory_address, LOCAL_ADDRESS_TYPE write_size, READ_DATA_TYPE_IN_MEMORY_ARRAY *write_data)
	{
		debug_printf("Point 1\n");

		// Check for out of bounds condition
		if(PP_ERROR == is_address_translation_correct(memory_address+write_size))
			return PP_ERROR;
		
		debug_printf("Point 2\n");

		if(memory_address+write_size > arraySize)
			return PP_ERROR;
	
		debug_printf("Point 3\n");
		/*
		//replace by Marzieh  with memcopy for better performance
		for(uint64_t increment=0; increment<read_size; increment++) {
			debug_printf("Address %ld = %d \n", increment, *(memory_pool + memory_address + increment));
			*(memory_pool + memory_address + increment) = *read_data;
			read_data++;
		}*/

		memcpy (  memory_pool + memory_address, write_data,write_size);
		debug_printf("Point 4\n");

		return PP_SUCCESS;

	}

	private:
	unsigned char *memory_pool; 	
	
};
#endif
