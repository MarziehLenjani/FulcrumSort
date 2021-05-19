/*
 * physicalComponent.hpp
 *
 *  Created on: May 16, 2021
 *      Author: marzieh
 */

#ifndef PHYSICALCOMPONENT_HPP_
#define PHYSICALCOMPONENT_HPP_
#include "types.hpp"
#include "configAndStats.hpp"


class physicalComponent{

public:
	 ID_TYPE id;
	 configAndStats * confObj;

	 physicalComponent * firstDimOwner;
	 physicalComponent * secondDimOwner;
	 physicalComponent * thirdDimOwner;
	 physicalComponent(ID_TYPE l_id, configAndStats * l_confObj, physicalComponent * l_firstDimOwner, physicalComponent * l_secondDimOwner, physicalComponent * l_thirdDimOwner);
	 ~physicalComponent();
};

#endif /* FULCRUM_SIM_DEVEL_SRC_PHYSICALCOMPONENT_HPP_ */
