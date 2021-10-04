#pragma once

#include <cstdint>
#include "types.hpp"
#include <string>
#include <iostream>

// Configuration
#define G_NUM_DEVICES				1
#define G_NUM_STACKS_PER_DEVICE		1
#define G_NUM_LAYERS_PER_STACK		8
#define G_NUM_BANKS_PER_LAYER		64
#define G_NUM_SUBARRAY_PER_BANK		16
#define G_SIZE_OF_SUBARRAY_IN_BYTE	(1024*1024UL)
#define G_NUM_BYTES_IN_ROW			256

#define G_NUM_TOTAL_STACKS			(G_NUM_DEVICES * G_NUM_STACKS_PER_DEVICE)
#define G_NUM_TOTAL_LAYERS			(G_NUM_TOTAL_STACKS * G_NUM_LAYERS_PER_STACK)
#define G_NUM_TOTAL_BANKS			(G_NUM_TOTAL_LAYERS * G_NUM_BANKS_PER_LAYER)
#define G_NUM_TOTAL_SUBARRAY		(G_NUM_TOTAL_BANKS * G_NUM_SUBARRAY_PER_BANK)

//#define G_NUM_OF_DATA_ELEMENTS		(G_NUM_TOTAL_SUBARRAY * 1024)


#define G_RADIX_BITS				16
#define G_KEY_BITS					(sizeof(KEY_TYPE) * 8)
#define G_NUM_HIST_ELEMS			(1UL << G_RADIX_BITS)

//use bank level histogram
//#if (G_NUM_HIST_ELEMS > 8192)
#define G_BANK_LEVEL_HISTOGRAM
//#endif

// Timings for modeling
//#define G_LOCAL_SORT_INIT_CYCLES					8
#define G_LOCAL_SORT_CYCLES_PER_ELEM_PER_BIT		1
#define G_LOCAL_HIST_RESET_CYCLES_PER_ELEM			1
#define G_LOCAL_HIST_CYCLES_PER_ELEM				3
#define G_REDUCTION_PER_HIST_ELEM_CYCLES			1
#define G_OFFSET_PER_HIST_ELEM_CYCLES				1

#define G_INTCNT_CLOCK_PERIOD_NS					1.0
#define G_LOGIC_CLOCK_PERIOD_NS						1.6

#define G_HMC_LINK_EFFICIENCY						0.8
#define G_HMC_LINK_PAYLOAD_BANDWIDTH_GB				(60.0 * 8 / 9 * G_HMC_LINK_EFFICIENCY)
#define G_HMC_LINK_PAYLOAD_SIZE						8
#define G_HMC_LINK_PACKET_PER_CLOCK					(G_HMC_LINK_PAYLOAD_BANDWIDTH_GB * G_INTCNT_CLOCK_PERIOD_NS / G_HMC_LINK_PAYLOAD_SIZE)

#define G_NV_LINK_EFFICIENCY						0.8
#define G_NV_LINK_PAYLOAD_BANDWIDTH_GB				(50 * G_NV_LINK_EFFICIENCY)
#define G_NV_LINK_PAYLOAD_SIZE						8
#define G_NV_LINK_PACKET_PER_CLOCK					(G_NV_LINK_PAYLOAD_BANDWIDTH_GB * G_INTCNT_CLOCK_PERIOD_NS / G_NV_LINK_PAYLOAD_SIZE)

extern u64 stateCounter[16];


//#define G_ROW_READ_LATENCY		3
//#define G_ROW_WRITE_LATENCY		4
//#define G_ROW_RMW_LATENCY		5

#define G_ROW_OPEN_LATENCY				10
#define G_ROW_CLOSE_LATENCY				6
#define G_ROW_ACCESS_LATENCY			(G_ROW_OPEN_LATENCY + G_ROW_CLOSE_LATENCY)
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


#define PLACEMENT_QUEUE_MAX_CAPACITY		8
#define PLACEMENT_QUEUE_HIGH_WATER_MARK		7
#define PLACEMENT_QUEUE_LOW_WATER_MARK		2


// Global objects
//extern EventQueue eventQueue
extern double simTimeNs;
extern u64 radixSortMask;
extern u64 radixSortShift;

extern u64 radixStartBit;
extern u64 radixEndBit;

extern KEY_TYPE rangeEnd;
extern u64 numOfProcessedSubarrays;
extern u64 numOfInFlightPackets;
extern u64 lastIdx;
//extern u64 reservedBytesForReadWriteArray;
extern u64 hopCounter;

//extern LOCAL_ADDRESS_TYPE histStartAddr;
//extern LOCAL_ADDRESS_TYPE histEndAddr;
extern KEY_TYPE* dataArray;

extern u64 locShiftAmt;

//#include "PacketAllocator.hpp"
//extern PacketAllocator<PlacementPacket>* placementPacketAllocator;
#include "Packet.hpp"
extern Packet<PlacementPacket>* packetPool;

extern u64 placementRowHit;
extern u64 placementRowMiss;
extern u64 maxPlacementQSize;

extern u64 elemPerSubarray;

extern bool dragonEdges[64][64];
extern u8 dragonNextDst[64][64];
#include "PlacementEventQueue.hpp"
extern PlacementEventQueue placementEventQ;

//extern u64 numRowActivations;
//extern u64 numSubToSubPackets;
//extern u64 numBankToBankPackets;
//extern u64 numSegTSVPackets;


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

static void printSimTime(const std::string& msg = ""){
	std::cout << "[TIME " << simTimeNs << " ns] " << msg << std::endl;
}

static bool radixComp(KEY_TYPE a, KEY_TYPE b){
	a = (a & radixSortMask) >> radixSortShift;
	b = (b & radixSortMask) >> radixSortShift;
	return a < b;
}

constexpr ID_TYPE extractUptoLayer(ID_TYPE dstBankAddr){
	return dstBankAddr / G_NUM_BANKS_PER_LAYER;
}

constexpr ID_TYPE extractUptoStack(ID_TYPE dstBankAddr){
	return dstBankAddr / (G_NUM_BANKS_PER_LAYER * G_NUM_LAYERS_PER_STACK);
}

constexpr ID_TYPE extractUptoDevice(ID_TYPE dstBankAddr){
	return (dstBankAddr / (G_NUM_BANKS_PER_LAYER * G_NUM_LAYERS_PER_STACK * G_NUM_STACKS_PER_DEVICE));
}

constexpr ID_TYPE extractBankId(ID_TYPE dstBankAddr){
	return dstBankAddr % G_NUM_BANKS_PER_LAYER;
}

constexpr ID_TYPE extractLayerId(ID_TYPE dstBankAddr){
	return extractUptoLayer(dstBankAddr) % G_NUM_LAYERS_PER_STACK;
}

constexpr ID_TYPE extractStackId(ID_TYPE dstBankAddr){
	return extractUptoStack(dstBankAddr) % G_NUM_STACKS_PER_DEVICE;
}

constexpr ID_TYPE extractDeviceId(ID_TYPE dstBankAddr){
	return  extractUptoDevice(dstBankAddr);
}





