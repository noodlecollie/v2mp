#ifndef V2MP_VERSION_H
#define V2MP_VERSION_H

#include <stdint.h>
#include "LibV2MP/LibExport.gen.h"

#ifdef __cplusplus
extern "C" {
#endif

API_LIBV2MPASM int32_t V2MP_Version_GetMajor(void);
API_LIBV2MPASM int32_t V2MP_Version_GetMinor(void);
API_LIBV2MPASM int32_t V2MP_Version_GetPatch(void);
API_LIBV2MPASM const char* V2MP_Version_GetVersionString(void);
API_LIBV2MPASM const char* V2MP_Version_GetBuildIdentifier(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MP_VERSION_H
