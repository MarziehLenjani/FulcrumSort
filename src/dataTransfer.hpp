/*
 * dataTransfer.hpp
 *
 *  Created on: May 20, 2021
 *      Author: marzieh
 */

#ifndef DATATRANSFER_HPP_
#define DATATRANSFER_HPP_
#include "types.hpp"

class dataTransfer{
public:
	ID_TYPE payloadWidthIn_READ_DATA_TYPE=0;
	ID_TYPE destinationID=0;
	READ_DATA_TYPE_IN_MEMORY_ARRAY * payload=NULL;
	dataTransfer(ID_TYPE t_payloadWidthIn_READ_DATA_TYPE, ID_TYPE t_destinationID, READ_DATA_TYPE_IN_MEMORY_ARRAY * t_payload);
	~dataTransfer();
};



#endif /* FULCRUMSORT_SRC_DATATRANSFER_HPP_ */
