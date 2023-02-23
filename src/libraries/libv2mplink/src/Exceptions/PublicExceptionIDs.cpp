#include "Exceptions/PublicExceptionIDs.h"
#include "LibBaseUtil/Array.h"

namespace V2MPLink
{
	static const char* const WARNING_IDS[] =
	{
#define LIST_ITEM(value, stringID, desc) stringID,
		PUBLICWARNINGTYPE_LIST
#undef LIST_ITEM
	};

	static const char* const WARNING_DESCRIPTIONS[] =
	{
#define LIST_ITEM(value, stringID, desc) desc,
		PUBLICWARNINGTYPE_LIST
#undef LIST_ITEM
	};

	static const char* const ERROR_IDS[] =
	{
#define LIST_ITEM(value, stringID, desc) stringID,
		PUBLICERRORTYPE_LIST
#undef LIST_ITEM
	};

	static const char* const ERROR_DESCRIPTIONS[] =
	{
#define LIST_ITEM(value, stringID, desc) desc,
		PUBLICERRORTYPE_LIST
#undef LIST_ITEM
	};

	size_t GetPublicWarningsListSize()
	{
		return BaseUtil::ArraySize(WARNING_IDS);
	}

	const char* GetPublicWarningStringID(PublicWarningID id, const char* returnedIfNotFound)
	{
		const size_t index = static_cast<size_t>(id);
		return index < BaseUtil::ArraySize(WARNING_IDS) ? WARNING_IDS[index] : returnedIfNotFound;
	}

	const char* GetPublicWarningDescription(PublicWarningID id, const char* returnedIfNotFound)
	{
		const size_t index = static_cast<size_t>(id);
		return index < BaseUtil::ArraySize(WARNING_DESCRIPTIONS) ? WARNING_DESCRIPTIONS[index] : returnedIfNotFound;
	}

	size_t GetPublicErrorsListSize()
	{
		return BaseUtil::ArraySize(ERROR_IDS);
	}

	const char* GetPublicErrorStringID(PublicErrorID id, const char* returnedIfNotFound)
	{
		const size_t index = static_cast<size_t>(id);
		return index < BaseUtil::ArraySize(ERROR_IDS) ? ERROR_IDS[index] : returnedIfNotFound;
	}

	const char* GetPublicErrorDescription(PublicErrorID id, const char* returnedIfNotFound)
	{
		const size_t index = static_cast<size_t>(id);
		return index < BaseUtil::ArraySize(ERROR_DESCRIPTIONS) ? ERROR_DESCRIPTIONS[index] : returnedIfNotFound;
	}
}
