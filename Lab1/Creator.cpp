#define _CRT_SECURE_NO_WARNINGS
#include "Employee.h"
#include <cstdio>
#include <cstdlib>


int main(int argc, char* argv[])
{
	FILE* output = fopen(argv[1], "wb");

	int number_of_records = atoi(argv[2]);
	for (int i = 0; i < number_of_records; i++)
	{
		Employee tmp;
		printf("Enter ID:\t");
		scanf("%d", &tmp.ID);
		printf("Enter name:\t");
		scanf("%s", &tmp.name);
		printf("Enter working hours:\t");
		scanf("%d", &tmp.hours);
		printf("\n");

		fwrite(&tmp, sizeof(Employee), 1, output);
	}
	fclose(output);
	return 0;
}