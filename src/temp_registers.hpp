#include <iostream>
using namespace std;

class temp_registers{
	public:
		temp_registers(void) {
			reg[0] = 0;
			reg[1] = 0;
			reg[2] = 0;
		}

		unsigned int read_reg(unsigned char reg_number) {
			if(reg_number > 2)
				return 0;
			else
				return reg[reg_number];
		}

		void write_reg(unsigned char reg_number, unsigned int value) {
			if(reg_number < 3)
				reg[reg_number] = value;
		}
	private: 
		unsigned int reg[3];
};

/*
int main(void) {
	temp_registers fulcrum_registers;

	cout << "Reading reg 1 " << fulcrum_registers.read_reg(1) << endl;
	fulcrum_registers.write_reg(2, 45);
	cout << "writing into reg 2 " << fulcrum_registers.read_reg(2) << endl;
	cout << "Reading from reg 0 = " << fulcrum_registers.read_reg(0) << endl;

	return 1;

}*/
