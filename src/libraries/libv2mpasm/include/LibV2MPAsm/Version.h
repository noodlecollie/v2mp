#ifndef V2MPASM_VERSION_H
#define V2MPASM_VERSION_H

#include <stdint.h>
#include "LibV2MPAsm/LibExport.gen.h"

LIBV2MPASM_PUBLIC(int32_t) V2MPAsm_Version_GetMajor(void);
LIBV2MPASM_PUBLIC(int32_t) V2MPAsm_Version_GetMinor(void);
LIBV2MPASM_PUBLIC(int32_t) V2MPAsm_Version_GetPatch(void);
LIBV2MPASM_PUBLIC(const char*) V2MPAsm_Version_GetVersionString(void);
LIBV2MPASM_PUBLIC(const char*) V2MPAsm_Version_GetBuildIdentifier(void);

#endif // V2MPASM_VERSION_H
