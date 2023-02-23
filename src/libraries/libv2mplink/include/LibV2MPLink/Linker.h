#ifndef V2MPLINK_LINKER_H
#define V2MPLINK_LINKER_H

#include "LibV2MPLink/LibExport.gen.h"

#ifdef __cplusplus
extern "C" {
#endif

struct V2MPLink_Linker;

typedef enum V2MPLinker_LinkerResult
{
	V2MPLINK_COMPLETED_OK = 0,
	V2MPLINK_COMPLETED_WITH_WARNINGS,
	V2MPLINK_FAILED
} V2MPLinker_LinkerResult;

API_LIBV2MPLINK V2MPLink_Linker* V2MPLink_Linker_Create(const char* objectFilePath, const char* outputFilePath);
API_LIBV2MPLINK void V2MPLink_Linker_Destroy(V2MPLink_Linker* linker);

API_LIBV2MPLINK V2MPLinker_LinkerResult V2MPLink_Linker_Run(struct V2MPLink_Linker* linker);
API_LIBV2MPLINK size_t V2MPLink_Linker_GetExceptionCount(const struct V2MPLink_Linker* linker);
API_LIBV2MPLINK const struct V2MPLink_Exception* V2MPLink_Linker_GetException(const struct V2MPLink_Linker* linker, size_t index);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MPLINK_LINKER_H
