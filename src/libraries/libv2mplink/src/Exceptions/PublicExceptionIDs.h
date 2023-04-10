#pragma once

#include <string>

namespace V2MPLink
{
#define PUBLICWARNINGTYPE_LIST \
	LIST_ITEM(UNSPECIFIED = 0, "unspecified-warning", "Unspecified warning.") \
	LIST_ITEM(INTERNAL, "internal-warning", "An unexpected internal warning was encountered.")

#define PUBLICERRORTYPE_LIST \
	/* General: */ \
	LIST_ITEM(UNSPECIFIED = 0, "unspecified-error", "Unspecified error.") \
	LIST_ITEM(INTERNAL, "internal-error", "An unexpected internal error occurred.") \
	LIST_ITEM(UNIMPLEMENTED, "unimplemented", "Some behaviour or feature is not yet unimplemented.") \
	LIST_ITEM(NON_EXISTENT_FILE, "non-existent-file", "File was not found on disk.") \
	LIST_ITEM(ERROR_OPENING_FILE, "error-opening-file", "File could not be opened.")

	enum class PublicWarningID
	{
#define LIST_ITEM(value, stringID, desc) value,
		PUBLICWARNINGTYPE_LIST
#undef LIST_ITEM
	};

	enum class PublicErrorID
	{
#define LIST_ITEM(value, stringID, desc) value,
		PUBLICERRORTYPE_LIST
#undef LIST_ITEM
	};

	size_t GetPublicWarningsListSize();
	const char* GetPublicWarningStringID(PublicWarningID id, const char* returnedIfNotFound = "UNKNOWN");
	const char* GetPublicWarningDescription(PublicWarningID id, const char* returnedIfNotFound = "UNKNOWN");

	size_t GetPublicErrorsListSize();
	const char* GetPublicErrorStringID(PublicErrorID id, const char* returnedIfNotFound = "UNKNOWN");
	const char* GetPublicErrorDescription(PublicErrorID id, const char* returnedIfNotFound = "UNKNOWN");
}
