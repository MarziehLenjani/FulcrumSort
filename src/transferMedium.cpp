/*
 * transferMedium.cpp

 *
 *  Created on: Feb 14, 2020
 *      Author: ml2au
 */
#include "transferMedium.hpp"
#include <cstddef>

transferMedium::transferMedium(ID_TYPE l_id, configAndStats * l_confObj, physicalComponent * l_firstDimOwner, physicalComponent * l_secondDimOwner, physicalComponent * l_thirdDimOwner)
:physicalComponent(l_id, l_confObj, l_firstDimOwner, l_secondDimOwner, l_thirdDimOwner){


}
transferMedium::~transferMedium(){

}



