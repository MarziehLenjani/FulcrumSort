#pragma once

#include <cstdlib>
#include <cstring>
#include <cassert>
#include "types.hpp"
#include "configAndStats.hpp"

class MemoryObject {

private:
	u64 size = 0;		//in bytes

	MemoryObject(){}

	void assertValidAccess(LOCAL_ADDRESS_TYPE addr, u64 accSize) const {
		assert((addr + accSize) <= size);
	}


public:

	u8* data = nullptr;
	bool allocated = false;

	MemoryObject (const u64 _size, bool allocate = true) {
		size = _size;
		if(allocate) {
			data = (u8*)malloc(size);
			allocated = true;
		}
	}

	~MemoryObject(){
		if(allocated){
			free(data);
		}
	}

	template <typename T>
	T readX(LOCAL_ADDRESS_TYPE addr) const {
		//assertValidAccess(addr, sizeof(T));
		return *((T*)(data + addr));
	}

	// variable length reads
	void read(LOCAL_ADDRESS_TYPE addr, u64 readSize, u8* dst) const {
		assertValidAccess(addr, readSize);
		memcpy(dst, data + addr, readSize);
	}

	u8 read8(LOCAL_ADDRESS_TYPE addr) const {
		return readX<u8>(addr);
	}

	u16 read16(LOCAL_ADDRESS_TYPE addr) const {
		return readX<u16>(addr);
	}

	u32 read32(LOCAL_ADDRESS_TYPE addr) const {
		return readX<u32>(addr);
	}

	u64 read64(LOCAL_ADDRESS_TYPE addr) const {
		return readX<u64>(addr);
	}

	LOCAL_ADDRESS_TYPE readLocalAddr(LOCAL_ADDRESS_TYPE addr){
		return readX<LOCAL_ADDRESS_TYPE>(addr);
	}

	FULCRU_WORD_TYPE readWord(LOCAL_ADDRESS_TYPE addr) const {
		return readX<FULCRU_WORD_TYPE>(addr);
	}

	KEY_TYPE readKey(LOCAL_ADDRESS_TYPE addr) const {
		return readX<KEY_TYPE>(addr);
	}

	HIST_ELEM_TYPE readHist(LOCAL_ADDRESS_TYPE addr) const {
		return readX<HIST_ELEM_TYPE>(addr);
	}




	template <typename T>
	void writeX(LOCAL_ADDRESS_TYPE addr, const T val){
		//assertValidAccess(addr, sizeof(T));
		*((T*)(data + addr)) = val;
	}

	// variable length writes
	void write(LOCAL_ADDRESS_TYPE addr, u64 writeSize, u8* src) {
		assertValidAccess(addr, writeSize);
		memcpy(data + addr, src, writeSize);
	}

	void write8(LOCAL_ADDRESS_TYPE addr, u8 val) {
		writeX<u8>(addr, val);
	}

	void write16(LOCAL_ADDRESS_TYPE addr, u16 val) {
		writeX<u16>(addr, val);
	}

	void write32(LOCAL_ADDRESS_TYPE addr, u32 val) {
		writeX<u32>(addr, val);
	}

	void write64(LOCAL_ADDRESS_TYPE addr, u64 val) {
		writeX<u64>(addr, val);
	}

	void writeLocalAddr(LOCAL_ADDRESS_TYPE addr, const LOCAL_ADDRESS_TYPE val) {
		writeX<LOCAL_ADDRESS_TYPE>(addr, val);
	}

	void writeWord(LOCAL_ADDRESS_TYPE addr, const FULCRU_WORD_TYPE val) {
		writeX<FULCRU_WORD_TYPE>(addr, val);
	}

	void writeKey(LOCAL_ADDRESS_TYPE addr, const KEY_TYPE val) {
		writeX<KEY_TYPE>(addr, val);
	}

	void writeHist(LOCAL_ADDRESS_TYPE addr, const HIST_ELEM_TYPE val) {
		writeX<HIST_ELEM_TYPE>(addr, val);
	}



	void incrementHist(LOCAL_ADDRESS_TYPE addr){
		((HIST_ELEM_TYPE*)(data + addr))[0]++;
	}

};
