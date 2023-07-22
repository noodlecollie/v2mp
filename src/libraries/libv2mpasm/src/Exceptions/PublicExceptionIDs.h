#pragma once

#include <string>

namespace V2MPAsm
{
#define PUBLICWARNINGTYPE_LIST \
	LIST_ITEM(UNSPECIFIED = 0, "unspecified-warning", "Unspecified warning.") \
	LIST_ITEM(INTERNAL, "internal-warning", "An unexpected internal warning was encountered.") \
	LIST_ITEM(ARG_OUT_OF_RANGE, "arg-out-of-range", "An instruction argument was out of range.") \
	LIST_ITEM(UNTERMINATED_TOKEN, "unterminated-token", "An unterminated token was encountered.") \
	LIST_ITEM(REDUNDANT_LABEL, "redundant-label", "A redundant label was encountered.") \
	LIST_ITEM(LABEL_DISCARDED, "label-discarded", "A label was discarded.") \
	LIST_ITEM(TOO_MANY_ARGUMENTS, "too-many-arguments", "Too many arguments were given for an instruction.") \
	LIST_ITEM(RESERVED_BITS_SET, "reserved-bits-set", "Reserved bits were set to a value other than zero.") \
	LIST_ITEM(DIVISION_BY_ZERO, "division-by-zero", "An attempt was made to divide by zero.") \
	LIST_ITEM(INALID_ARG_VALUE, "invalid-arg-value", "An argument value was not valid for its instruction.")

#define PUBLICERRORTYPE_LIST \
	/* General: */ \
	LIST_ITEM(UNSPECIFIED = 0, "unspecified-error", "Unspecified error.") \
	LIST_ITEM(INTERNAL, "internal-error", "An unexpected internal error occurred.") \
	LIST_ITEM(UNIMPLEMENTED, "unimplemented", "Some behaviour or feature is not yet unimplemented.") \
	LIST_ITEM(NON_EXISTENT_FILE, "non-existent-file", "File was not found on disk.") \
	LIST_ITEM(EMPTY_INPUT, "empty-input", "The input data to the parser was empty.") \
	LIST_ITEM(ERROR_OPENING_FILE, "error-opening-file", "File could not be opened.") \
	LIST_ITEM(EXCEEDED_ERROR_COUNT, "exceeded-error-count", "Parsing was terminated due to the number of errors encountered.") \
	/* Tokenising stage: */ \
	LIST_ITEM(UNRECOGNISED_TOKEN, "unrecognised-token", "An unrecognised token was encountered.") \
	LIST_ITEM(UNTERMINATED_TOKEN, "unterminated-token", "An unterminated token was encountered.") \
	LIST_ITEM(UNEXPECTED_TOKEN, "unexpected-token", "A token was encountered where it was not expected.") \
	LIST_ITEM(INVALID_NUMERIC_LITERAL, "invalid-numeric-literal", "An invalid numeric literal was encountered.") \
	LIST_ITEM(INVALID_LABEL_REFERENCE, "invalid-label-reference", "An invalid label reference was encountered.") \
	LIST_ITEM(INVALID_LABEL_NAME, "invalid-label-definition", "An invalid label definition was encountered.") \
	/* Parsing stage: */ \
	LIST_ITEM(UNRECOGNISED_INSTRUCTION, "unrecognised-instruction", "An unrecognised instruction was encountered.") \
	LIST_ITEM(TOO_FEW_ARGUMENTS, "too-few-arguments", "Too few arguments were given for an instruction.") \
	LIST_ITEM(DUPLICATE_LABEL, "duplicate-label", "A duplicate label was encountered.") \
	LIST_ITEM(ADDRESS_SPACE_EXCEEDED, "address-space-exceeded", "The maximum program address space was exceeded.") \
	LIST_ITEM(INVALID_REGISTER_ID, "invalid-register-id", "An invalid register ID was encountered.") \
	LIST_ITEM(REGISTER_NOT_ALLOWED, "register-not-allowed", "A register was used where is not allowed.") \
	LIST_ITEM(INVALID_ARGUMENT_TYPE, "invalid-argument-type", "An arument for an instruction was of an invalid type.") \
	LIST_ITEM(LABEL_REF_OUT_OF_RANGE, "label-ref-out-of-range", "The value of a label reference was out of range.")

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
