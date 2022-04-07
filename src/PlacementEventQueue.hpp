#pragma once

#include "types.hpp"
#include <cassert>
#include <vector>

#define		MAX_ROW_ACT_LATENCY_IN_CYCLES				32

class Subarray;

// A circular event queue
class PlacementEventQueue {

	std::vector<Subarray*> eventQ[MAX_ROW_ACT_LATENCY_IN_CYCLES];
	u64 currIdx = 0;

public:

	void clock(){
		eventQ[currIdx].clear();
		currIdx++;
		currIdx = currIdx % MAX_ROW_ACT_LATENCY_IN_CYCLES;
	}

	void schedule(Subarray* sub, u64 latency){
		u64 idx = (currIdx + latency) % MAX_ROW_ACT_LATENCY_IN_CYCLES;
		eventQ[idx].push_back(sub);
	}

	std::vector<Subarray*> getCurrEventList(){
		return eventQ[currIdx];
	}
};
