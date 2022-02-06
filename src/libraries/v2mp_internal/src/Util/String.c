#include <string.h>
#include "V2MPInternal/Util/String.h"
#include "V2MPInternal/Util/Heap.h"

char* V2MP_String_Duplicate(const char* origString)
{
	size_t length;
	char* newBuffer;

	if ( !origString )
	{
		return NULL;
	}

	length = strlen(origString) + 1;
	newBuffer = (char*)V2MP_MALLOC(length);

	if ( !newBuffer )
	{
		return NULL;
	}

	memcpy(newBuffer, origString, length);

	// Shouldn't be required, but just to be 100% safe:
	newBuffer[length - 1] = '\0';

	return newBuffer;
}
