/*
 * transferMedium.cpp

 *
 *  Created on: Feb 14, 2020
 *      Author: ml2au
 */
#include "transferMedium.hpp"
#include <cstddef>

#include "bank.hpp"
#include "computSubArray.hpp"
#include "configAndStats.hpp"
#include "layer.hpp"
#include "types.hpp"
#include "vault.hpp"

transferMedium::transferMedium(	computSubArray * ownerALPU_t,
		bank * ownerBank_t,
		layer * ownerLayer_t,
		vault* ownerVault_t) {
	ownerALPU=ownerALPU_t;
	ownerBank=ownerBank_t;
	ownerLayer=ownerLayer_t;
	ownerVault=ownerVault_t;
	ASSERT_EX(ownerVault!=NULL, std::cout<<"ownerVault="<<ownerVault);
	ASSERT_EX(ownerVault->configObjPointer!=NULL, std::cout<<"ownerVault->configObjPointer="<<ownerVault->configObjPointer);
	if(ownerALPU!=NULL){ //it means the medium is lisa Type
		widthInWORD=ownerVault->configObjPointer->numALUWidthInOneRow;
		latencyPerWidthInCycle=ownerVault->configObjPointer->coefficinetForInterSubArrayMovement*ownerVault->configObjPointer->rowCycleInCycle;
	}else{
		widthInWORD=1;
		latencyPerWidthInCycle=ownerVault->configObjPointer->columAccessByShiftInCycle;
	}

}
bool transferMedium::canTransfer(){
	bool canTransfer_t=!busy;
	TRACK_PRINTING(beingTracked, std::cout<<"busy="<<std::boolalpha<<busy<<std::endl);
	if(ownerALPU!=NULL){
		if(ownerALPU->getIsProcessing() or ownerALPU->getIsTranferring())
		{
			TRACK_And_WAIT_PRINTING(beingTracked,trackAndWait, std::cout<< "ownerALPU->subarrayID= "<<ownerALPU->subarrayID<<", ownerALPU->getIsProcessing()="<<std::boolalpha<<ownerALPU->getIsProcessing()<<", ownerALPU->getIsTranferring()="<<std::boolalpha<<ownerALPU->getIsTranferring()<<std::endl);
			canTransfer_t=false;
		}

	}
	return canTransfer_t;
}
void transferMedium::setBusy(bool busy) {
	this->busy = busy;
	if(ownerALPU!=NULL){
		ownerALPU->setIsTranferring(busy);
	}
}





