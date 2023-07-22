#include "Interface_Exception.h"
#include "LibBaseUtil/String.h"

namespace V2MPLink
{
	std::shared_ptr<V2MPLink_Exception> CreateException(const PublicException& other)
	{
		return std::shared_ptr<V2MPLink_Exception>(new V2MPLink_Exception{other});
	}

	std::shared_ptr<V2MPLink_Exception> CreateErrorException(
		PublicErrorID id,
		const std::string& file,
		const std::string& message
	)
	{
		return std::shared_ptr<V2MPLink_Exception>(new V2MPLink_Exception
			{
				PublicException(id, file, message)
			}
		);
	}

	std::shared_ptr<V2MPLink_Exception> CreateWarningException(
		PublicWarningID id,
		const std::string& file,
		const std::string& message
	)
	{
		return std::shared_ptr<V2MPLink_Exception>(new V2MPLink_Exception
			{
				PublicException(id, file, message)
			}
		);
	}

	std::shared_ptr<V2MPLink_Exception> CreateInternalErrorException(
		const std::string& file,
		const std::string& message
	)
	{
		return CreateErrorException(PublicErrorID::INTERNAL, file, message);
	}

	std::shared_ptr<V2MPLink_Exception> CreateInternalWarningException(
		const std::string& file,
		const std::string& message
	)
	{
		return CreateWarningException(PublicWarningID::INTERNAL, file, message);
	}

	std::shared_ptr<V2MPLink_Exception> CreateUnimplementedException(
		const std::string& file,
		const std::string& message
	)
	{
		return CreateErrorException(PublicErrorID::UNIMPLEMENTED, file, message);
	}
}

extern "C"
{
	API_LIBV2MPLINK V2MPLink_Exception_Type V2MPLink_Exception_GetType(const struct V2MPLink_Exception* exception)
	{
		return exception ? exception->inner.GetType() : V2MPLINK_EXCEPTION_WARNING;
	}

	API_LIBV2MPLINK const char* V2MPLink_Exception_GetID(const struct V2MPLink_Exception* exception)
	{
		return exception ? exception->inner.GetIDString() : nullptr;
	}

	API_LIBV2MPLINK size_t V2MPLink_Exception_ToString(const struct V2MPLink_Exception* exception, char* buffer, size_t length)
	{
		if ( !exception )
		{
			return 0;
		}

		return BaseUtil::CopyStringToBuffer(exception->inner.ToString(), buffer, length);
	}

	API_LIBV2MPLINK size_t V2MPLink_ExceptionList_GetWarningsListSize(void)
	{
		return V2MPLink::GetPublicWarningsListSize();
	}

	API_LIBV2MPLINK const char* V2MPLink_ExceptionList_GetWarningStringID(size_t index)
	{
		return V2MPLink::GetPublicWarningStringID(static_cast<V2MPLink::PublicWarningID>(index), nullptr);
	}

	API_LIBV2MPLINK const char* V2MPLink_ExceptionList_GetWarningDescription(size_t index)
	{
		return V2MPLink::GetPublicWarningDescription(static_cast<V2MPLink::PublicWarningID>(index), nullptr);
	}

	API_LIBV2MPLINK size_t V2MPLink_ExceptionList_GetErrorsListSize(void)
	{
		return V2MPLink::GetPublicErrorsListSize();
	}

	API_LIBV2MPLINK const char* V2MPLink_ExceptionList_GetErrorStringID(size_t index)
	{
		return V2MPLink::GetPublicErrorStringID(static_cast<V2MPLink::PublicErrorID>(index), nullptr);
	}

	API_LIBV2MPLINK const char* V2MPLink_ExceptionList_GetErrorDescription(size_t index)
	{
		return V2MPLink::GetPublicErrorDescription(static_cast<V2MPLink::PublicErrorID>(index), nullptr);
	}
} // extern "C"
