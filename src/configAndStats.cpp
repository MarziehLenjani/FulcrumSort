//
///*
// * configAndStats.cpp
// *
// *  Created on: Jan 29, 2020
// *      Author: ml2au
// */
//#include "configAndStats.hpp"
//
//#include <stdio.h>
//#include <stdlib.h>
//#include <iostream>
//#include <boost/program_options.hpp>
//#include <fstream>
//#include <sstream>
//#include <atomic>
//#include <map>
//#include <string>
//#include <assert.h>
//namespace po = boost::program_options;
//
//void configAndStats::parseOptions(int argc, char **argv) {
//    desc.add_options()   ("help", "produce help message")
//			(CONF_TEST_NUMBER_NAME,po::value<CONF_TEST_NUMBER_TYPE>(&testNumber)->default_value(0), CONF_TEST_NUMBER_NAME) //TODO:
//			(CONF_NUMBER_OF_LAYERS_NAME,po::value<CONF_NUMBER_OF_LAYERS_TYPE>(&numLayer)->default_value(8), "number of layers in a 3d stack memory")
//			(CONF_NUMBER_OF_BANKS_PER_LAYER_NAME,po::value<CONF_NUMBER_OF_BANKS_PER_LAYER_TYPE>(&numBankInLayer)->default_value(64), "number of banks in a layer")
//			(CONF_NUMBER_OF_COMPUTE_SUBARRAY_PER_BANK_NAME,po::value<CONF_NUMBER_OF_COMPUTE_SUBARRAY_PER_BANK_TYPE>(&numComputeSubarrayInABank)->default_value(16), "number of compute subarrays  in a bank")
//			//(CONF_NUMBER_OF_LAYERS_NAME,po::value<CONF_NUMBER_OF_LAYERS_TYPE>(&numLayer)->default_value(1), "number of layers in a 3d stack memory")
//			//(CONF_NUMBER_OF_BANKS_PER_LAYER_NAME,po::value<CONF_NUMBER_OF_BANKS_PER_LAYER_TYPE>(&numBankInLayer)->default_value(1), "number of banks in a layer")
//			//(CONF_NUMBER_OF_COMPUTE_SUBARRAY_PER_BANK_NAME,po::value<CONF_NUMBER_OF_COMPUTE_SUBARRAY_PER_BANK_TYPE>(&numComputeSubarrayInABank)->default_value(2), "number of compute subarrays  in a bank")
//			(CONF_SIZE_OF_EACH_SUBARRAY_IN_BYTE_NAME,po::value<CONF_SIZE_OF_EACH_SUBARRAY_IN_BYTE_TYPE>(&sizeOfEachSubarrayInByte)->default_value(1048576), "size of each subarrray in bytes, default 1 MB")
//			(CONF_ADDRESS_MAPPING_NAME,po::value< CONF_ADDRESS_MAPPING_TYPE >(& addressMapping)->default_value(CONF_ADDRESS_MAPPING_TYPE("MLBS")), "address mapping type: default memory ID,  layer ID, Bank ID, subarray ID")
//			(CONF_NUMBER_OF_WORDS_IN_A_ROW_NAME,po::value< CONF_NUMBER_OF_WORDS_IN_A_ROW_TYPE >(& numberOfWordsInARow)->default_value(64), "number of words in a row")
//			(CONF_ROW_CYCLE_IN_SUB_CLOCK_CYCLE_NAME,po::value< CONF_ROW_CYCLE_IN_SUB_CLOCK_CYCLE_TYPE >(& rowCycleInSubClockCyle)->default_value(63), "row cycles in sub clock cycles")
//			(CONF_SATA_FILE_NAME,po::value< std::string >(& statFileName)->default_value(std::string("stat.csv")), CONF_SATA_FILE_NAME);
//
//
//    po::store(po::parse_command_line(argc, argv, desc), vm);
//			po::notify(vm);
//			//TODO: original place for notify was here
//}
//
//
//void configAndStats::addValueToStatVector(std::string statName, std::string statValueInString){
//	outputStatVector.push_back(std::make_pair (statName,statValueInString));
//}
//
//
//std::string configAndStats::toStringScientific(float value) {
//  std::stringstream out;
//  out << std::scientific;
//  out << value;
//  return out.str();
//}
//void configAndStats::printStatsAndConfig( bool header, bool streamOut){
//
//
//	std::ostringstream strToPrint;
//
//
//	if(streamOut)
//	{
//
//		int i=0;
//		for(const auto& statPair: outputStatVector) {
//
//			strToPrint << statPair.first <<":" <<statPair.second<<std::endl ;
//			i++;
//		}
//
//		std::cout<<strToPrint.str();
//
//	}else{
//
//
//
//		if(header){
//			statfile.open (statFileName);
//			if(!statfile.is_open()){
//					std::cout << "error: ****** could not open the file:"<<statFileName<<std::endl ;
//			}else{
//					std::cout << "openning this file:"<<statFileName<<std::endl<<"for writing header" ;
//			}
//			int i=0;
//			for(const auto& statPair: outputStatVector) {
//				if(i!=0){
//					strToPrint  << "," ;
//				}
//				strToPrint << statPair.first  ;
//				i++;
//			}
//			strToPrint << std::endl ;
//		}else{
//			int i=0;
//			if(!statfile.is_open()){
//					std::cout << "error: ****** could not open the file:"<<statFileName<<std::endl ;
//			}else{
//					std::cout << "openning this file:"<<statFileName<<std::endl ;
//			}
//
//			for(const auto& statPair: outputStatVector) {
//				if(i!=0){
//					strToPrint <<",";
//				}
//				strToPrint <<statPair.second;
//				i++;
//			}
//			strToPrint << std::endl ;
//		}
//
//		std::cout << "Writing this to a file------------:"<<std::endl<< strToPrint.str()<<std::endl;
//
//		bool good=(bool) (statfile<<strToPrint.str());
//		if(!good){
//			std::cout << "error: ****** could not write to  the file: statFileName"<<statFileName<<std::endl ;
//
//			exit(1);
//		}
//	}
//
//}
//configAndStats::configAndStats():desc("Allowed options") {
//	//TODO: do your initializations here
//
//}
//void configAndStats::closeStatFile() {
//	if(statfile.is_open()){
//		statfile.flush();
//		statfile.close();
//	}
//	if(statfile.is_open()){
//		std::cout << "error: ****** could not close the file: statFileName"<<statFileName<<std::endl ;
//	}
//}
//configAndStats::~configAndStats() {
//	closeStatFile();
//
//}
//
//
