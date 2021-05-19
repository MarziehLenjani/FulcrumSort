#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void main()
{
	unsigned char *num;
	char str;
	long double number_of_bytes = (double) 32 * 1024 * 1024 * 1024;
	num = (unsigned char*) calloc(number_of_bytes, sizeof(char));
	printf("address of the Num = %p \n", num);
	printf("Value being pointed by the first element of the num variable = %d \n", *num);

	unsigned long address;
	srand(time(0));
	for(unsigned long access=0; access < 4096 * 100; access++)
	{
		address = (rand() % 10000) * (rand() % 10000);
		printf("Access #%ld Accessing address %ld Value read=%c \n", access, address, *(num+address));
	}
	str = getc(stdin);
}
