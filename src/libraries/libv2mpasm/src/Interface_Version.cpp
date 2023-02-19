#include <string>
#include <sstream>
#include <cstring>
#include "LibV2MPAsm/Version.h"
#include "Interface_Version.h"

#ifdef __cplusplus
extern "C" {
#endif

API_LIBV2MPASM int32_t V2MPAsm_Version_GetMajor(void)
{
	return V2MPAsm::VERSION_MAJOR;
}

API_LIBV2MPASM int32_t V2MPAsm_Version_GetMinor(void)
{
	return V2MPAsm::VERSION_MINOR;
}

API_LIBV2MPASM int32_t V2MPAsm_Version_GetPatch(void)
{
	return V2MPAsm::VERSION_PATCH;
}

API_LIBV2MPASM const char* V2MPAsm_Version_GetVersionString(void)
{
	// TODO: Use a buffer and a safe C string lib?
	// Eg. https://github.com/intel/safestringlib/tree/v1.2.0
	static std::string versionString;

	if ( versionString.empty() )
	{
		std::stringstream stream;
		stream << V2MPAsm::VERSION_MAJOR << "." << V2MPAsm::VERSION_MINOR << "." << V2MPAsm::VERSION_PATCH;
		versionString = stream.str();
	}

	return versionString.c_str();
}

#ifdef __cplusplus
} // extern "C"
#endif
