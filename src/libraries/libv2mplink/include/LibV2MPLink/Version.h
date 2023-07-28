#ifndef V2MPLINK_VERSION_H
#define V2MPLINK_VERSION_H

#include <stdint.h>
#include "LibV2MPLink/LibExport.gen.h"

LIBV2MPLINK_PUBLIC(int32_t) V2MPLink_Version_GetMajor(void);
LIBV2MPLINK_PUBLIC(int32_t) V2MPLink_Version_GetMinor(void);
LIBV2MPLINK_PUBLIC(int32_t) V2MPLink_Version_GetPatch(void);
LIBV2MPLINK_PUBLIC(const char*) V2MPLink_Version_GetVersionString(void);
LIBV2MPLINK_PUBLIC(const char*) V2MPLink_Version_GetBuildIdentifier(void);

#endif // V2MPLINK_VERSION_H
