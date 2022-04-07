#pragma once

#include "EventBase.hpp"
#include "types.hpp"
#include <cassert>
#include <vector>

#define		MAX_EVENT_SCHEDULE_LATENCY_IN_CYCLES		1024

// A circular event queue
class EventQueue {

	std::vector<EventBase> eventQ[MAX_EVENT_SCHEDULE_LATENCY_IN_CYCLES];
	u64 currIdx = 0;
	u64 pendingEvents = 0;

public:

	void schedule(EventBase event, u64 latency){
		assert(latency < MAX_EVENT_SCHEDULE_LATENCY_IN_CYCLES);

		if(latency == 0){		//do it now
			event.callback(event.obj, event.arg);
		}
		else {		//schedule for a later time
			u64 idx = (currIdx + latency) % MAX_EVENT_SCHEDULE_LATENCY_IN_CYCLES;
			eventQ[idx].push_back(event);
			pendingEvents++;
		}
	}

	void schedule(void (*callback)(void*, void*), void* obj, void* arg, u64 latency){
		assert(latency < MAX_EVENT_SCHEDULE_LATENCY_IN_CYCLES);

		if(latency == 0){		//do it now
			callback(obj, arg);
		}
		else {		//schedule for a later time
			u64 idx = (currIdx + latency) % MAX_EVENT_SCHEDULE_LATENCY_IN_CYCLES;
			eventQ[idx].push_back({callback, obj, arg});
			pendingEvents++;
		}
	}

	u64 executeNextEvents(){
		if(pendingEvents == 0){
			return -1;		//no event to execute
		}
		u64 elapsedCycles = 0;
		while(eventQ[currIdx].size() == 0){
			currIdx++;
			elapsedCycles++;
			if(currIdx == MAX_EVENT_SCHEDULE_LATENCY_IN_CYCLES){
				currIdx = 0;	//wrap around
			}
		}

		// Now currIdx points to the next non-empty event list
		// Execute all events in currIdx
		for(EventBase& event : eventQ[currIdx]){
			event.callback(event.obj, event.arg);
		}

		pendingEvents -= eventQ[currIdx].size();

		return elapsedCycles;
	}

	u64 finish(){
		u64 elapsedCycles = 0;
		while(hasPendingEvents()){
			elapsedCycles += executeNextEvents();
		}
		return elapsedCycles;
	}

	bool hasPendingEvents(){
		return pendingEvents != 0;
	}

};
