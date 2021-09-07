#pragma once

#include "types.hpp"
#include "Packet.hpp"
#include <vector>

template <typename T>
class PacketAllocator{

private:
	std::vector<Packet<T>*> freeList;
	Packet<T>* base = nullptr;
	PacketAllocator(){};

public:

	PacketAllocator(u64 maxPackets){
		freeList.reserve(maxPackets);
		base = (Packet<T>*)malloc(maxPackets * sizeof(Packet<T>));
		for(long i = maxPackets - 1; i >= 0; i--){
			freeList.push_back(base + i);
		}
	}

	~PacketAllocator(){
		free(base);
	}

	Packet<T>* alloc(){
		const u64 qSize = freeList.size();
		assert(qSize > 0);
		Packet<T>* lastFree = freeList[qSize - 1];
		freeList.pop_back();
		return lastFree;
	}

	void free(Packet<T>* ptr){
		freeList.push_back(ptr);
	}
};
