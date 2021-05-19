#include <iostream>
using namespace std;

#define		SHIFT_LEFT		0
#define		SHIFT_RIGHT		1
class ALU {
	public:
		unsigned int add(unsigned int inputA, unsigned int inputB) {
			return (inputA+inputB);
		}
	
		unsigned int subtract(unsigned int inputA, unsigned int inputB) {
			return (inputA-inputB);
		}

		unsigned int multiply(unsigned int inputA, unsigned int inputB) {
			return ((unsigned int)(inputA*inputB));
		}
	
		unsigned int bitwise_and(unsigned int inputA, unsigned int inputB) {
			return (inputA & inputB);
		}
	
		unsigned int bitwise_or(unsigned int inputA, unsigned int inputB) {
			return (inputA | inputB);
		}
	
		unsigned int bitwise_xor(unsigned int inputA, unsigned int inputB) {
			return (inputA ^ inputB);
		}
	
		unsigned int bitwise_nor(unsigned int inputA, unsigned int inputB) {
			return (~(inputA | inputB));
		}
	
		unsigned int bitwise_shift(unsigned int inputA, unsigned char shiftDir, unsigned char shiftValue) {
			if(shiftDir == SHIFT_LEFT)
				return (inputA << shiftValue);
			else
				return (inputA >> shiftValue);
		}
};

/*
int main(void) {
	ALU fulcrum_alu;
	unsigned int num1, num2;

	num1 = 560;
	num2 = 870;

	cout << " ADD operation of " << num1 << " and " << num2 << " is " << (fulcrum_alu.add(num1, num2)) << endl;
	cout << " SUB operation of " << num1 << " and " << num2 << " is " << (fulcrum_alu.subtract(num1, num2))  << endl;;
	cout << " MUL operation of " << num1 << " and " << num2 << " is " << (fulcrum_alu.multiply(num1, num2)) << endl;;
	cout << " BITWISE_AND operation of " << num1 << " and " << num2 << " is " << (fulcrum_alu.bitwise_and(num1, num2) ) << endl;;
	cout << " BITWISE_OR operation of " << num1 << " and " << num2 << " is " << (fulcrum_alu.bitwise_or(num1, num2) ) << endl;;
	cout << " BITWISE_XOR operation of " << num1 << " and " << num2 << " is " << (fulcrum_alu.bitwise_xor(num1, num2) ) << endl;;
	cout << " BITWISE_NOR operation of " << num1 << " and " << num2 << " is " << (fulcrum_alu.bitwise_nor(num1, num2) ) << endl;;
	cout << " BITWISE_SHIFT operation of " << num1 << " and " << num2 << " is " << (fulcrum_alu.bitwise_shift(num1, SHIFT_LEFT, 3)) << endl;;
}

*/
