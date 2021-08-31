#pragma once

#include <cstdint>
#include "types.hpp"
#include "EventQueue.hpp"
#include <string>
#include <iostream>

// Configuration
//#define G_NUM_BINS					16
//#define G_LOG_NUM_BINS				4


#define G_NUM_WORDS_IN_ROW			64
#define G_NUM_BYTES_IN_ROW			(G_NUM_WORDS_IN_ROW * sizeof(FULCRU_WORD_TYPE))

#define G_RADIX_BITS				11
#define G_KEY_BITS					(sizeof(KEY_TYPE) * 8)
//#define G_KEY_BITS					32UL
#define G_NUM_HIST_ELEMS			256


// Timings for modeling
#define G_LOCAL_SORT_INIT_CYCLES					8
#define G_LOCAL_SORT_CYCLES_PER_ELEM_PER_BIT		1
#define G_LOCAL_HIST_RESET_CYCLES_PER_ELEM			1
#define G_REDUCTION_PER_HIST_ELEM_CYCLES			1
#define G_OFFSET_PER_HIST_ELEM_CYCLES				1


//#define G_ROW_READ_LATENCY		3
//#define G_ROW_WRITE_LATENCY		4
//#define G_ROW_RMW_LATENCY		5

#define G_ROW_OPEN_LATENCY				3
#define G_ROW_CLOSE_LATENCY				2
//#define G_ADDITIONAL_LATENCY_READ		1
//#define G_ADDITIONAL_LATENCY_WRITE		1

#define OPEN_PAGE_POLICY		0
#define CLOSE_PAGE_POLICY		1
#define PAGE_POLICY				OPEN_PAGE_POLICY


// Metadata description (all within the first row, and latched to a walker)
#define G_ADDR_OF_HIST_START_ADDR			0
#define G_ADDR_OF_HIST_END_ADDR				4
#define G_ADDR_OF_READ_START_ADDR			8
#define G_ADDR_OF_READ_END_ADDR				12
#define G_ADDR_OF_WRITE_START_ADDR			16
#define G_ADDR_OF_WRITE_END_ADDR			20
//#define G_ADDR_OF_TEMP_READ_ADDR			24


#define PLACEMENT_QUEUE_HIGH_WATER_MARK		7
#define PLACEMENT_QUEUE_LOW_WATER_MARK		2


// Global objects
//extern EventQueue eventQueue
extern u64 simCycles;
extern u64 radixSortMask;
extern u64 radixSortShift;

extern KEY_TYPE rangeEnd;
extern u64 numOfProcessedSubarrays;
extern u64 numOfInFlightPackets;
extern u64 lastIdx;
extern u64 reservedBytesForReadWriteArray;

extern LOCAL_ADDRESS_TYPE histStartAddr;
extern LOCAL_ADDRESS_TYPE histEndAddr;
extern KEY_TYPE* dataArray;


static u64 getNextPow2(u64 val, u64 minVal) {
	if(val <= minVal){
		return minVal;
	}
	u64 res = 1UL << (63 - __builtin_clzl(val));
	if(res < val){
		res = res << 1;
	}
	return res;
}

static u64 getNextPow2(u64 val){
	return getNextPow2(val, 2);
}

static void printSimCycle(const std::string& msg = ""){
	std::cout << "[CYCLE " << simCycles << "] " << msg << std::endl;
}

static bool radixComp(KEY_TYPE a, KEY_TYPE b){
	a = (a & radixSortMask) >> radixSortShift;
	b = (b & radixSortMask) >> radixSortShift;
	return a < b;
}




