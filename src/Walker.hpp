#pragma once

#include "globals.hpp"
#include "types.hpp"
#include "MemoryObject.hpp"

class Walker {

public:
	Walker(){
		latchedRow = new MemoryObject(G_NUM_BYTES_IN_ROW, false);
	}
	~Walker(){
		delete latchedRow;
	}

	MemoryObject* latchedRow;
	u64 latchedRowIndex = -1;
	bool isPendingReq = false;

	//LOCAL_ADDRESS_TYPE currColumn = 0;
	//bool isDirty = false;
};


