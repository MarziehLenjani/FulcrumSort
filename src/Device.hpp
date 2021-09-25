#pragma once

#include <cstdlib>
#include <vector>
#include <cassert>
#include "globals.hpp"
#include "HMCLink.hpp"
#include "PhysicalComponent.hpp"
#include "Stack.hpp"

class NVLink;

class Device : public PhysicalComponent {
public:
	std::vector<Stack*> stackVector;

	NVLink* cxlLinks[G_NUM_DEVICES];

	Device(ID_TYPE l_id, PhysicalComponent* l_parent);
	~Device();

	void initLinks();
	void runLinkOneClock();
};
