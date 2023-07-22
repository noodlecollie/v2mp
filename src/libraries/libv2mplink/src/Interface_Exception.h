#pragma once

#include <vector>
#include <memory>
#include "Exceptions/PublicException.h"
#include "Exceptions/PublicExceptionIDs.h"

struct V2MPLink_Exception
{
	V2MPLink::PublicException inner;
};

namespace V2MPLink
{
	using ExceptionList = std::vector<std::shared_ptr<V2MPLink_Exception>>;

	std::shared_ptr<V2MPLink_Exception> CreateException(const PublicException& other);

	std::shared_ptr<V2MPLink_Exception> CreateErrorException(
		PublicErrorID id,
		const std::string& file = std::string(),
		const std::string& message = std::string()
	);

	std::shared_ptr<V2MPLink_Exception> CreateWarningException(
		PublicWarningID id,
		const std::string& file = std::string(),
		const std::string& message = std::string()
	);

	std::shared_ptr<V2MPLink_Exception> CreateInternalErrorException(
		const std::string& file = std::string(),
		const std::string& message = std::string()
	);

	std::shared_ptr<V2MPLink_Exception> CreateInternalWarningException(
		const std::string& file = std::string(),
		const std::string& message = std::string()
	);

	std::shared_ptr<V2MPLink_Exception> CreateUnimplementedException(
		const std::string& file = std::string(),
		const std::string& message = std::string()
	);
}
