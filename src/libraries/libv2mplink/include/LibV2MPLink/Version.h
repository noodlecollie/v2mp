#ifndef V2MPLINK_VERSION_H
#define V2MPLINK_VERSION_H

#include <stdint.h>
#include "LibV2MPLink/LibExport.gen.h"

#ifdef __cplusplus
extern "C" {
#endif

API_LIBV2MPLINK int32_t V2MPLink_Version_GetMajor(void);
API_LIBV2MPLINK int32_t V2MPLink_Version_GetMinor(void);
API_LIBV2MPLINK int32_t V2MPLink_Version_GetPatch(void);
API_LIBV2MPLINK const char* V2MPLink_Version_GetVersionString(void);
API_LIBV2MPLINK const char* V2MPLink_Version_GetBuildIdentifier(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MPLINK_VERSION_H
