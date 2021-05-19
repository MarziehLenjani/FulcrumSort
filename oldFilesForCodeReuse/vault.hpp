#ifndef VAULT_HPP
#define VAULT_HPP
#include "types.hpp"
//#include "computSubArray.hpp"
#include <vector>
//#include "logicLayer.hpp"
class computSubArray;
class walkerDataAndSetting;
class logicLayer;
class configAndStats;
class transferMedium;
class vault{
    public:
	    configAndStats* configObjPointer=NULL;
		int numLayer=0;
		int numBankPerVaultLayer=0;
		int numComSubPerBansk=0;
		int totalSubArrayCompute=0;
		std::vector<computSubArray *> compSubVector;
		//vault(int memoryArraySize=0);
		vault(configAndStats* configObjPointer_t);
		~vault();
		void queuJob(walkerDataAndSetting* jb1, int queuID);
		void restIndexes();
		void restData();
		bool allFree();
		static int vaultID;
		int maxQueuLength=8;
		logicLayer * logicLayerObj;
		bool proceed(outPutofAssignedJobTypeEnum outJobType, bool broadCastALPU_settings);
		//bool isALPU_Free(int ALPU_ID);
		void assignAndSrartJobOnALPU(walkerDataAndSetting * tJob1,walkerDataAndSetting * tJob2, int ALPU_ID, outPutofAssignedJobTypeEnum outJobType);
		walkerDataAndSetting * returnOutPuJob(int ALPU_ID);
		void setSubArrayTrackID(int t_subArrayTrackID);
		void setQueueTrackID(int t_queueTrackID);
		void brdCastjobButNotStart(walkerDataAndSetting * tJob,  int jobNumber,bool isAPersistentJob_t, bool sharedMemoryWalkerValues_t);
		transferMedium* tsv;
    private:
		int subArrayTrackID=-1;
		int queueTrackID=-1;

};

#endif
