#include <memory>
#include "Exceptions/PublicException.h"
#include "Exceptions/PublicExceptionIDs.h"
#include "Interface_Exception.h"
#include "Utils/StringUtils.h"

namespace V2MPAsm
{
	std::shared_ptr<V2MPAsm_Exception> CreateException(const PublicException& other)
	{
		return std::shared_ptr<V2MPAsm_Exception>(new V2MPAsm_Exception{other});
	}

	std::shared_ptr<V2MPAsm_Exception> CreateErrorException(
		PublicErrorID id,
		const std::string& file,
		size_t line,
		size_t column,
		const std::string& message
	)
	{
		return std::shared_ptr<V2MPAsm_Exception>(new V2MPAsm_Exception
			{
				PublicException(id, file, line, column, message)
			}
		);
	}

	std::shared_ptr<V2MPAsm_Exception> CreateWarningException(
		PublicWarningID id,
		const std::string& file,
		size_t line,
		size_t column,
		const std::string& message
	)
	{
		return std::shared_ptr<V2MPAsm_Exception>(new V2MPAsm_Exception
			{
				PublicException(id, file, line, column, message)
			}
		);
	}

	std::shared_ptr<V2MPAsm_Exception> CreateInternalErrorException(
		const std::string& file,
		size_t line,
		size_t column,
		const std::string& message
	)
	{
		return CreateErrorException(PublicErrorID::INTERNAL, file, line, column, message);
	}

	std::shared_ptr<V2MPAsm_Exception> CreateInternalWarningException(
		const std::string& file,
		size_t line,
		size_t column,
		const std::string& message
	)
	{
		return CreateWarningException(PublicWarningID::INTERNAL, file, line, column, message);
	}

	std::shared_ptr<V2MPAsm_Exception> CreateUnimplementedException(
		const std::string& file,
		size_t line,
		size_t column,
		const std::string& message
	)
	{
		return CreateErrorException(PublicErrorID::UNIMPLEMENTED, file, line, column, message);
	}
}


#ifdef __cplusplus
extern "C" {
#endif

API_LIBV2MPASM V2MPAsm_Exception_Type V2MPAsm_Exception_GetType(const struct V2MPAsm_Exception* exception)
{
	return exception ? exception->inner.GetType() : V2MPASM_EXCEPTION_WARNING;
}

API_LIBV2MPASM const char* V2MPAsm_Exception_GetID(const struct V2MPAsm_Exception* exception)
{
	return exception ? exception->inner.GetIDString() : nullptr;
}

API_LIBV2MPASM size_t V2MPAsm_Exception_ToString(const struct V2MPAsm_Exception* exception, char* buffer, size_t length)
{
	if ( !exception )
	{
		return 0;
	}

	return V2MPAsm::CopyStringToBuffer(exception->inner.ToString(), buffer, length);
}

#ifdef __cplusplus
} // extern "C"
#endif
