#ifndef V2MPLINK_LINKER_H
#define V2MPLINK_LINKER_H

#include "LibV2MPLink/LibExport.gen.h"

#ifdef __cplusplus
extern "C" {
#endif

struct V2MPLink_Linker;

API_LIBV2MPLINK V2MPLink_Linker* V2MPLink_Linker_Create(const char* objectFilePath, const char* outputFilePath);
API_LIBV2MPLINK void V2MPLink_Linker_Destroy(V2MPLink_Linker* linker);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MPLINK_LINKER_H
