
#include "NVLink.hpp"
#include "LogicLayer.hpp"
#include "HMCLink.hpp"
#include "Stack.hpp"
#include "Device.hpp"

LogicLayer::LogicLayer(PhysicalComponent * parent) : PhysicalComponent(parent->id, parent) {
	for(u64 i = 0; i < G_NUM_STACKS_PER_DEVICE; i++){
		hmcLinks[i] = nullptr;
	}
};

LogicLayer::~LogicLayer(){
	for(u64 i = 0; i < G_NUM_STACKS_PER_DEVICE; i++){
		if(hmcLinks[i] != nullptr){
			delete hmcLinks[i];
			hmcLinks[i] = nullptr;
		}
	}
}

void LogicLayer::runOneCycle(){
	for(u64 i = 0; i < G_NUM_STACKS_PER_DEVICE; i++){
		if(i != id){
			hmcLinks[i]->clock();
		}
	}

	for(u64 i = 0; i < G_NUM_BANKS_PER_LAYER; i++){
		if(!packetQs[i].empty()){
			auto pkt = packetQs[i].front();
			packetQs[i].pop();

			//TODO: Send packet via correct link

			u64 dstDeviceId = extractDeviceId(pkt->dstSubAddr);
			u64 currDeviceId = parent->parent->id;	//logicLayer -> stack -> device
			if(dstDeviceId != currDeviceId){
				//send via cxl link
				Stack* stack = (Stack*)parent;
				Device* device = (Device*) stack->parent;
				device->cxlLinks[dstDeviceId]->send(pkt);
			}
			else{
				u64 dstStackId = extractStackId(pkt->dstSubAddr);
				assert(dstStackId != id);
				hmcLinks[dstStackId]->send(pkt);
			}
		}
	}
}

void LogicLayer::initLinks(){
	Stack* stack = (Stack*)parent;
	Device* device = (Device*) stack->parent;
	for(u64 i = 0; i < G_NUM_STACKS_PER_DEVICE; i++){
		if(i != id){
			hmcLinks[i] = new HMCLink(device->stackVector[i]);
		}
	}
}

