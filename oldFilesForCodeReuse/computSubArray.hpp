#ifndef COMPUTE_SUBARRAY_HPP
#define COMPUTE_SUBARRAY_HPP
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
// Utilities and system includes
#include <vector>
#include <math.h>
#include <queue>
//----------------------
#include "types.hpp"


class vault;
class memoryArray;
class walkerDataAndSetting;
class sparseMatrix;
class bank;
class layer;
class transferMedium;
class computSubArray{
public:
	int CurrColIndex=0;
	int subarrayID;
	//IndexType *ColIndexs;
	//float *val;
	sparseMatrix* matrixPointer;
	int length=0;
	vault* ownerVault;
	bank * ownerBank;
	layer * ownerLayer;
	computSubArray(int subarrayIDt, int memArrSize, vault* t_ownerVault, bank* tOwnerBank,layer* tOwnerLayer );
	~computSubArray();
	void computSubArrayAssignData(sparseMatrix * spmPointer , IndexType t_start, int lengtht );

	void restIndexes();
	bool hasData;
	void restData();
	void proceed();
	void proceedOneCycle(void);
	void track(int subArrayBeingTracked, bool debugMode, std::string message );

	int numALUWidthInOneRow=-1;
	int columAccessByShiftInCycle=-1;
	int rowCycleInCycle=-1;

	memoryArray* memArr;
	int memArrSize;


	bool isBusy();
	bool isFree();
	walkerDataAndSetting * job1=NULL;
	walkerDataAndSetting * job2=NULL;
	walkerDataAndSetting * job3=NULL;
	//bool isALPU_Free();
	void assignAndSrartJobOnALPU(walkerDataAndSetting * tJob1,walkerDataAndSetting * tJob2, outPutofAssignedJobTypeEnum outJobTypeOfRunningJob_t);
	void AssignAjobButNotStart(walkerDataAndSetting * tJob, int jobNumber);
	void clearAReadBuffer(walkerDataAndSetting*& tJob);
	void clearAWriteBufferTobeQueued(walkerDataAndSetting*  &tJob);

	bool getIsProcessing() const {
		return isProcessing;
	}

	void setIsProcessing(bool isProcessing) {
		this->isProcessing = isProcessing;
	}

	bool getIsTranferring() const {
		return isTranferring;
	}

	void setIsTranferring(bool isTranferring) {
		this->isTranferring = isTranferring;
	}

	bool compFlagEqual=false;
	bool compFlagGreat=false;
	bool compFlagLess=false;
	bool beingTracked=false;
	bool even=true;
	outPutofAssignedJobTypeEnum outJobTypeOfRunningJob;
	int waitForRowCycleCounter=0;
	std::queue<walkerDataAndSetting *> waitingForRowCycleJob;
	bool justWaitingForTheLastWrite=false;
	transferMedium* lisa;
private:
	bool isProcessing=false;
	bool isTranferring=false;
	//bool busy;

};
#endif
