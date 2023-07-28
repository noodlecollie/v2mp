#ifndef V2MP_VERSION_H
#define V2MP_VERSION_H

#include <stdint.h>
#include "LibV2MP/LibExport.gen.h"

LIBV2MP_PUBLIC(int32_t) V2MP_Version_GetMajor(void);
LIBV2MP_PUBLIC(int32_t) V2MP_Version_GetMinor(void);
LIBV2MP_PUBLIC(int32_t) V2MP_Version_GetPatch(void);
LIBV2MP_PUBLIC(const char*) V2MP_Version_GetVersionString(void);
LIBV2MP_PUBLIC(const char*) V2MP_Version_GetBuildIdentifier(void);

#endif // V2MP_VERSION_H
