#pragma once

#include "types.hpp"
#include "PacketAddress.hpp"

template <typename T>
class Packet{

	Packet(){}

public:
	//ID_TYPE dstId;	//destination subarray id within the stack
	//PacketAddress dstAddr;
	ID_TYPE dstSubAddr;
	T payload;

	Packet(ID_TYPE dstSubAddr, const T& _payload) : dstSubAddr(dstSubAddr), payload(_payload){

	};
};

