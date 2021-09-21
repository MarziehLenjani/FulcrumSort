
#include "Device.hpp"
#include "CXLLink.hpp"
#include "PulleySystem.hpp"


Device::Device(ID_TYPE l_id, PhysicalComponent* l_parent) : PhysicalComponent(l_id, l_parent) {
	for(u64 i = 0; i < G_NUM_STACKS_PER_DEVICE; i++){
		stackVector.push_back(new Stack(i, this));
	}
	for(u64 i = 0; i < G_NUM_DEVICES; i++){
		cxlLinks[i] = nullptr;
	}
}

Device::~Device(){
	for(Stack* it : stackVector){
		delete it;
	}

	for(u64 i = 0; i < G_NUM_DEVICES; i++){
		if(cxlLinks[i] != nullptr){
			delete cxlLinks[i];
			cxlLinks[i] = nullptr;
		}
	}
}


void Device::initLinks(){
	PulleySystem* pulley = (PulleySystem*) parent;
	for(u64 i = 0; i < G_NUM_DEVICES; i++){
		if(i != id){
			cxlLinks[i] = new CXLLink(pulley->deviceVector[i]);
		}
	}
}

void Device::runLinkOneClock(){
	for(u64 i = 0; i < G_NUM_DEVICES; i++){
		if(i != id){
			cxlLinks[i]->clock();
		}
	}
}
