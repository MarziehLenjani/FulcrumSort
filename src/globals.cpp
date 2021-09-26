#include "globals.hpp"
#include "EventQueue.hpp"
#include "PacketAllocator.hpp"

EventQueue eventQueue;
u64 simCycles = 0;

u64 radixSortMask = 0;
u64 radixSortShift = 0;
KEY_TYPE rangeEnd = 0;
u64 numOfProcessedSubarrays = 0;
u64 numOfInFlightPackets = 0;
u64 lastIdx = 0;
//u64 reservedBytesForReadWriteArray = 0;

//LOCAL_ADDRESS_TYPE histStartAddr = 0;
//LOCAL_ADDRESS_TYPE histEndAddr = 0;
KEY_TYPE* dataArray = nullptr;

u64 radixStartBit = 0;
u64 radixEndBit = 0;

u64 stateCounter[16];
u64 hopCounter = 0;
u64 locShiftAmt = 0;

//PacketAllocator<PlacementPacket>* placementPacketAllocator = nullptr;
Packet<PlacementPacket>* packetPool = nullptr;

u64 placementRowHit = 0;
u64 placementRowMiss = 0;

u64 elemPerSubarray = 0;

bool dragonEdges[64][64];
u8 dragonNextDst[64][64];
