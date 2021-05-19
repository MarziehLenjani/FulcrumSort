#include <stdio.h>
#include <iostream>
#include "configAndStats.hpp"
#include "physicalComponent.hpp"
#include "stackedMemory.hpp"
#include "statConfigNamesAndTypes.hpp"
#include "dataPartitioning.hpp"


//#ifdef USE_THE_MAIN_MAIN
int main(int argc, char **argv)
{

	configAndStats confObj;
	confObj.parseOptions(argc, argv);

	//----------------testing config and stat object

	std::cout<<"testNumber is "<<confObj.getConfig< CONF_TEST_NUMBER_TYPE>(std::string(CONF_TEST_NUMBER_NAME))<<std::endl;
	long long int clockCycle= 100000;
	float energyConsumption= 0.998;
	confObj.addValueToStatVector("clockCycle",std::to_string(clockCycle));
	confObj.addValueToStatVector("energyInNJ",confObj.toStringScientific(energyConsumption));
	confObj.printStatsAndConfig(false,true );
	confObj.printStatsAndConfig(true,false );
	confObj.printStatsAndConfig(false,false );
	//----------------------building a 3d stack component
	stackedMemory stackedMemoryObj((ID_TYPE)0,&confObj, NULL, NULL, NULL);
	int maxNumClockCycle=1000;
	int colkCycleCounter=0;
	while(!stackedMemoryObj.endOfProcessing){
		stackedMemoryObj.runOneSubClokCycle();
		colkCycleCounter++;
		if(colkCycleCounter>maxNumClockCycle){
			stackedMemoryObj.endOfProcessing=true;
		}
	}
	//-----------testing data partitioning APIS
	dataPartitioning dataPartitioningObj(&stackedMemoryObj,&confObj);
#ifdef PHYSICAL_EXTERNAL_ADDRESS_TYPE_UNSIGHED_LONG_INT
	PHYSICAL_EXTERNAL_ADDRESS_TYPE address=0xFFFFFFFFFFFFFFFF;
#else
	PHYSICAL_EXTERNAL_ADDRESS_TYPE address=0xFFFFFFFF;
#endif
	ID_TYPE stackId;
	ID_TYPE layerId;
	ID_TYPE bankId;
	ID_TYPE computeSubarrayID;
	dataPartitioningObj.globalAddressToLocalAddressTranslation( address,  stackId, layerId, bankId,  computeSubarrayID);
	std::cout<<"address:"<<address<<std::endl;
	std::cout<<"stackId:"<<stackId<<std::endl;
	std::cout<<"layerId:"<<layerId<<std::endl;
	std::cout<<"bankId:"<<bankId<<std::endl;
	std::cout<<"computeSubarrayID:"<<computeSubarrayID<<std::endl;

	///---------------testing broadcasting
	{
		LOCAL_ADDRESS_TYPE DataAddress= (LOCAL_ADDRESS_TYPE) 64;
		bool writeMetadat=true;
		LOCAL_ADDRESS_TYPE AddressOfTheStartAddress= 0;
		LOCAL_ADDRESS_TYPE AddressOfTheEndAdddress=4;
		READ_DATA_TYPE_IN_MEMORY_ARRAY broadcastedData[8]={0x01,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
		LOCAL_ADDRESS_TYPE sizeOFData=8;
		ERROR_RETURN_TYPE ret= dataPartitioningObj.broadcastDataToAllComputeSubArray (DataAddress, writeMetadat,AddressOfTheStartAddress,AddressOfTheEndAdddress,  broadcastedData,  sizeOFData );
		stackedMemoryObj.computSubarrayVector[0]->memoryArrayObj->printData(DataAddress, sizeOFData);
		stackedMemoryObj.computSubarrayVector[0]->memoryArrayObj->printData(AddressOfTheStartAddress, sizeof (LOCAL_ADDRESS_TYPE));
		stackedMemoryObj.computSubarrayVector[0]->memoryArrayObj->printData(AddressOfTheEndAdddress, sizeof (LOCAL_ADDRESS_TYPE));
	}

	//---------------testing random generate and partitioning among all subarrays
	{
		int minRand=0;
		int maxRand=100;
		int seed=56;
		LOCAL_ADDRESS_TYPE DataAddress= (LOCAL_ADDRESS_TYPE) 64;
		bool writeMetadat=true;
		LOCAL_ADDRESS_TYPE AddressOfTheStartAddress= 0;
		LOCAL_ADDRESS_TYPE AddressOfTheEndAdddress=4;
		LOCAL_ADDRESS_TYPE numOFDataElements =stackedMemoryObj.totNumComputeSubarray*10+7; // to test unequal partitions
		std::cout<<"stackedMemoryObj.totNumComputeSubarray:"<<stackedMemoryObj.totNumComputeSubarray<<std::endl;
		ERROR_RETURN_TYPE ret=dataPartitioningObj.generateRandomlyAndPartitionEquallyAmongAllComputeSubArray (minRand, maxRand, seed, DataAddress, writeMetadat, AddressOfTheStartAddress, AddressOfTheEndAdddress, numOFDataElements );
		LOCAL_ADDRESS_TYPE sizeOFFirstPartition=std::ceil(numOFDataElements*sizeof(int)/stackedMemoryObj.totNumComputeSubarray);
		stackedMemoryObj.computSubarrayVector[0]->memoryArrayObj->printData(DataAddress, sizeOFFirstPartition);
		stackedMemoryObj.computSubarrayVector[0]->memoryArrayObj->printData(AddressOfTheStartAddress, sizeof (LOCAL_ADDRESS_TYPE));
		stackedMemoryObj.computSubarrayVector[0]->memoryArrayObj->printData(AddressOfTheEndAdddress, sizeof (LOCAL_ADDRESS_TYPE));


	}
	//test readData API in partitioning class
	{
		LOCAL_ADDRESS_TYPE DataAddress;
		bool readByMetadat=true;
		 bool printData=true;
		 LOCAL_ADDRESS_TYPE AddressOfTheStartAddress=0;
		 LOCAL_ADDRESS_TYPE AddressOfTheEndAdddress=4;
		 READ_DATA_TYPE_IN_MEMORY_ARRAY* readData;
		 READ_DATA_TYPE_IN_MEMORY_ARRAY** pointerToReadDataPointer=&readData;
		 LOCAL_ADDRESS_TYPE sizeOFData;
		 ID_TYPE layerId=0;
		 ID_TYPE bankID=0;
		 ID_TYPE computeSubArrayID=0;
		ERROR_RETURN_TYPE ret=dataPartitioningObj.readData(DataAddress, readByMetadat,  printData,  AddressOfTheStartAddress,AddressOfTheEndAdddress, pointerToReadDataPointer, sizeOFData, layerId, bankID, computeSubArrayID);

	}
}
//#endif
