// This file is generated by CMake. Do not edit it manually!

#include "LibV2MP/Version.h"
#include "Interface_Version.gen.h"

LIBV2MP_PUBLIC(int32_t) V2MP_Version_GetMajor(void)
{
	return V2MP_VERSION_MAJOR;
}

LIBV2MP_PUBLIC(int32_t) V2MP_Version_GetMinor(void)
{
	return V2MP_VERSION_MINOR;
}

LIBV2MP_PUBLIC(int32_t) V2MP_Version_GetPatch(void)
{
	return V2MP_VERSION_PATCH;
}

LIBV2MP_PUBLIC(const char*) V2MP_Version_GetVersionString(void)
{
	return V2MP_VERSION_STRING;
}

LIBV2MP_PUBLIC(const char*) V2MP_Version_GetBuildIdentifier(void)
{
	return V2MP_BUILD_IDENTIFIER_STRING;
}
