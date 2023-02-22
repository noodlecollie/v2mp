#include "Interface_Exception.h"
#include "LibBaseUtil/String.h"

extern "C"
{
	API_LIBV2MPLINK V2MPLink_Exception_Type V2MPLink_Exception_GetType(const struct V2MPLink_Exception* exception)
	{
		return exception ? exception->inner.GetType() : V2MPLINK_EXCEPTION_WARNING;
	}

	API_LIBV2MPLINK const char* V2MPAsm_Exception_GetID(const struct V2MPLink_Exception* exception)
	{
		return exception ? exception->inner.GetIDString() : nullptr;
	}

	API_LIBV2MPLINK size_t V2MPAsm_Exception_ToString(const struct V2MPLink_Exception* exception, char* buffer, size_t length)
	{
		if ( !exception )
		{
			return 0;
		}

		return BaseUtil::CopyStringToBuffer(exception->inner.ToString(), buffer, length);
	}

	API_LIBV2MPLINK size_t V2MPAsm_ExceptionList_GetWarningsListSize(void)
	{
		return V2MPLink::GetPublicWarningsListSize();
	}

	API_LIBV2MPLINK const char* V2MPAsm_ExceptionList_GetWarningStringID(size_t index)
	{
		return V2MPLink::GetPublicWarningStringID(static_cast<V2MPLink::PublicWarningID>(index), nullptr);
	}

	API_LIBV2MPLINK const char* V2MPAsm_ExceptionList_GetWarningDescription(size_t index)
	{
		return V2MPLink::GetPublicWarningDescription(static_cast<V2MPLink::PublicWarningID>(index), nullptr);
	}

	API_LIBV2MPLINK size_t V2MPAsm_ExceptionList_GetErrorsListSize(void)
	{
		return V2MPLink::GetPublicErrorsListSize();
	}

	API_LIBV2MPLINK const char* V2MPAsm_ExceptionList_GetErrorStringID(size_t index)
	{
		return V2MPLink::GetPublicErrorStringID(static_cast<V2MPLink::PublicErrorID>(index), nullptr);
	}

	API_LIBV2MPLINK const char* V2MPAsm_ExceptionList_GetErrorDescription(size_t index)
	{
		return V2MPLink::GetPublicErrorDescription(static_cast<V2MPLink::PublicErrorID>(index), nullptr);
	}
} // extern "C"
