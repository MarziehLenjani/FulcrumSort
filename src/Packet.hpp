#pragma once

#include "types.hpp"
#include "PacketAddress.hpp"

template <typename T>
class Packet{
public:
	//ID_TYPE dstId;	//destination subarray id within the stack
	//PacketAddress dstAddr;
	ID_TYPE dstBankAddr;
	ID_TYPE dstSubId;
	T payload;
};

