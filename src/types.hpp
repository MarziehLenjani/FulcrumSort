#ifndef TYPES_HPP
#define TYPES_HPP


#include<map>
#include <cstdint>
#include <boost/assign/list_of.hpp>
#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <list>
#include <cstddef>
#include <functional>
#include <vector>

using namespace boost::assign;
//-------------------------------------Akhi's types
#define ERROR_RETURN_TYPE char
// Error Codes
#define     PP_SUCCESS                       (ERROR_RETURN_TYPE) 00      // Use this ret code for successful execution
#define     PP_ERROR                          (ERROR_RETURN_TYPE)  1      // Use this error code when the error is undefined

#define     PP_OUT_OF_BOUNDS_ERROR          100



//------------------------------------Marzieh's Types

//#define PHYSICAL_EXTERNAL_ADDRESS_TYPE_UNSIGHED_LONG_INT
#ifdef PHYSICAL_EXTERNAL_ADDRESS_TYPE_UNSIGHED_LONG_INT

#define PHYSICAL_EXTERNAL_ADDRESS_TYPE unsigned long int
#else
#define PHYSICAL_EXTERNAL_ADDRESS_TYPE_UNSIGHED_INT
#define PHYSICAL_EXTERNAL_ADDRESS_TYPE unsigned  int
#endif


#define FULCRUM_32_BIT

#ifdef FULCRUM_32_BIT
typedef unsigned char READ_DATA_TYPE_IN_MEMORY_ARRAY;
typedef unsigned int FULCRU_WORD_TYPE;
typedef unsigned int ID_TYPE;
typedef unsigned int LOCAL_ADDRESS_TYPE;
#endif


typedef uint64_t HIST_ELEM_TYPE;
typedef uint32_t KEY_TYPE;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int64_t i64;

//typedef u32		HIST_COUNT_TYPE;
//
//typedef struct {
//	HIST_COUNT_TYPE cnt = 0;
//	HIST_COUNT_TYPE used = 0;
//} Histogram;

//----------------------------------------------

//-----------------------------------------------

template <typename T1, typename T2> T2 castNoLoss(T1 tmpUnsign){
	T2 newT;
	memcpy(&newT, &tmpUnsign, sizeof(T2));
	return newT;
};

typedef struct{
	KEY_TYPE key;
	LOCAL_ADDRESS_TYPE offset;		//address within subarray
} PlacementPacket;


#endif
