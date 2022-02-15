#include <string.h>
#include "BaseUtil/Util.h"

void* BaseUtil_MemSet(void* ptr, int value, size_t numBytes)
{
	return memset(ptr, value, numBytes);
}
