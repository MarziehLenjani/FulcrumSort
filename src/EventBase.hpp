#pragma once

class EventBase {
public:
	void (* callback)(void*, void*);		//callback function
	void* obj;								//object
	void* arg;								//argument passed to the callback function
};
