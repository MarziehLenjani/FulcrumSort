#include <iostream>
#include <stdlib.h>
#include <deque>

#define NUM_OF_COLUMNS	64	

class walker{
	public:
		// constructor for the walker
		walker(void) {
			unsigned int i =0;	// TODO remove
			last_accessed_column_index = 0; 	// 0--> indicates that the walker was untouched

			isWalker1 = true;
			isWalker2 = false;
			isWalker3 = false;
		}

		// return the latest column index being accessed
		unsigned short lastAccessedColIndex(void) {
			return last_accessed_column_index;
		}

		// returns the data present in that column of the walker
		unsigned int read_columnData(unsigned short num) {
			int value;

			if (num > colData.size()) {
				printf("[DEBUG] It appears that you have accessed an out of bounds elements, walker current size=%ld \n", colData.size());
				return 0;	// TODO Modify this to PP_SUC and PP_ERR
			}
		
			value = colData.at(num);
			return value;

		}

		//
		// function used to write data into the columns of the walker
		// args - row number, columnar data
		// return type - 0-> success 1->error
		//
		unsigned char writeColumnData(unsigned int input_column_data[NUM_OF_COLUMNS]) {
			for(int i=0; i<NUM_OF_COLUMNS; i++) {
				colData.push_back(input_column_data[i]);
			}
			return 0;
			// TODO what are the error cases here?
		}

		//
		//print all the column data of this walker
		//
		unsigned char print_column_data(void) {
			for(int i=0; i<NUM_OF_COLUMNS; i++) {
				printf("Column %d = %d \n", i, colData.at(i));
			}
			return 0;
			// TODO what are the error cases here?
		}
	private:
		std::deque<unsigned int> colData;
		unsigned short last_accessed_column_index;		// Stores the value of the last accessed column by the ALPU
									// Suppose 1024 columns are their, in first cycle col1 and col2  get processed, thus current_column_index=2 at the end of first cycle
									// Also current_column_index = 1:NUM_OF_COLUMNS (Column 0 is not available for simplicity)
									// if lastAccessedColIndex = 0 then, it appears like the walker was untouched

		// Since each walker has directional (left-right) restrictions, we need this info to conform to these rules
		bool isWalker1;
		bool isWalker2;
		bool isWalker3;

};
