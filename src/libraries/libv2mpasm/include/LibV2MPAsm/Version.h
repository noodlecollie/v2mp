#ifndef V2MPASM_VERSION_H
#define V2MPASM_VERSION_H

#include <stdint.h>
#include "LibV2MPAsm/LibExport.h"

#ifdef __cplusplus
extern "C" {
#endif

API_LIBV2MPASM int32_t V2MPAsm_Version_GetMajor(void);
API_LIBV2MPASM int32_t V2MPAsm_Version_GetMinor(void);
API_LIBV2MPASM int32_t V2MPAsm_Version_GetPatch(void);
API_LIBV2MPASM const char* V2MPAsm_Version_GetVersionString(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MPASM_VERSION_H
