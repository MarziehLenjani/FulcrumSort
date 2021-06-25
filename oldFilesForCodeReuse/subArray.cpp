#include <iostream>
#include "debug.hpp"

#define		NUM_OF_ROWS		512
#define		NUM_OF_COLS		64

#define		PP_SUC			1
#define		PP_ERR			0

class subArray{
	public:
		subArray(void) {
			for(int i=0; i<NUM_OF_ROWS;i++){
				for(int j=0;j<NUM_OF_COLS; j++) {
					subArrayData[i][j] = 0;		// Initializing everything to zero
				}
			}
		}

		unsigned char write_row_data_to_subArray(unsigned short rowNumber, unsigned int rowData[]) {
			if(rowNumber > NUM_OF_ROWS)
				return PP_ERR;

			for(int i=0; i<NUM_OF_COLS;i++) {
				subArrayData[rowNumber][i] = rowData[i];
			}
			return PP_SUC;

		}

		unsigned char read_row_data_from_subArray(unsigned short rowNumber, unsigned int rowData[]) {
			if(rowNumber > NUM_OF_ROWS)
				return PP_ERR;

			for(unsigned short i=0; i<NUM_OF_COLS; i++) {
				rowData[i] = subArrayData[rowNumber][i];
				debug_printf("Row %d Column %d = %d \n", rowNumber, i, subArrayData[rowNumber][i]);
			}

			return PP_SUC;
		}
	private:
		unsigned int subArrayData[NUM_OF_ROWS][NUM_OF_COLS];
};
#ifdef USE_MAIN_IN_SUBARRAY
int main(void) {
	unsigned int sample_data[]={1,2,3,4,5};
	unsigned int read_data[5];
	subArray	sub_1;
	unsigned char ret;


	ret = sub_1.write_row_data_to_subArray(1, sample_data);
	ret = sub_1.read_row_data_from_subArray(1, read_data);

	for(int i=0; i<5;i++)
		debug_printf("data being read back :: read_data[%d] = %d \n",i,read_data[i]);

}
#endif
