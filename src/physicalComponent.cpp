/*
 * physicalComponent.cpp
 *
 *  Created on: May 16, 2021
 *      Author: ml2au
 */
#include "physicalComponent.hpp"


physicalComponent::physicalComponent(ID_TYPE l_id, configAndStats * l_confObj, physicalComponent * l_firstDimOwner, physicalComponent * l_secondDimOwner, physicalComponent * l_thirdDimOwner){
	id=l_id;
	confObj=l_confObj;
	firstDimOwner=l_firstDimOwner;
	secondDimOwner=l_secondDimOwner;
	thirdDimOwner=l_thirdDimOwner;


}
physicalComponent::~physicalComponent(){


}



