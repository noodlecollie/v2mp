#include "Exceptions/PublicExceptionIDs.h"
#include "Utils/ArrayUtils.h"

namespace V2MPAsm
{
	std::string GetPublicWarningStringID(PublicWarningID id)
	{
		static const char* const IDS[] =
		{
#define LIST_ITEM(value, stringID, desc) stringID,
			PUBLICWARNINGTYPE_LIST
#undef LIST_ITEM
		};

		const size_t index = static_cast<size_t>(id);
		return index < ArraySize(IDS) ? IDS[index] : "UNKNOWN";
	}

	std::string GetPublicWarningDescription(PublicWarningID id)
	{
		static const char* const DESCRIPTIONS[] =
		{
#define LIST_ITEM(value, stringID, desc) desc,
			PUBLICWARNINGTYPE_LIST
#undef LIST_ITEM
		};

		const size_t index = static_cast<size_t>(id);
		return index < ArraySize(DESCRIPTIONS) ? DESCRIPTIONS[index] : "UNKNOWN";
	}

	std::string GetPublicErrorStringID(PublicErrorID id)
	{
		static const char* const IDS[] =
		{
#define LIST_ITEM(value, stringID, desc) stringID,
			PUBLICERRORTYPE_LIST
#undef LIST_ITEM
		};

		const size_t index = static_cast<size_t>(id);
		return index < ArraySize(IDS) ? IDS[index] : "UNKNOWN";
	}

	std::string GetPublicErrorDescription(PublicErrorID id)
	{
		static const char* const DESCRIPTIONS[] =
		{
#define LIST_ITEM(value, stringID, desc) desc,
			PUBLICERRORTYPE_LIST
#undef LIST_ITEM
		};

		const size_t index = static_cast<size_t>(id);
		return index < ArraySize(DESCRIPTIONS) ? DESCRIPTIONS[index] : "UNKNOWN";
	}
}
