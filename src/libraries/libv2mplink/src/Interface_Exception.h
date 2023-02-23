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
}
