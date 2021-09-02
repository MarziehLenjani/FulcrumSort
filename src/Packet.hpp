#pragma once

#include "types.hpp"

template <typename T>
class Packet{
public:
	ID_TYPE dstId;
	T payload;

	Packet(ID_TYPE _dstId, const T& _payload) : dstId(_dstId), payload(_payload){};
};

