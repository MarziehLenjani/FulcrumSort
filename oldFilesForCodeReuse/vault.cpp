#include "vault.hpp"

#include "../src/Bank.hpp"
#include "../src/Layer.hpp"
#include "logicLayer.hpp"
#include "computSubArray.hpp"
#include "types.hpp"
#include "walkerDataAndSetting.hpp"
#include "configAndStats.hpp"
#include "transferMedium.hpp"
int vault::vaultID=0;
vault::vault(configAndStats* configObjPointer_t){
	configObjPointer=configObjPointer_t;
	ASSERT_EX(configObjPointer!=NULL, std::cout<<"configObjPointer="<<configObjPointer);
	numLayer=configObjPointer_t->numLayer;
	numBankPerVaultLayer=configObjPointer_t->numBankPerVaultLayer;
	numComSubPerBansk=configObjPointer_t->numComSubPerBansk;
	totalSubArrayCompute=numLayer*numBankPerVaultLayer*numComSubPerBansk;
	logicLayerObj= new logicLayer(this);
	Bank * tOwnerBank;
	Layer * tOwnerLayer;

	assert(this==logicLayerObj->ownerVault);
	int subArrayPerBankCounter=0;
	for (int i=0;i<totalSubArrayCompute;i++){
		if(i%(numComSubPerBansk*numBankPerVaultLayer)==0){
			tOwnerLayer= new Layer(this);
		}
		if(i% (numComSubPerBansk)==0){
			tOwnerBank=new Bank(tOwnerLayer, this);
		}

		compSubVector.push_back(new computSubArray(i, configObjPointer_t->memoryArraySize, this, tOwnerBank,tOwnerLayer) );
	}
	vaultID++;

	 tsv = new transferMedium (NULL, NULL, NULL,this);

}
vault::~vault(){
	//TODO: delete banks, layers and subarrays
	delete tsv;
	delete logicLayerObj;
	auto it = compSubVector.begin();


	while (it != compSubVector.end()) {
			// Remove elements while iterating
		delete (*it);
		it = compSubVector.erase(it);

	}

}
void vault::restIndexes(){
	for(computSubArray* sub : compSubVector){
		sub->restIndexes();
	}
}

void vault::restData(){
	for(computSubArray* sub : compSubVector){
		sub->restData();
	}
}
bool vault::allFree(){
	bool allFree=true;
	for (auto sbc = compSubVector.begin(); sbc != compSubVector.end(); ++sbc) {
		if((*sbc)->isBusy()==true)
			allFree=false;

	}
	return allFree;
}
void vault::queuJob(walkerDataAndSetting* jb1, int queuID){
	//since it is being created from a reading buffer

	jb1-> setCurIndex(0);

	logicLayerObj->queuJob(jb1, queuID);
}
bool vault::proceed(outPutofAssignedJobTypeEnum outJobType, bool broadCastALPU_settings){

	//123

	for(int i=0;i<totalSubArrayCompute;i++){

		TRACK_PRINTING(i==subArrayTrackID,std::cout<<"subarrayID:"<<i<<std::endl);

		bool busy_t=compSubVector[i]->isBusy();
		assert(this==logicLayerObj->ownerVault);
		if(busy_t==true)
		{
			TRACK_PRINTING(i==subArrayTrackID, std::cout<<"subarrayID:"<<i<<":is proceeding because busy_t = "<<std::boolalpha <<busy_t<<std::endl);
			compSubVector[i]->proceedOneCycle();

		}else{
			TRACK_PRINTING(i==subArrayTrackID, std::cout<<"subarrayID:"<<i<<":is not proceeding because busy_t =:"<<std::boolalpha <<busy_t<<std::endl);
		}

	}
	//This function checks all the jobs queued and sees if any task is in progress or if any task is waiting

	bool inProgress=logicLayerObj->queuProceed(outJobType, broadCastALPU_settings);
	return(inProgress);
}
/*
bool vault::isALPU_Free(int ALPU_ID){
	return compSubVector[ALPU_ID].isALPU_Free();
}
*/
void vault::assignAndSrartJobOnALPU(walkerDataAndSetting * tJob1,walkerDataAndSetting * tJob2, int ALPU_ID, outPutofAssignedJobTypeEnum outJobType){
	compSubVector[ALPU_ID]->assignAndSrartJobOnALPU(tJob1,tJob2, outJobType);
}
walkerDataAndSetting * vault::returnOutPuJob(int ALPU_ID){
	return compSubVector[ALPU_ID]->job3;
}
void vault::setSubArrayTrackID(int t_subArrayTrackID){
	subArrayTrackID=t_subArrayTrackID;
}
void vault::setQueueTrackID(int t_queueTrackID){
	queueTrackID=t_queueTrackID;
	logicLayerObj->setQueueTrackID(t_queueTrackID);
}


void vault::brdCastjobButNotStart(walkerDataAndSetting * tJob,  int jobNumber, bool isAPersistentJob_t, bool sharedMemoryWalkerValues_t){

	for (int i=0;i<totalSubArrayCompute;i++){
		walkerDataAndSetting* newJob=new walkerDataAndSetting(tJob,sharedMemoryWalkerValues_t);
		assert(newJob!=NULL);
		newJob->isAPersistentJob=isAPersistentJob_t;
		compSubVector[i]->AssignAjobButNotStart( newJob,  jobNumber);
	}

}


