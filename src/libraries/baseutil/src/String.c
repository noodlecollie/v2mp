#include <string.h>
#include <ctype.h>
#include <stdlib.h>
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

const char* BaseUtil_String_EndWithoutWhitespace(const char* origString)
{
	const char* lastValid;

	if ( !origString )
	{
		return NULL;
	}

	if ( !(*origString) )
	{
		return origString;
	}

	lastValid = origString;

	while ( *origString )
	{
		if ( !isspace(*origString) )
		{
			lastValid = origString;
		}

		++origString;
	}

	// If for some reason the string was all whitespace,
	// lastValid will be whitespace. In this case, the
	// terminator should be placed there. Otherwise, the
	// terminator should be placed at the next character.
	return isspace(*lastValid) ? lastValid : (lastValid + 1);
}

const char* BaseUtil_String_BeginWithoutWhitespace(const char* origString)
{
	if ( !origString )
	{
		return NULL;
	}

	while ( *origString && isspace(*origString) )
	{
		++origString;
	}

	return origString;
}

const char* BaseUtil_String_NextWhitespace(const char* string)
{
	if ( !string )
	{
		return NULL;
	}

	while ( *string && !isspace(*string) )
	{
		++string;
	}

	return string;
}

bool BaseUtil_String_CharIsAlphanumericOrUnderscore(char ch)
{
	return isalnum(ch) || ch == '_';
}

bool BaseUtil_String_ToLongInt(const char* str, const char** end, int base, long int* output)
{
	const char* localEnd = NULL;

	if ( !str || !(*str) || !output )
	{
		return false;
	}

	// Why is the first argument to strtol const, and the second one not??
	*output = strtol(str, (char**)(end ? end : &localEnd), base);

	if ( *output != 0 )
	{
		return true;
	}

	// If the number was not valid and 0 was returned, the "end" pointer
	// will be set to be equal to the "str" pointer.
	return (end ? (*end) : localEnd) > str;
}

int BaseUtil_String_GetBaseFromNumberPrefix(const char* str)
{
	if ( !str )
	{
		return 0;
	}

	if ( str[0] == '0' )
	{
		if ( str[1] == 'X' || str[1] == 'x' )
		{
			return 16;
		}

		if ( str[1] == 'B' || str[1] == 'b' )
		{
			return 2;
		}

		return 10;
	}

	if ( str[0] >= '1' && str[0] <= '9' )
	{
		return 10;
	}

	return 0;
}
