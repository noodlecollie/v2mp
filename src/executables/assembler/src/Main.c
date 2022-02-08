#include <stdio.h>
#include "V2MPAsm/TempTest.h"

int main(int argc, char** argv)
{
	if ( argc < 2 )
	{
		printf("No file specified.\n");
		return 1;
	}

	V2MPAsm_TempTest_ReadFile(argv[1]);
	return 0;
}
