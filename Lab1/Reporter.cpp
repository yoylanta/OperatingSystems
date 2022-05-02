#define _CRT_SECURE_NO_WARNINGS
#include "Employee.h"
#include <cstdlib>
#include <cstdio>

int main(int argc, char* argv[])
{
	FILE* input = fopen(argv[1], "rb");
	FILE* output = fopen(argv[2], "w");
	Employee buffer;
	fprintf(output, "FILE_REPORT:%s\n", argv[1]);
	while (fread(&buffer, sizeof(Employee), 1, input))
	{
		fprintf(output, "%d,%s,%d,%d\n", buffer.ID, buffer.name,
			buffer.hours, buffer.hours * atoi(argv[3]));
	}

	fclose(output);
	fclose(input);
	return 0;
}