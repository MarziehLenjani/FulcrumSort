/*
 * statConfigNamesAndTypes.hpp
 *
 *  Created on: May 16, 2021
 *      Author: marzieh
 */

#ifndef STATCONFIGNAMES_HPP_
#define STATCONFIGNAMES_HPP_
#include "types.hpp"
#include<map>

#define CONF_TEST_NUMBER_NAME "testNumber"
#define CONF_TEST_NUMBER_TYPE ID_TYPE
//----
#define CONF_SATA_FILE_NAME "statFileName"
#define CONF_SATA_FILE_TYPE std::string
//---
#define CONF_NUMBER_OF_LAYERS_NAME "numLayers"
#define CONF_NUMBER_OF_LAYERS_TYPE ID_TYPE

#define CONF_NUMBER_OF_BANKS_PER_LAYER_NAME "numBanksPerLayer"
#define CONF_NUMBER_OF_BANKS_PER_LAYER_TYPE ID_TYPE

#define CONF_NUMBER_OF_COMPUTE_SUBARRAY_PER_BANK_NAME "numComputeSubArrayPerBank"
#define CONF_NUMBER_OF_COMPUTE_SUBARRAY_PER_BANK_TYPE ID_TYPE

#define CONF_SIZE_OF_EACH_SUBARRAY_IN_BYTE_NAME "sizeOFEachSubarrayInBytes"
#define CONF_SIZE_OF_EACH_SUBARRAY_IN_BYTE_TYPE ID_TYPE
//--------------------------------------
#define CONF_ADDRESS_MAPPING_NAME "addressMapping"
#define CONF_ADDRESS_MAPPING_TYPE std::string

#define CONF_NUMBER_OF_WORDS_IN_A_ROW_TYPE int
#define CONF_NUMBER_OF_WORDS_IN_A_ROW_NAME "numberOfWordsInARow"

#define CONF_ROW_CYCLE_IN_SUB_CLOCK_CYCLE_TYPE int
#define CONF_ROW_CYCLE_IN_SUB_CLOCK_CYCLE_NAME "rowCycleInSubClockCyle"


enum addressMappingEnum{MLBS, MVLBS}; //the second one is memory, vault, layer, bank, subarray
class mapStringToMappingEnum{
public:
	std::map<std::string, addressMappingEnum> xmap=  map_list_of("MLBS",MLBS)("MVLBS", MVLBS);

};

//-------------------------

#endif /* STATCONFIGNAMES_HPP_ */
