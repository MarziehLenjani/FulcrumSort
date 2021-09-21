#pragma once

#include "types.hpp"

struct PhysicalComponent{
	 //const ID_TYPE localId;
	 //const ID_TYPE globalId;
	const ID_TYPE id;
	 PhysicalComponent* parent;

	//physicalComponent(ID_TYPE localId, ID_TYPE globalId, physicalComponent* parent) : localId(localId), globalId(globalId), parent(parent){};
	 PhysicalComponent(ID_TYPE id, PhysicalComponent* parent) : id(id), parent(parent){};
};

