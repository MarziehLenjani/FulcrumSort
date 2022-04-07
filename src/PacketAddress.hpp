//#pragma once
//
//#include "types.hpp"
//#include "globals.hpp"
//
//class PacketAddress {
//public:
//
//	// device | stack | layer | bank | subarray
//	ID_TYPE dstSubAddr = 0;
//
//	PacketAddress(ID_TYPE _dstSubAddr) : dstSubAddr(_dstSubAddr){}
//
////	constexpr ID_TYPE subAddr(){
////		return dstSubAddr;
////	}
////
////	constexpr ID_TYPE subId(){
////		return dstSubAddr % G_NUM_SUBARRAY_PER_BANK;
////	}
////
////	constexpr ID_TYPE bankId(){
////		return (dstSubAddr / G_NUM_SUBARRAY_PER_BANK) % G_NUM_BANKS_PER_LAYER;
////	}
////
////	constexpr ID_TYPE layerId(){
////		return (dstSubAddr / (G_NUM_SUBARRAY_PER_BANK * G_NUM_BANKS_PER_LAYER)) % G_NUM_LAYERS_PER_STACK;
////	}
////
////	constexpr ID_TYPE stackId(){
////		return (dstSubAddr / (G_NUM_SUBARRAY_PER_BANK * G_NUM_BANKS_PER_LAYER * G_NUM_LAYERS_PER_STACK)) % G_NUM_STACKS_PER_DEVICE;
////	}
////
////	constexpr ID_TYPE deviceId(){
////		return (dstSubAddr / (G_NUM_SUBARRAY_PER_BANK * G_NUM_BANKS_PER_LAYER * G_NUM_LAYERS_PER_STACK * G_NUM_STACKS_PER_DEVICE)) % G_NUM_DEVICES;
////	}
//
//	//constexpr u32 uptoSubAddr(){
//	//	return fullSubAddr;
//	//}
//
//	//constexpr u32 uptoBankAddr(){
//	//
//	//}
//
//
//
//};
