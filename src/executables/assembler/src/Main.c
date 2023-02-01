#include <stdio.h>
#include "V2MPAsm/Assembler.h"
#include "V2MPAsm/Exception.h"

int main(int argc, char** argv)
{
	if ( argc < 2 )
	{
		printf("No input file specified.\n");
		return 1;
	}

	if ( argc < 3 )
	{
		printf("No output file specified.\n");
		return 1;
	}

	struct V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromFiles(argv[1], argv[2]);

	if ( !V2MPAsm_Assembler_Run(assembler) )
	{
		const size_t numExceptions = V2MPAsm_Assembler_GetExceptionCount(assembler);

		printf("Encountered %zu exceptions:\n", numExceptions);

		for ( size_t index = 0; index < numExceptions; ++index )
		{
			const struct V2MPAsm_Exception* exception = V2MPAsm_Assembler_GetException(assembler, index);

			char buffer[256];
			V2MPAsm_Exception_ToString(exception, buffer, sizeof(buffer));

			printf("%s\n", buffer);
		}
	}

	V2MPAsm_Assembler_Destroy(assembler);

	return 0;
}
