#include <string.h>
#include "BaseUtil/String.h"
#include "BaseUtil/Heap.h"

char* BaseUtil_String_Duplicate(const char* origString)
{
	size_t length;
	char* newBuffer;

	if ( !origString )
	{
		return NULL;
	}

	length = strlen(origString) + 1;
	newBuffer = (char*)BASEUTIL_MALLOC(length);

	if ( !newBuffer )
	{
		return NULL;
	}

	memcpy(newBuffer, origString, length);

	// Shouldn't be required, but just to be 100% safe:
	newBuffer[length - 1] = '\0';

	return newBuffer;
}
