#ifndef V2MPLINK_LINKER_H
#define V2MPLINK_LINKER_H

#include "LibV2MPLink/LibExport.gen.h"
#include <stdlib.h>

struct V2MPLink_Linker;

typedef enum V2MPLink_LinkerResult
{
	V2MPLINK_COMPLETED_OK = 0,
	V2MPLINK_COMPLETED_WITH_WARNINGS,
	V2MPLINK_FAILED
} V2MPLink_LinkerResult;

LIBV2MPLINK_PUBLIC(V2MPLink_Linker*) V2MPLink_Linker_Create(const char* objectFilePath, const char* outputFilePath);
LIBV2MPLINK_PUBLIC(void) V2MPLink_Linker_Destroy(V2MPLink_Linker* linker);

LIBV2MPLINK_PUBLIC(V2MPLink_LinkerResult) V2MPLink_Linker_Run(struct V2MPLink_Linker* linker);
LIBV2MPLINK_PUBLIC(size_t) V2MPLink_Linker_GetExceptionCount(const struct V2MPLink_Linker* linker);
LIBV2MPLINK_PUBLIC(const struct V2MPLink_Exception*) V2MPLink_Linker_GetException(const struct V2MPLink_Linker* linker, size_t index);

#endif // V2MPLINK_LINKER_H
