///*
// * config.hpp
// *
// *  Created on: Jan 29, 2020
// *      Author: ml2au
// */
//#ifndef CONFIG_AND_STATS_HPP
//#define CONFIG_AND_STATS_HPP
//
//
//#include <boost/program_options.hpp>
//#include <fstream>
//#include <iostream>
//#include <boost/assign/list_of.hpp>
//#include <cassert>
//
//#include "statConfigNamesAndTypes.hpp"
//namespace po = boost::program_options;
//using namespace std;
//class configAndStats {
//public:
//	//--------------------stat config vector part
//	std::vector<std::pair<std::string,std::string>>outputStatVector; //This is an object, added for where we need to print stats in a specific order
//	void addValueToStatVector(std::string statName, std::string statValueInString);
//	//--------------------stat config map part
//	std::map<std::string,std::pair<std::string,boost::any>>statConfigMap;
//	//-------------------
//	//the template functions should be defined in the header file to avoid load error: https://bytefreaks.net/programming-2/c/c-undefined-reference-to-templated-class-function
//
//	template<typename T> void addValueToStatConfigMap(std::string statConfigName, std::string statType,T statValue){
//		statConfigMap[statConfigName]=std::make_pair(statType,statValue);
//	}
//	template<typename T> boost::any * getAddressFromStatMap(std::string statConfigName, std::string statType){
//		if (statConfigMap.find(statConfigName) == statConfigMap.end()){
//			T temp;
//			statConfigMap[statConfigName]=std::make_pair(statType,temp);
//		}
//		return &statConfigMap[statConfigName].second;
//	}
//
//	template<typename T> std::string getValueInStringFromStatMap(std::string statConfigName){
//
//		return std::to_string(boost::any_cast<T>(statConfigMap[statConfigName].second));
//
//	}
//
//
//	//-------------- config variables
//	CONF_TEST_NUMBER_TYPE testNumber ;
//	std::string statFileName;
//	CONF_NUMBER_OF_LAYERS_TYPE numLayer;
//	CONF_NUMBER_OF_BANKS_PER_LAYER_TYPE numBankInLayer;
//	CONF_NUMBER_OF_COMPUTE_SUBARRAY_PER_BANK_TYPE numComputeSubarrayInABank;
//	CONF_SIZE_OF_EACH_SUBARRAY_IN_BYTE_TYPE sizeOfEachSubarrayInByte;
//	CONF_ADDRESS_MAPPING_TYPE addressMapping;
//    CONF_NUMBER_OF_WORDS_IN_A_ROW_TYPE numberOfWordsInARow;
//    CONF_ROW_CYCLE_IN_SUB_CLOCK_CYCLE_TYPE rowCycleInSubClockCyle;
//
//
//
//
//	//--------------------------
//	po::options_description desc;
//	po::variables_map vm;
//	void parseOptions(int argc, char **argv);
//	void printConfig();
//
//	template<typename T>  T getConfig(std::string configName) {
//		//TODO: check the right place for notify
//		po::notify(vm);
//		bool found=false;
//	    for (const auto& it : vm) {
//
//	    	if(configName.compare(it.first.c_str() )== 0){
//	    		found =true;
//	    		auto& value = it.second.value();
//	    		if (auto v = boost::any_cast<T>(&value)){
//	    			return *v;
//	    		}else{
//	    			   std::cout<<__FILE__<<":"<<__LINE__<<":Assert message:"<<"missmatch in requested configuration type"<<std::endl;
//	    			    assert(false);
//	    		}
//	    	}
//	    }
//	    std::cout<<__FILE__<<":"<<__LINE__<<":Assert message:"<<"requested configuration not found"<<std::endl;
//	    assert(found==true);
//	    return 0;
//
//	}
//	std::string toStringScientific(float value);
//	configAndStats();
//	void printStatsAndConfig( bool header, bool streamOut );
//	ofstream statfile;
//
//
//	void closeStatFile(void);
//
//	  ~configAndStats();
//};
//
//#endif /* CONFIG_AND_STATS_HPP*/
