#pragma once

#include "types.hpp"

struct physicalComponent{
	 ID_TYPE id;
	 physicalComponent* parent;

	 physicalComponent(ID_TYPE l_id, physicalComponent* l_parent) : id(l_id), parent(l_parent){};
};

