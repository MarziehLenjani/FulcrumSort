/*
 * dataTransfer.cpp
 *
 *  Created on: Jun 3, 2021
 *      Author: marzieh
 */
#include "dataTransfer.hpp"
dataTransfer::dataTransfer(ID_TYPE t_payloadWidthIn_READ_DATA_TYPE, ID_TYPE t_destinationID, READ_DATA_TYPE_IN_MEMORY_ARRAY * t_payload)
{
  payloadWidthIn_READ_DATA_TYPE=t_payloadWidthIn_READ_DATA_TYPE;
   destinationID=t_destinationID;
   payload= (READ_DATA_TYPE_IN_MEMORY_ARRAY*) calloc(payloadWidthIn_READ_DATA_TYPE, sizeof(READ_DATA_TYPE_IN_MEMORY_ARRAY));
   memcpy ( payload, t_payload, payloadWidthIn_READ_DATA_TYPE*sizeof(READ_DATA_TYPE_IN_MEMORY_ARRAY));
}

dataTransfer::~dataTransfer(){
	free(payload);
	payload=NULL;
}

