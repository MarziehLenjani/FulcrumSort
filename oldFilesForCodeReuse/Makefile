SHELL := /bin/bash
#set -e
INCLUDES  := -I../../../common/inc -I$(BOOST_INCLUDEDIR)
LIBRARIES := -L$(BOOST_LIBRARYDIR)
SRC_DIR := ./src
OBJ_DIR := ./obj
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS := $(LIBRARIES) -lboost_system -lboost_program_options
CPPFLAGS := -g -Wall -std=c++11  $(INCLUDES) 
CXXFLAGS :=
TEST_EXE_NAME= ./main.out
#TEST_EXE_NAME=valgrind  --leak-check=yes ./main.out
#TEST_EXE_NAME=./bin/main.out
all: build
.PHONY: clean test4 tst12 release loadModules rmPrevMain
testEcdcho:
	echo $(OBJ_FILES) 
	echo $(sSRC_DIR)

main.out: $(OBJ_FILES) 
	g++ $(LDFLAGS) -o $@ $^
	
loadModules:
	bash -c "module load boost"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp loadModules
	g++ $(CPPFLAGS)  $(CXXFLAGS) -fmax-errors=5 -c -o $@ $<
build: main.out
########################Main program
test0:
	$(TEST_EXE_NAME) --testNumber=0
#################################
test1:
	$(TEST_EXE_NAME) --numColA=10000 --numRowA=256 --percentageM=0.1 --percentageV=0.1 --trackSubarrayID=1  --sparseIntervalM=4 --sparseIntervalV=2 --computeType="BC"
test2:
	$(TEST_EXE_NAME) --numColA=10000 --numRowA=256 --percentageM=0.1 --percentageV=0.1 --trackSubarrayID=1  --sparseIntervalM=4 --sparseIntervalV=2 --computeType="BCWait"
test3:
	$(TEST_EXE_NAME) --numColA=10000 --numRowA=256 --percentageM=0.1 --percentageV=0.1 --trackSubarrayID=1  --sparseIntervalM=4 --sparseIntervalV=2 --computeType="BC_STORE" 
test4:
	$(TEST_EXE_NAME) --numColA=10000 --numRowA=256 --percentageM=0.1 --percentageV=0.1 --trackSubarrayID=1  --sparseIntervalM=4 --sparseIntervalV=2 --computeType="BC_MERGE_LAYOUT"
test5:
	$(TEST_EXE_NAME) --numColA=10000 --numRowA=256 --percentageM=0.1 --percentageV=0.1 --trackSubarrayID=1   --computeType="BC_STORE"
test6:
	$(TEST_EXE_NAME) --numColA=10000 --numRowA=256 --percentageM=1 --percentageV=1 --trackSubarrayID=1  --computeType="BC_STORE" 
test7:
	$(TEST_EXE_NAME) --numColA=10000 --numRowA=256 --percentageM=0.1 --percentageV=0.1 --trackSubarrayID=1   --computeType="BCWait"
test8:
	$(TEST_EXE_NAME) --numColA=10000 --numRowA=256 --percentageM=0.01 --percentageV=0.01 --trackSubarrayID=1   --computeType="BC_STORE"
#######################SAME Config, different stategy
test9:
	$(TEST_EXE_NAME) --numColA=10000 --numRowA=256 --percentageM=0.01 --percentageV=0.1 --trackSubarrayID=1   --computeType="BC"
test10:
	$(TEST_EXE_NAME) --numColA=10000 --numRowA=256 --percentageM=0.01 --percentageV=0.1 --trackSubarrayID=1   --computeType="BCWait"
test11:
	$(TEST_EXE_NAME) --numColA=10000 --numRowA=256 --percentageM=0.01 --percentageV=0.1 --trackSubarrayID=1   --computeType="BC_STORE"
test12_1:
	$(TEST_EXE_NAME) --numColA=10000 --numRowA=256 --percentageM=0.01 --percentageV=0.1 --trackSubarrayID=1   --computeType="BC_MERGE_LAYOUT" --modelInterSubArrayMovement=false
test12_2:
	$(TEST_EXE_NAME) --numColA=10000 --numRowA=256 --percentageM=0.01 --percentageV=0.1 --trackSubarrayID=1   --computeType="BC_MERGE_LAYOUT" --modelInterSubArrayMovement=true


#############################	Test Modules
test13:
	$(TEST_EXE_NAME) test16
test14:
	$(TEST_EXE_NAME) --testNumber=14
test15:
	$(TEST_EXE_NAME) --testNumber=15
test16:
	$(TEST_EXE_NAME) --testNumber=16
test17: #testOptionmodification
	$(TEST_EXE_NAME) --testNumber=17	
test18: #test map function 
	$(TEST_EXE_NAME) --testNumber=18	
test19: #test map function 
	$(TEST_EXE_NAME) --testNumber=19	
##############
test20: 
	$(TEST_EXE_NAME) --testNumber=20
test21: 
	$(TEST_EXE_NAME) --testNumber=21
#################Starting testing test0 with different parameters
test30:
	$(TEST_EXE_NAME) --testNumber=0 --numColA=10000 --numRowA=256 --percentageM=0.1 --percentageV=0.1 --computeType="BC" --statFileName="statTest30.csv"
test31:
	$(TEST_EXE_NAME) --testNumber=0 --numColA=10000 --numRowA=256 --percentageM=0.1 --percentageV=0.1 --computeType="BCWait" --statFileName="statTest31.csv"
test32:
	$(TEST_EXE_NAME) --testNumber=0 --numColA=10000 --numRowA=256 --percentageM=0.1 --percentageV=0.1 --computeType="BC_STORE" --statFileName="statTest32.csv"
test33:
	$(TEST_EXE_NAME) --testNumber=0 --numColA=10000 --numRowA=256 --percentageM=0.1 --percentageV=0.1 --computeType="BC_MERGE_LAYOUT" --statFileName="statTest33.csv"
#34:testInterSubArrayTesting
test34:
	$(TEST_EXE_NAME) --testNumber=34 
test35:
	$(TEST_EXE_NAME) --testNumber=0 --numColA=10000000 --numRowA=1000 --percentageM=0.1 --percentageV=0.1 --computeType="BC_MERGE_LAYOUT" --statFileName="statTest35.csv"
##########Manualy testing the correctness of real matrix files
test36_1:
	$(TEST_EXE_NAME) --testNumber=36 --matrixFileName="/zf14/ml2au/SyncWithGit/dataNotSynced/extarctedMatrixFiles/fp/fp.rb" --sparseFileFormatType="RB"		--statFileName="statTest36_1.csv"
test36_2:
	$(TEST_EXE_NAME) --testNumber=36 --matrixFileName="/zf14/ml2au/SyncWithGit/dataNotSynced/ManuallySlectedSmallForTest/ASIC_100ks/ASIC_100ks.rb" --sparseFileFormatType="RB"		--statFileName="statTest36_2.csv" 
test36_100:
	$(TEST_EXE_NAME) --testNumber=36 --matrixFileName="/zf14/ml2au/SyncWithGit/dataNotSynced/ManuallySlectedSmallForTest/ASIC_100ks/ASIC_100ks.rb" --sparseFileFormatType="RB"		--statFileName="statTest36_100.csv" 

test36_1000:
	$(TEST_EXE_NAME) --testNumber=36 --matrixFileName="/zf14/ml2au/SyncWithGit/dataNotSynced/ManuallySelected/kmer_V1r/kmer_V1r/kmer_V1r.rb" --sparseFileFormatType="RB"		--statFileName="statTest36_3.csv" 
############################
test37:
	$(TEST_EXE_NAME) --testNumber=0 --percentageV=0.1 --computeType="BC_MERGE_LAYOUT" --statFileName="statTest37.csv"  --matrixFileName="/zf14/ml2au/SyncWithGit/dataNotSynced/extarctedMatrixFiles/fp.rb" --sparseFileFormatType="RB"		

test38:
	$(TEST_EXE_NAME)  --computeType=BC  --matrixFileName=/zf14/ml2au/SyncWithGit/dataNotSynced/extarctedMatrixFiles/TF17/TF17.rb  --sparseFileFormatType=RB  --percentageV=0.001  --statFileName=/zf14/ml2au/summaryResults/sparsAwareFulcrumResult/SPMSV_realMatrixes/TF17_TF17_vperc_0010000000_BC.csv  --testNumber=0
test39:
	$(TEST_EXE_NAME)  --computeType=BC_MERGE_LAYOUT  --matrixFileName=/zf14/ml2au/SyncWithGit/dataNotSynced/extarctedMatrixFiles/vsp_sctap1-2b_and_seymourl/vsp_sctap1-2b_and_seymourl.rb  --sparseFileFormatType=RB  --percentageV=0.1  --statFileName=/zf14/ml2au/summaryResults/sparsAwareFulcrumResult/SPMSV_realMatrixes/vsp_sctap1-2b_and_seymourl_vsp_sctap1-2b_and_seymourl_vperc_1000000000_BC_MERGE_LAYOUT.csv  --testNumber=0 
test40:
	$(TEST_EXE_NAME) computeType=BC_STORE --matrixFileName=/zf14/ml2au/SyncWithGit/dataNotSynced/extarctedMatrixFiles/vsp_sctap1-2b_and_seymourl/vsp_sctap1-2b_and_seymourl.rb --sparseFileFormatType=RB --percentageV=1e-05 --statFileName=/zf14/ml2au/summaryResults/sparsAwareFulcrumResult/SPMSV_realMatrixes/vsp_sctap1-2b_and_seymourl_vsp_sctap1-2b_and_seymourl_vperc_0000100000_BC_STORE.csv --testNumber=0
test41:
	$(TEST_EXE_NAME) --computeTypemk=BCWait  --matrixFileName=/zf14/ml2au/SyncWithGit/dataNotSynced/extarctedMatrixFiles/208bit/208bit.rb  --sparseFileFormatType=RB  --percentageV=0.1  --statFileName=/zf14/ml2au/summaryResults/sparsAwareFulcrumResult/SPMSV_realMatrixes/208bit_208bit_vperc_1000000000_BCWait.csv  --testNumber=0
test42:
	$(TEST_EXE_NAME) --computeType=BC_MERGE_LAYOUT  --matrixFileName=/zf14/ml2au/SyncWithGit/dataNotSynced/extarctedMatrixFiles/208bit/208bit.rb  --sparseFileFormatType=RB  --percentageV=0.1  --statFileName=/zf14/ml2au/summaryResults/sparsAwareFulcrumResult/SPMSV_realMatrixes/208bit_208bit_vperc_1000000000_BC_MERGE_LAYOUT.csv  --testNumber=0
test43_1:
	$(TEST_EXE_NAME) --computeType=BC  --matrixFileName=/zf14/ml2au/SyncWithGit/dataNotSynced/ManuallySlectedSmallForTest/ASIC_100ks/ASIC_100ks.rb  --sparseFileFormatType=RB  --percentageV=0.1  --statFileName=/zf14/ml2au/summaryResults/sparsAwareFulcrumResult/SPMSV_realMatrixes/ASIC_100k_ASIC_100ks_vperc_1000000000_BC.csv  --testNumber=0
test43_2:
	$(TEST_EXE_NAME) --computeType=BC_STORE  --matrixFileName=/zf14/ml2au/SyncWithGit/dataNotSynced/ManuallySlectedSmallForTest/ASIC_100ks/ASIC_100ks.rb  --sparseFileFormatType=RB  --percentageV=0.1  --statFileName=/zf14/ml2au/summaryResults/sparsAwareFulcrumResult/SPMSV_realMatrixes/ASIC_100k_ASIC_100ks_vperc_1000000000_BC_STORE.csv  --testNumber=0
test43_3:
	$(TEST_EXE_NAME) --computeType=BCWait  --matrixFileName=/zf14/ml2au/SyncWithGit/dataNotSynced/ManuallySlectedSmallForTest/ASIC_100ks/ASIC_100ks.rb  --sparseFileFormatType=RB  --percentageV=0.1  --statFileName=/zf14/ml2au/summaryResults/sparsAwareFulcrumResult/SPMSV_realMatrixes/ASIC_100k_ASIC_100ks_vperc_1000000000_BCWait.csv  --testNumber=0
test43_4:
	$(TEST_EXE_NAME) --computeType=BC_MERGE_LAYOUT  --matrixFileName=/zf14/ml2au/SyncWithGit/dataNotSynced/ManuallySlectedSmallForTest/ASIC_100ks/ASIC_100ks.rb  --sparseFileFormatType=RB  --percentageV=0.1  --statFileName=/zf14/ml2au/summaryResults/sparsAwareFulcrumResult/SPMSV_realMatrixes/ASIC_100k_ASIC_100ks_vperc_1000000000_BC_MERGE_LAYOUT.csv  --testNumber=0
test43_5:
	$(TEST_EXE_NAME) --computeType=BLOCK_DENSE  --matrixFileName=/zf14/ml2au/SyncWithGit/dataNotSynced/ManuallySlectedSmallForTest/ASIC_100ks/ASIC_100ks.rb  --sparseFileFormatType=RB  --percentageV=0.1  --statFileName=/zf14/ml2au/summaryResults/sparsAwareFulcrumResult/SPMSV_realMatrixes/ASIC_100k_ASIC_100ks_vperc_1000000000_BLOCK_DENSE.csv  --testNumber=0
test43_6:
	$(TEST_EXE_NAME) --computeType=CSR_FILTERED  --matrixFileName=/zf14/ml2au/SyncWithGit/dataNotSynced/ManuallySlectedSmallForTest/ASIC_100ks/ASIC_100ks.rb  --sparseFileFormatType=RB  --percentageV=0.1  --statFileName=/zf14/ml2au/summaryResults/sparsAwareFulcrumResult/SPMSV_realMatrixes/ASIC_100k_ASIC_100ks_vperc_1000000000_CSR_FILTERED.csv  --testNumber=0
test43_L_1:
	$(TEST_EXE_NAME) --computeType=CSR_FILTERED  --matrixFileName=/zf14/ml2au/SyncWithGit/dataNotSynced/ManuallySelected/DIMACS10/road_usa/road_usa.rb  --sparseFileFormatType=RB   --statFileName=statToDelet/statTest43_L_1.csv --testNumber=0
test43_L_1_b32:
	$(TEST_EXE_NAME) --computeType=CSR_FILTERED  --matrixFileName=/zf14/ml2au/SyncWithGit/dataNotSynced/ManuallySelected/DIMACS10/road_usa/road_usa.rb  --sparseFileFormatType=RB   --statFileName=statToDelet/statTest43_L_1.csv --testNumber=0 --blockDim1Size=32 --numSegments=8

#########################33

test44_S_1:
	$(TEST_EXE_NAME)  --statFileName="statToDelet/statTest44_S_1.csv" --matrixFileName="/zf14/ml2au/SyncWithGit/dataNotSynced/ManuallySlectedSmallForTest/ASIC_100ks/ASIC_100ks.rb" --sparseFileFormatType="RB" --testNumber=44
#1: 318968,23947347,23947347,-1,-1,1,5.03148e-08 short
test44_L_1:
	$(TEST_EXE_NAME)  --statFileName="statToDelet/statTest44_L_1.csv" --matrixFileName="/zf14/ml2au/SyncWithGit/dataNotSynced/ManuallySelected/Perc_8_road_usa/road_usa.rb" --sparseFileFormatType="RB" --testNumber=44
#2: 221138,18571154,18571154,-1,-1,1,5.51488e-08 short
test44_L_2:
	$(TEST_EXE_NAME)  --statFileName="statToDelet/statTest44_L_2.csv" --matrixFileName="/zf14/ml2au/SyncWithGit/dataNotSynced/ManuallySelected/Perc_8_mawi_201512012345/mawi_201512012345.rb" --sparseFileFormatType="RB" --testNumber=44
#3: 458751,16777216,16777216,-1,-1,1,1.78814e-07 short
test44_L_3:
	$(TEST_EXE_NAME)  --statFileName="statToDelet/statTest44_L_3.csv" --matrixFileName="/zf14/ml2au/SyncWithGit/dataNotSynced/ManuallySelected/Perc_7_delaunay_n24/delaunay_n24.rb" --sparseFileFormatType="RB" --testNumber=44
#4: 1101139,16777216,16777216,-1,-1,1,4.70938e-07, medium
test44_L_4:
	$(TEST_EXE_NAME)  --statFileName="statToDelet/statTest44_L_4.csv" --matrixFileName="/zf14/ml2au/SyncWithGit/dataNotSynced/ManuallySelected/Perc_7_rgg_n_2_24_s0/rgg_n_2_24_s0.rb" --sparseFileFormatType="RB" --testNumber=44
#5:			1877756,16240000,16240000,-1,-1,1,8.80545e-07									long
test44_L_5:
	$(TEST_EXE_NAME)  --statFileName="statToDelet/statTest44_L_5.csv" --matrixFileName="/zf14/ml2au/SyncWithGit/dataNotSynced/ManuallySelected/Perc_7_nlpkkt200/nlpkkt200.rb" --sparseFileFormatType="RB" --testNumber=44
#6: 187299,14081816,14081816,-1,-1,1,8.5394e-08 short
test44_L_6:
	$(TEST_EXE_NAME)  --statFileName="statToDelet/statTest44_L_6.csv" --matrixFileName="/zf14/ml2au/SyncWithGit/dataNotSynced/ManuallySelected/Perc_8_road_central/road_central.rb" --sparseFileFormatType="RB" --testNumber=44
#7:-1420501,11449533,11449533 #TODO:buggy probably negative memory requirement ???                   long
test44_L_7:
	$(TEST_EXE_NAME)  --statFileName="statToDelet/statTest44_L_7.csv" --matrixFileName="/zf14/ml2au/SyncWithGit/dataNotSynced/ManuallySelected/Perc_6_stokes/stokes.rb" --sparseFileFormatType="RB" --testNumber=44
#8: 133064,6815744,6815744,-1,-1,1,2.93284e-07 short
test44_L_8:
	$(TEST_EXE_NAME)  --statFileName="statToDelet/statTest44_L_8.csv" --matrixFileName="/zf14/ml2au/SyncWithGit/dataNotSynced/ManuallySelected/Perc_7_adaptive/adaptive.rb" --sparseFileFormatType="RB" --testNumber=44
#9:486745,5558326,5558326,-1,-1,1,1.92666e-06 short
test44_L_9:
	$(TEST_EXE_NAME)  --statFileName="statToDelet/statTest44_L_9.csv" --matrixFileName="/zf14/ml2au/SyncWithGit/dataNotSynced/ManuallySelected/Perc_6_circuit5M/circuit5M.rb" --sparseFileFormatType="RB" --testNumber=44
#10: 352206,4802000,4802000,-1,-1,1,1.85095e-06 short
test44_L_10:
	$(TEST_EXE_NAME)  --statFileName="statToDelet/statTest44_L_10.csv" --matrixFileName="/zf14/ml2au/SyncWithGit/dataNotSynced/ManuallySelected/Perc_6_channel-500x100x100-b050/channel-500x100x100-b050.rb" --sparseFileFormatType="RB" --testNumber=44
#11:78653,4026819,4026819,-1,-1,1,4.96707e-07,0.01 short
test44_L_11:
	$(TEST_EXE_NAME)  --statFileName="statToDelet/statTest44_L_11.csv" --matrixFileName="/zf14/ml2au/SyncWithGit/dataNotSynced/ManuallySelected/Perc_7_venturiLevel3/venturiLevel3.rb" --sparseFileFormatType="RB" --testNumber=44
#12:177041,4690002,4690002,-1,-1,1,9.23629e-07 short
test44_L_12:
	$(TEST_EXE_NAME)  --statFileName="statToDelet/statTest44_L_12.csv" --matrixFileName="/zf14/ml2au/SyncWithGit/dataNotSynced/ManuallySelected/Perc_7_rajat31/rajat31.rb" --sparseFileFormatType="RB" --testNumber=44
#795132,5154859,5154859,-1,-1,1,3.73316e-06, long
test44_L_13:
	$(TEST_EXE_NAME)  --statFileName="statToDelet/statTest44_L_13.csv" --matrixFileName="/zf14/ml2au/SyncWithGit/dataNotSynced/ManuallySelected/Perc_6_cage15/cage15.rb" --sparseFileFormatType="RB" --testNumber=44
#1545443,7414866,7414866,-1,-1,1,3.53053e-06   #long
test44_L_14:
	$(TEST_EXE_NAME)  --statFileName="statToDelet/statTest44_L_14.csv" --matrixFileName="/zf14/ml2au/SyncWithGit/dataNotSynced/ManuallySelected/Perc_6_indochina-2004/indochina-2004.rb" --sparseFileFormatType="RB" --testNumber=44
#672890,55042369,55042369,-1,-1,1,1.9345e-08
test44_L_15:
	$(TEST_EXE_NAME)  --statFileName="statToDelet/statTest44_L_15.csv" --matrixFileName="/zf14/ml2au/SyncWithGit/dataNotSynced/ManuallySelected/Perc_8_kmer_V2a/kmer_V2a.rb" --sparseFileFormatType="RB" --testNumber=44
#61297,109900,1977885,-1,-1,1,3.5843e-05
test44_L_16:
	$(TEST_EXE_NAME)  --statFileName="statToDelet/statTest45_L_16.csv" --matrixFileName="/zf14/ml2au/SyncWithGit/dataNotSynced/ManuallySelected/Perc_5_Rucci1/Rucci1.rb" --sparseFileFormatType="RB" --testNumber=44
#391968,98303,98303,-1,-1,1,0.00518683,
test44_L_17:
	$(TEST_EXE_NAME)  --statFileName="statToDelet/statTest45_L_17.csv" --matrixFileName="/zf14/ml2au/SyncWithGit/dataNotSynced/ManuallySelected/Perc_3_mycielskian17/mycielskian17.rb" --sparseFileFormatType="RB" --testNumber=44
#53403,62729,1748122,-1,-1,1,6.20503e-05
test44_L_18:
	$(TEST_EXE_NAME)  --statFileName="statToDelet/statTest45_L_18.csv" --matrixFileName="/zf14/ml2au/SyncWithGit/dataNotSynced/ManuallySelected/Perc_5_sls/sls.rb" --sparseFileFormatType="RB" --testNumber=44
#113505,45101,45101,-1,-1,1,0.00713151 #short
test44_L_19:
	$(TEST_EXE_NAME)  --statFileName="statToDelet/statTest45_L_19.csv" --matrixFileName="/zf14/ml2au/SyncWithGit/dataNotSynced/ManuallySelected/Perc_3_mouse_gene/mouse_gene.rb" --sparseFileFormatType="RB" --testNumber=44
#130592,49151,49151,-1,-1,1,0.00690914 #small 
test44_L_20:
	$(TEST_EXE_NAME)  --statFileName="statToDelet/statTest45_L_20.csv" --matrixFileName="/zf14/ml2au/SyncWithGit/dataNotSynced/ManuallySelected/Perc_3_mycielskian16/mycielskian16.rb" --sparseFileFormatType="RB" --testNumber=44
#38862,97578,97578
test44_L_21:
	$(TEST_EXE_NAME)  --statFileName="statToDelet/statTest45_L_21.csv" --matrixFileName="/zf14/ml2au/SyncWithGit/dataNotSynced/ManuallySelected/Perc_4_m_t1/m_t1.rb" --sparseFileFormatType="RB" --testNumber=44

#main.out: main.cpp sparseMatrix.cpp computSubArray.cpp vault.cpp
#main.out: main.cpp sparseMatrix.cpp computSubArray.cpp vault.cpp
#	g++ -g -std=c++11 -I../../common/inc -o $@ $< 
#	g++ $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

###############getting results
test45_1:
	$(TEST_EXE_NAME) --computeType=BLOCK_DENSE --matrixFileName="/zf14/ml2au/SyncWithGit/dataNotSynced/ManuallySelected/Perc_8_road_central/road_central.rb" --sparseFileFormatType=RB   --statFileName=statToDelet/road_central_BLOCK_DENSE_extOpt_0.csv  --testNumber=0 --blockDim1Size=32 --numSegments=8
test45_2:
	$(TEST_EXE_NAME) --computeType=BCWait --matrixFileName="/zf14/ml2au/SyncWithGit/dataNotSynced/ManuallySelected/Perc_8_road_central/road_central.rb" --sparseFileFormatType=RB   --statFileName=statToDelet/road_central_BCWait_extOpt_0.csv  --testNumber=0 

clean:
	rm -rf $(OBJ_DIR)
	rm -f main.out
	mkdir obj
	
release:
	cp ./main.out bin/main.out
