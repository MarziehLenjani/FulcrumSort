include  specificMakefile.mk
SHELL := /bin/bash
#set -e
INCLUDES  :=  -I$(BOOST_INCLUDEDIR)
LIBRARIES := -L$(BOOST_LIBRARYDIR)
SRC_DIR := ./src
OBJ_DIR := ./obj
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
HEADER_FILES := $(wildcard $(SRC_DIR)/*.hpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS := $(LIBRARIES) -lboost_system -lboost_program_options
CPPFLAGS := -g -std=c++11  $(INCLUDES) -O3
CXXFLAGS :=
EXE_FILE_NAME ?= main.out
EXE_COMMAND  ?= ./$(EXE_FILE_NAME)
EXTRA_COMPILE_OPTIONS  ?=-DUSE_THE_MAIN_MAIN
TST=0
all: build

testEcdcho:
	echo $(OBJ_FILES) 
	echo $(SRC_DIR)

$(EXE_FILE_NAME): $(OBJ_FILES) 
	g++ -o $@ $^  $(LDFLAGS) -O3
	
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADER_FILES)
	g++ $(CPPFLAGS)  $(CXXFLAGS) $(EXTRA_COMPILE_OPTIONS) $(CONFIGURATION_COMPILE_OPTIONS) -fmax-errors=5 -c -o $@ $<
build: $(EXE_FILE_NAME)
########################Main program 
#to run any test type this 
#make run TST=...

#################
run:compile	
	$(EXE_COMMAND) $(COMMAND_LINE_OPTIONS) --testNumber=$(TST)
	
debug:compile # you need to type run 
	gdb    --args $(EXE_FILE_NAME)   --testNumber=$(TST)
	#gdb -batch -x /tmp/cmds --args $(EXE_FILE_NAME)   --testNumber=$(TST) # you need to put run in /tmp/cmds if you do not want type run 
	
clean:
	rm -rf $(OBJ_DIR)
	rm -f $(EXE_FILE_NAME)
	
release:
	cp $(EXE_FILE_NAME) bin/$(EXE_FILE_NAME)
	
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)	

compile: $(OBJ_DIR) $(EXE_FILE_NAME)
#########################



.DEFAULT_GOAL := compile
