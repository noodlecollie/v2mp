#pragma once

#include <memory>
#include <string>
#include <vector>
#include "Exceptions/PublicException.h"
#include "Exceptions/PublicExceptionIDs.h"
#include "Utils/ParsingUtils.h"

struct V2MPAsm_Exception
{
	V2MPAsm::PublicException inner;
};

namespace V2MPAsm
{
	using ExceptionList = std::vector<std::shared_ptr<V2MPAsm_Exception>>;

	std::shared_ptr<V2MPAsm_Exception> CreateException(const PublicException& other);

	std::shared_ptr<V2MPAsm_Exception> CreateErrorException(
		PublicErrorID id,
		const std::string& file = std::string(),
		size_t line = LINE_NUMBER_BASE,
		size_t column = COLUMN_NUMBER_BASE,
		const std::string& message = std::string()
	);

	std::shared_ptr<V2MPAsm_Exception> CreateWarningException(
		PublicWarningID id,
		const std::string& file = std::string(),
		size_t line = LINE_NUMBER_BASE,
		size_t column = COLUMN_NUMBER_BASE,
		const std::string& message = std::string()
	);

	std::shared_ptr<V2MPAsm_Exception> CreateInternalErrorException(
		const std::string& file = std::string(),
		size_t line = LINE_NUMBER_BASE,
		size_t column = COLUMN_NUMBER_BASE,
		const std::string& message = std::string()
	);

	std::shared_ptr<V2MPAsm_Exception> CreateInternalWarningException(
		const std::string& file = std::string(),
		size_t line = LINE_NUMBER_BASE,
		size_t column = COLUMN_NUMBER_BASE,
		const std::string& message = std::string()
	);

	std::shared_ptr<V2MPAsm_Exception> CreateUnimplementedException(
		const std::string& file = std::string(),
		size_t line = LINE_NUMBER_BASE,
		size_t column = COLUMN_NUMBER_BASE,
		const std::string& message = std::string()
	);
}
