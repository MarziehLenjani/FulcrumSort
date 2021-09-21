#include "computSubArray.hpp"
#include "memoryArray.hpp"
#include "types.hpp"
#include <string>
#include <iostream>
#include <sstream>
#include <assert.h>

#include "../src/Bank.hpp"
#include "vault.hpp"
#include "memoryArray.hpp"
#include "walkerDataAndSetting.hpp"
#include "configAndStats.hpp"
#include "transferMedium.hpp"
 computSubArray::computSubArray(int subarrayIDt, int memArrSize, vault* t_ownerVault, Bank* tOwnerBank,Layer* tOwnerLayer){
	 ownerVault=t_ownerVault;
	 subarrayID=subarrayIDt;
	 ownerBank=tOwnerBank;
	 ownerLayer=tOwnerLayer;
	 numALUWidthInOneRow=ownerVault->configObjPointer->numALUWidthInOneRow;
	 rowCycleInCycle=ownerVault->configObjPointer->rowCycleInCycle;
	 columAccessByShiftInCycle=ownerVault->configObjPointer->columAccessByShiftInCycle;
	 isProcessing=false;
	 lisa = new transferMedium (this, ownerBank, ownerLayer,ownerVault);
	 //cycleInNS=6;
	 //std::cout<<"columAccessByShiftInCycle "<<columAccessByShiftInCycle<<std::endl;
	 //std::cout<<"cycleInNS "<<cycleInNS<<std::endl;
	 //exit(0);
//	 if(memArrSize>0){
//		 memArr=new memoryArray(memArrSize);
//	 }

 }

 computSubArray::~computSubArray(){
	if(subarrayID%(ownerVault->numComSubPerBansk*ownerVault->numBankPerVaultLayer)==0){
			delete ownerLayer;
	}
	if(subarrayID% (ownerVault->numComSubPerBansk)==0){
			delete ownerBank;
	}

	 delete lisa;
 //	delete memArr;

  }
 void computSubArray::computSubArrayAssignData(sparseMatrix * spmPointer , IndexType t_start, int lengtht ){
	 //ColIndexs=ColIndexst;
	 //val=valt;
	 matrixPointer=spmPointer;
	 length=lengtht;
	 hasData=true;

 }
 void computSubArray::track(int subArrayBeingTracked, bool debugMode, std::string message ){
	if(((subarrayID==subArrayBeingTracked) ||(subArrayBeingTracked<0)) && debugMode==true)
		std::cout<<"subarray : "<<subarrayID<<message<<'\n';
 }
void computSubArray::restIndexes(){
	CurrColIndex=0;
}
void computSubArray::restData(){
	hasData=false;
	length=0;
	restIndexes();
}
void computSubArray::proceed(){
	// this is actually proceeding two cycles
	CurrColIndex++;
}
void computSubArray::proceedOneCycle(void){
	//exit(1);
	//TODO: finish this
	TRACK_PRINTING(beingTracked, std::cout<<"subarrayID:"<<subarrayID<<"--------proceedOneCycle start report -----------"<<std::endl);
	TRACK_PRINTING(beingTracked, std::cout<<"subarrayID:"<<subarrayID<<", isProcessing ="<<std::boolalpha<<isProcessing<<", job1="<<job1 <<", job2="<<job2<<", job3="<<job3 <<std::endl);
	TRACK_PRINTING(beingTracked, std::cout<<"subarrayID:"<<subarrayID<<", waitingForRowCycleJob.size():"<<waitingForRowCycleJob.size()<< ", waitForRowCycleCounter:" <<waitForRowCycleCounter<<std::endl);
	TRACK_PRINTING(beingTracked and job1!=NULL, std::cout<<"job1->singleWalker="<<std::boolalpha<<job1->singleWalker<<std::endl);
	TRACK_PRINTING(beingTracked and job2!=NULL , std::cout<<"job2->singleWalker="<<std::boolalpha<<job2->singleWalker<<std::endl);

	if(isProcessing){
		if(job1!=NULL ){
			if(job1->RowLoadWriteIsNeeded==true and job1->queuToLoadNewRow==false){
				waitingForRowCycleJob.push(job1);
				job1->queuToLoadNewRow=true;
				TRACK_PRINTING(beingTracked, std::cout<<"subarrayID:"<<subarrayID<<", Job1 is queued to wait for row cycle"<<std::endl);
			}
		}
		if(job2!=NULL ){
			if(job2->RowLoadWriteIsNeeded==true and job2->queuToLoadNewRow==false){
				waitingForRowCycleJob.push(job2);
				job2->queuToLoadNewRow=true;
				TRACK_PRINTING(beingTracked, std::cout<<"subarrayID:"<<subarrayID<<", Job2 is queued to wait for row cycle"<<std::endl);
			}
		}
		if(job3!=NULL ){
			if(job3->RowLoadWriteIsNeeded==true and job3->queuToLoadNewRow==false){
				waitingForRowCycleJob.push(job3);
				job3->queuToLoadNewRow=true;
				TRACK_PRINTING(beingTracked, std::cout<<"subarrayID:"<<subarrayID<<", Job3 is queued to wait for row cycle"<<std::endl);

			}
		}
	}

	if(waitingForRowCycleJob.size()>0){
		TRACK_PRINTING(beingTracked, std::cout<<"subarrayID:"<<subarrayID<<", waitingForRowCycleJob.size():"<<waitingForRowCycleJob.size()<< ", waitForRowCycleCounter:" <<waitForRowCycleCounter<<std::endl);
		if(waitForRowCycleCounter==ceil(rowCycleInCycle)-1)
		{
			auto waitingJob=waitingForRowCycleJob.front();
			waitingJob->RowLoadWriteIsNeeded=false;
			waitingJob->queuToLoadNewRow=false;
			waitingForRowCycleJob.pop();
			waitForRowCycleCounter=0;
			TRACK_PRINTING(beingTracked, std::cout<<"subarrayID:"<<subarrayID<<"end of waiting for one job"<<", waitingForRowCycleJob.size():"<<waitingForRowCycleJob.size()<< ", waitForRowCycleCounter:" <<waitForRowCycleCounter<<std::endl);

			//TODO: complete this
			if(waitingJob->isWriteBuffer and waitingJob->writejobDone and outJobTypeOfRunningJob==A_NEW_JOB_ASSIGN_TO_JOB3_AND_QUEUED){
				ownerVault->queuJob(waitingJob, subarrayID);
				TRACK_PRINTING(beingTracked, std::cout<<"subarrayID:"<<subarrayID<<", A write buffer is queued:"<<waitingForRowCycleJob.size()<< ", waitForRowCycleCounter:" <<waitForRowCycleCounter<<std::endl);

			}
			TRACK_PRINTING(beingTracked, std::cout<<"after counter ends, job1="<<job1<<", job2=" <<job2<<", job3="<<job3 <<", isProcessing "<<std::boolalpha<<isProcessing<<std::endl);
			TRACK_PRINTING(beingTracked, std::cout<<"after counter ends, waitingForRowCycleJob.size()="<<waitingForRowCycleJob.size() <<"isProcessing ="<<std::boolalpha<<isProcessing<<", justWaitingForTheLastWrite"<<std::boolalpha<<justWaitingForTheLastWrite<<std::endl);

			if(waitingForRowCycleJob.size()>0)
			{
				TRACK_PRINTING(beingTracked, std::cout<<"still we have waiting, waitingForRowCycleJob.size="<<waitingForRowCycleJob.size()<<std::endl;);
				return;
			}else{
				if(justWaitingForTheLastWrite==true){
					TRACK_PRINTING(beingTracked, std::cout<<"no more waiting: clearing a write buffer"<<std::endl);

					clearAWriteBufferTobeQueued(job3);
					isProcessing=false;
					TRACK_PRINTING(beingTracked, std::cout<<"after clearing a write buffer, job1="<<job1<<", job2=" <<job2<<", job3="<<job3 <<", isProcessing "<<std::boolalpha<<isProcessing<<std::endl);

					return;
				}
			}
		}else{
			waitForRowCycleCounter++;
			return;
		}
	}

	if(job1!=NULL and job2!=NULL and isProcessing and job1->singleWalker==false and job2->singleWalker==false){
		//proceed for the two operand task for only one cycle
		//TODO: replace this with execution with instruction buffer
		//TODO: implement row cycle
		//TODO: implement alpha multiplication
		TRACK_PRINTING(beingTracked, std::cout<<"subarrayID:"<<subarrayID<<"\n"
		<<"job1->getCurIndex():"<<job1->getCurIndex()<<", job2->getCurIndex():"<<job2->getCurIndex()<<std::endl
		<<"job1->length:"<<job1->length<<", job2->length:"<<job2->length<<std::endl
		);
			if(even){
				TRACK_PRINTING(beingTracked, std::cout<<"subarrayID:"<<subarrayID<<":even index"<<std::endl);

				//TODO: add length end checking policy to the RTL code
				int shiftedValue;
				compFlagEqual=false;
				compFlagLess=false;
				compFlagGreat=false;

				if(job1->getCurIndex()<job1->length && job2->getCurIndex()>=job2->length){// job2 needs to continue then it is like its index is less than the job1
					compFlagLess=true;
					//shiftedValue=castNoLoss <REGISTER_TYPE, int>(job1->walkerValues[job1->getCurIndex()]);
					shiftedValue=castNoLoss <REGISTER_TYPE, int>(job1->readFromBuffer());
					TRACK_PRINTING(beingTracked, std::cout<<"subarrayID:"<<subarrayID<<":job2 ended but job1 is going"<<std::endl);
				}else{//else 1
					if(job1->getCurIndex()>=job1->length && job2->getCurIndex()<job2->length){
						compFlagGreat=true;
						//shiftedValue=castNoLoss<REGISTER_TYPE, int>(job2->walkerValues[job2->getCurIndex()]);
						shiftedValue=castNoLoss<REGISTER_TYPE, int>(job2->readFromBuffer());
						TRACK_PRINTING(beingTracked, std::cout<<"subarrayID:"<<subarrayID<<":job1 ended but job2 is going"<<std::endl);
					}else{ //else 2
						if(job1->getCurIndex()>=job1->length && job2->getCurIndex()>=job2->length){

									//even=true;
									if(outJobTypeOfRunningJob==A_NEW_JOB_ASSIGN_TO_JOB3_AND_QUEUED)
									{
										//TODO: creat a new function: tranform a write buffer to job
										//queu  job resets everything fo creating a reading job even if it was

										job3->length=job3->getCurIndex();
										assert(job3->length<=job3->maxLength);

										justWaitingForTheLastWrite=true;
										job3->RowLoadWriteIsNeeded=true;
										job3->writejobDone=true;
									}else{
										isProcessing=false;
									}
									//TODO: check if deleting these jobs creates any problem
									TRACK_PRINTING(beingTracked, std::cout<<"subarrayID:"<<subarrayID<<":*******************befor deleting"<<std::endl);
									assert(job1!=NULL);
									assert(job2!=NULL);
									assert(job2!=job1);
									//delete(job2);
									//delete(job1);

									clearAReadBuffer(job1);
									clearAReadBuffer(job2);




									TRACK_PRINTING(beingTracked, std::cout<<"subarrayID:"<<subarrayID<<":*******************this two walker job is done"<<std::endl);
						}else{ //else 3
							//TODO: make walkerValue private and only write to it with checking

							int colIndx1=castNoLoss<REGISTER_TYPE, int>(job1->readFromBuffer());
							int colIndx2=castNoLoss<REGISTER_TYPE, int>(job2->readFromBuffer());
							TRACK_PRINTING(beingTracked, std::cout<<"subarrayID:"<<subarrayID<<":both job1 and job2 are going"<<std::endl);


							if(colIndx1<colIndx2){
								compFlagLess=true;

							}
							if(colIndx1>colIndx2){
								compFlagGreat=true;
							}
							if(colIndx1==colIndx2){
								compFlagEqual=true;
							}
						} //end of else 3

					} //end os else 2
				} //end of else 1


				TRACK_PRINTING(beingTracked, std::cout<<"subarrayID:"<<subarrayID<<", compFlagEqual="<<std::boolalpha <<compFlagEqual<<std::endl);
				TRACK_PRINTING(beingTracked, std::cout<<"subarrayID:"<<subarrayID<<", compFlagLess="<<std::boolalpha <<compFlagLess<<std::endl);
				TRACK_PRINTING(beingTracked, std::cout<<"subarrayID:"<<subarrayID<<", compFlagGreat="<<std::boolalpha <<compFlagGreat<<std::endl);

				if(isProcessing)
				{
					ASSERT_EX(job3==NULL or (job3->writejobDone ==true or outJobTypeOfRunningJob!=A_NEW_JOB_ASSIGN_TO_JOB3_AND_QUEUED or job3->getCurIndex()< job3->maxLength), std::cout <<"job3->getCurIndex():"<<job3->getCurIndex()<<", job3->getCurIndex()" <<job3->getCurIndex()<<std::endl;);
					if(compFlagEqual==true ){


						job1->incAndCheckForWaitCycle(1);
						job2->incAndCheckForWaitCycle(1);
						if(outJobTypeOfRunningJob==A_NEW_JOB_ASSIGN_TO_JOB3_AND_QUEUED and job3->writejobDone !=true  )
						{

							job3->writeToBuffer(castNoLoss<int, REGISTER_TYPE>(shiftedValue));
							job3->incAndCheckForWaitCycle(1);
						}
					}else{
						if(compFlagLess==true){
							//check that always means the first one is less than the second

							if( outJobTypeOfRunningJob==A_NEW_JOB_ASSIGN_TO_JOB3_AND_QUEUED and job3->writejobDone !=true )
							{
								job3->writeToBuffer(castNoLoss<int, REGISTER_TYPE>(shiftedValue));
								job3->incAndCheckForWaitCycle(1);
							}
							job1->incAndCheckForWaitCycle(1);
						}else{
							if(compFlagGreat==true){

								if( outJobTypeOfRunningJob==A_NEW_JOB_ASSIGN_TO_JOB3_AND_QUEUED and job3->writejobDone !=true)
								{
									job3->writeToBuffer(castNoLoss<int, REGISTER_TYPE>(shiftedValue));
									job3->incAndCheckForWaitCycle(1);
								}
								job2->incAndCheckForWaitCycle(1);
							}
						}
					}
				}//end of isProcessing
					even=false;
			}else{// end of walker odd indexes, col index checking
				TRACK_PRINTING(beingTracked, std::cout<<"subarrayID:"<<subarrayID<<":odd index"<<std::endl);


				assert(job3==NULL or (outJobTypeOfRunningJob!=A_NEW_JOB_ASSIGN_TO_JOB3_AND_QUEUED or job3->getCurIndex()<job3->maxLength));
				if(compFlagEqual==true){
					float val1=castNoLoss<REGISTER_TYPE,float>(job1->readFromBuffer());
					float val2=castNoLoss<REGISTER_TYPE,float>(job2->readFromBuffer());
					if(outJobTypeOfRunningJob==A_NEW_JOB_ASSIGN_TO_JOB3_AND_QUEUED)
					{
						job3->writeToBuffer(castNoLoss<REGISTER_TYPE,float>(val1)+castNoLoss<REGISTER_TYPE,float>(val2));
						job3->incAndCheckForWaitCycle(1);
					}
					job1->incAndCheckForWaitCycle(1);
					job2->incAndCheckForWaitCycle(1);

				}else{
					if(compFlagLess==true){
						float val1=castNoLoss<REGISTER_TYPE,float>(job1->readFromBuffer());

						//check that always means the first one is less than the second
						if(outJobTypeOfRunningJob==A_NEW_JOB_ASSIGN_TO_JOB3_AND_QUEUED)
						{
							job3->writeToBuffer(castNoLoss<REGISTER_TYPE,float>(val1));
							job3->incAndCheckForWaitCycle(1);
						}
						job1->incAndCheckForWaitCycle(1);

					}else{
						if(compFlagGreat==true){
							assert(job2->runningALPU==this);

							float val2=castNoLoss<REGISTER_TYPE,float>(job2->readFromBuffer());
							if(outJobTypeOfRunningJob==A_NEW_JOB_ASSIGN_TO_JOB3_AND_QUEUED)
							{
								job3->writeToBuffer(castNoLoss<REGISTER_TYPE,float>(val2));
								job3->incAndCheckForWaitCycle(1);
							}
							job2->incAndCheckForWaitCycle(1);

						}
					}
				}
				even=true;
			}//end of walker even indexes, value maintaining

	}else{
		//assert(outJobTypeOfRunningJob!=A_NEW_JOB_ASSIGN_TO_JOB3_AND_QUEUED or job3->getCurIndex()<job3->maxLength);
		if(job1!=NULL and isProcessing and job1->singleWalker==true)
		{
			TRACK_PRINTING(beingTracked, std::cout<<"subarrayID:"<<subarrayID<<":*******************this single walker job is being processed"<<std::endl);
			//TODO: seperate index and multiplication for energy modeling later
			if(outJobTypeOfRunningJob==A_NEW_JOB_ASSIGN_TO_JOB3_AND_QUEUED)
			{

				assert(job1->getCurIndex()<job1->maxLength);
				job3->writeToBuffer(job1->readFromBuffer());
				job3->incAndCheckForWaitCycle(1);
			}

			job1->incAndCheckForWaitCycle(1);
			TRACK_PRINTING(beingTracked, std::cout<<"subarrayID:"<<subarrayID<<",job1->getCurIndex():"<<job1->getCurIndex()<<", job1->length:"<<job1->length<<std::endl);

			if(job1->getCurIndex()>=job1->length  ){

				if(outJobTypeOfRunningJob==A_NEW_JOB_ASSIGN_TO_JOB3_AND_QUEUED)
				{
					job3->length=job3->getCurIndex();
					assert(job3->length<=job3->maxLength);
					justWaitingForTheLastWrite=true;
					job3->writejobDone=true;
					job3->RowLoadWriteIsNeeded=true;
				}else{
					isProcessing=false;
				}

				clearAReadBuffer(job1);

				TRACK_PRINTING(beingTracked, std::cout<<"subarrayID:"<<subarrayID<<":*******************this single walker job is done"<<std::endl);
			}

		}else{
			ASSERT_EX(!isProcessing, std::cout<<"subarrayID:"<<subarrayID<<": unsuporrted situation"<<std::endl);
			ASSERT_EX(!isProcessing, std::cout<<"isProcessing="<<std::boolalpha<<isProcessing<<", job1="<<job1 <<", job2="<<job2<<std::endl);
			TRACK_PRINTING(job1!=NULL and isProcessing, std::cout<<"job1->singleWalker="<<std::boolalpha<<job1->singleWalker<<std::endl);
			TRACK_PRINTING(job2!=NULL and isProcessing, std::cout<<"job2->singleWalker="<<std::boolalpha<<job2->singleWalker<<std::endl);

		}
	}
	TRACK_PRINTING(beingTracked, std::cout<<"subarrayID:"<<subarrayID<<":-----------------------end of function call"<<std::endl);
}
/*
bool computSubArray::isALPU_Free(){

	TRACK_PRINTING(beingTracked , std::cout<<"subarrayID:"<<subarrayID<<":isProcessing:"<<std::boolalpha <<isProcessing<<std::endl);

	if(isProcessing)
	{
		return false;
	}else{
		return true;
	}
}
*/
void computSubArray::AssignAjobButNotStart(walkerDataAndSetting * tJob, int jobNumber){
	tJob->initialSettingForReadBuffer(this);

	switch (jobNumber){
	case 1:{
		job1=tJob;
		break;
	}
	case 2:{
		job2=tJob;
		break;
	}
	case 3:{
		job3=tJob;
		break;

	}
	default:
		job1=tJob;
		break;
	}
}
void computSubArray::assignAndSrartJobOnALPU(walkerDataAndSetting * tJob1,walkerDataAndSetting * tJob2, outPutofAssignedJobTypeEnum outJobTypeOfRunningJob_t ){
	if(beingTracked){
		TRACK_PRINTING(beingTracked , std::cout<<"*****************A new job is being assigned************"<<std::endl);
	}
	//assert(isALPU_Free());
	assert(tJob1!=NULL);
	int maxLengthForJob3=tJob1->length;
	job1=tJob1;
	tJob1->initialSettingForReadBuffer(this);
	assert(tJob1==NULL or tJob1!=tJob2);
	if(tJob2!=NULL){

		job2=tJob2;
		tJob2->initialSettingForReadBuffer(this);
	}
	ASSERT_EX(job1->singleWalker==true or  job2!=NULL, std::cout <<"a two walker is job is satring but job2 is null job2= "<<job2<<std::endl );
	if(job2!=NULL){ //tJob2 might be null but job2 might not because it is boadcasted or set before

		maxLengthForJob3+=job2->length;
	}
	//TODO: generate code for two walker output
	if(outJobTypeOfRunningJob_t==A_NEW_JOB_ASSIGN_TO_JOB3_AND_QUEUED)
	{
		//TODO: implement other types of new jobs
		bool doubleWalker=false;
		assert(maxLengthForJob3>=0);
		TRACK_PRINTING(beingTracked, std::cout<<"subarrayID:"<<subarrayID<<":maxLengthForJob3:"<<maxLengthForJob3<<std::endl);
		assert(job3==NULL);
		job3=new walkerDataAndSetting(maxLengthForJob3, doubleWalker, SHOULD_LOADED_FROM_QUEUE );
		assert(job3!=NULL);
		TRACK_PRINTING(beingTracked, std::cout<<"subarrayID:"<<subarrayID<<":sucessfull malloc"<<maxLengthForJob3<<std::endl);
		job3->initialSettingForWriteBuffer(this);

	}
	isProcessing=true;
	even=true;
	justWaitingForTheLastWrite=false;
	outJobTypeOfRunningJob=outJobTypeOfRunningJob_t;
	TRACK_PRINTING(beingTracked , std::cout<<"subarrayID:"<<subarrayID<<":job is assigned: isProcessing="<<std::boolalpha <<isProcessing<<std::endl);
	TRACK_PRINTING(beingTracked , std::cout<<"job1:"<<job1<<"length"<<job1->length<<std::endl);

	assert(tJob1==NULL or tJob1!=tJob2);
	return;

}

void computSubArray::clearAReadBuffer(walkerDataAndSetting* &tJob){
	TRACK_PRINTING(beingTracked , std::cout<<"tJob->runningALPU->subarrayID:"<<tJob->runningALPU->subarrayID<<",tJob->isAPersistentJob="<<std::boolalpha<<tJob->isAPersistentJob<<std::endl);
	ASSERT_EX(tJob!=NULL, std::cout<<"subarrayID"<<subarrayID<<std::endl);
	if(tJob->isAPersistentJob==true){
		TRACK_PRINTING(beingTracked , std::cout<<"tJob->runningALPU->subarrayID:"<<tJob->runningALPU->subarrayID<<":A persistent job is cleared, tJob->isAPersistentJob="<<std::boolalpha<<tJob->isAPersistentJob<<std::endl);
		tJob->initialSettingForReadBuffer(this);
	}else{
		TRACK_PRINTING(beingTracked , std::cout<<"tJob->runningALPU->subarrayID:"<<tJob->runningALPU->subarrayID<<":A non-persistent job is being deleted, tJob->isAPersistentJob="<<std::boolalpha<<tJob->isAPersistentJob<<std::endl);
		delete(tJob);
		tJob=NULL;
	}
	return;

}
void computSubArray::clearAWriteBufferTobeQueued(walkerDataAndSetting*&tJob){
	ASSERT_EX(tJob!=NULL, std::cout<<"subarrayID"<<subarrayID<<std::endl);
	tJob=NULL;

}

bool computSubArray::isBusy() {
	return (isProcessing or isTranferring);
}

bool computSubArray::isFree() {
	return !isBusy();
}
