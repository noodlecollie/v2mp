#pragma once

#include <string>

namespace V2MPAsm
{
#define PUBLICWARNINGTYPE_LIST \
	LIST_ITEM(UNSPECIFIED = 0, "unspecified", "Unspecified warning.") \
	LIST_ITEM(ARG_OUT_OF_RANGE, "arg-out-of-range", "Instruction argument is out of range.")

#define PUBLICERRORTYPE_LIST \
	LIST_ITEM(UNSPECIFIED = 0, "unspecified", "Unspecified error.") \
	LIST_ITEM(INTERNAL, "internal-error", "Unexpected internal error.") \
	LIST_ITEM(UNIMPLEMENTED, "unimplemented", "Behaviour unimplemented.") \
	LIST_ITEM(INVALID_INPUT, "invalid-input", "Input was not valid.") \
	LIST_ITEM(UNRECOGNISED_INSTRUCTION, "unrecognised-instruction", "Unrecognised instruction.") \
	LIST_ITEM(UNEXPECTED_TOKEN, "unexpected-token", "Unexpected token encountered.") \
	LIST_ITEM(UNRECOGNISED_TOKEN, "unrecognised-token", "Unrecognised token.") \
	LIST_ITEM(UNTERMINATED_TOKEN, "unterminated-token", "Unterminated token.") \
	LIST_ITEM(EMPTY_TOKEN, "empty-token", "Empty token.") \
	LIST_ITEM(INCORRECT_ARGS, "incorrect-args", "Incorrect arguments for instruction.") \
	LIST_ITEM(INVALID_NUMERIC_LITERAL, "invalid-numeric-literal", "Invalid numeric literal.") \
	LIST_ITEM(DUPLICATE_LABEL, "duplicate-label", "Duplicate label encountered.") \
	LIST_ITEM(ADDRESS_SPACE_EXCEEDED, "address-space-exceeded", "Address space was exceeded.") \
	LIST_ITEM(NON_EXISTENT_FILE, "non-existent-file", "File was not found on disk.") \
	LIST_ITEM(ERROR_OPENING_FILE, "error-opening-file", "Could not open file.")

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

	std::string GetPublicWarningStringID(PublicWarningID id);
	std::string GetPublicWarningDescription(PublicWarningID id);

	std::string GetPublicErrorStringID(PublicErrorID id);
	std::string GetPublicErrorDescription(PublicErrorID id);
}
