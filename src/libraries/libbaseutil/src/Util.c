#include <string.h>
#include <stdlib.h>
#include "LibBaseUtil/Util.h"

void* BaseUtil_MemSet(void* ptr, int value, size_t numBytes)
{
	return memset(ptr, value, numBytes);
}

void BaseUtil_ExitProcess(int code)
{
	exit(code);
}
