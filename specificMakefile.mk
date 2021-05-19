#CONFIGURATION_COMPILE_OPTIONS += -DTOTAL_NUMBER_OF_VAULTS=32 -DTOTAL_NUMBER_OF_BANKS=512 -DNUMBER_OF_WORDS_IN_A_ROW=64 -DNUMBER_OF_MEMORY_LAYERS=8 -DNUMBER_OF_BANKS_IN_A_LAYERS=64 
#CONFIGURATION_COMPILE_OPTIONS += -DNUMBER_OF_APLU_IN_A_BANK=16 -DNUMBER_OF_ALPU_IN_A_VALULT=256 -DTOTAL_NUMBER_OF_PARTITION=8192
#CONFIGURATION_COMPILE_OPTIONS += -DBASE_CYCLE_IN_NS=0.8 -DROW_CYCLE_IN_BASE_CYCLE=63 -DALU_CYCLE_IN_BASE_CYCLE=8.0 

##################################
runSimple: EXE_COMMAND= ./$(EXE_FILE_NAME)
runSimple: COMMAND_LINE_OPTIONS=  --statFileName=stat100.csv  --testNumber=100 
runSimple:run 
	echo "run simple"
##################################
runValgrind: EXE_COMMAND=valgrind  --leak-check=yes $(EXE_FILE_NAME)
runValgrind:run
	echo "run runValgrind"
###################################
runRelease: EXE_COMMAND=./bin/$(EXE_FILE_NAME)
runRelease:run
	echo "run runRelease"

.DEFAULT_GOAL := compile
