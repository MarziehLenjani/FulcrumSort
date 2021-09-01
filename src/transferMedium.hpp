/*
 * transferMedium.hpp
 *
 *  Created on: Feb 12, 2020
 *      Author: ml2au
 */

#ifndef TRANSFERMEDIUM_HPP_
#define TRANSFERMEDIUM_HPP_
#include "types.hpp"
#include "physicalComponent.hpp"
#include <deque>

#include "dataTransfer.hpp"
class transferMedium:public physicalComponent{
	bool busy=false;
public:
	std::deque<dataTransfer *> dataTransferQueue;
	ID_TYPE widthInBits;
	ID_TYPE latencyPerPacketInCycle;
	bool isBusy();
	void setBusy(bool busy) ;
	transferMedium(ID_TYPE l_id, physicalComponent * l_firstDimOwner, physicalComponent * l_secondDimOwner, physicalComponent * l_thirdDimOwner);
	~transferMedium();

};



#endif /* TRANSFERMEDIUM_HPP_ */
