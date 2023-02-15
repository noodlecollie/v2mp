#pragma once

#include <string>

namespace V2MPAsm
{
#define PUBLICWARNINGTYPE_LIST \
	LIST_ITEM(UNSPECIFIED = 0, "unspecified", "Unspecified warning.") \
	LIST_ITEM(INTERNAL, "internal-warning", "Unexpected internal warning.") \
	LIST_ITEM(ARG_OUT_OF_RANGE, "arg-out-of-range", "Instruction argument is out of range.") \
	LIST_ITEM(UNTERMINATED_TOKEN, "unterminated-token", "Unterminated token.") \
	LIST_ITEM(REDUNDANT_LABEL, "redundant-label", "Redundant label.") \
	LIST_ITEM(LABEL_DISCARDED, "label-discarded", "Label discarded.") \
	LIST_ITEM(TOO_MANY_ARGUMENTS, "too-many-arguments", "Too many arguments for instruction.") \
	LIST_ITEM(RESERVED_BITS_SET, "reserved-bits-set", "Reserved bits set to a value other than zero.") \
	LIST_ITEM(DIVISION_BY_ZERO, "division-by-zero", "Attempt to divide by zero.") \
	LIST_ITEM(INALID_ARG_VALUE, "invalid-arg-value", "Argument value is not valid for this instruction.")

#define PUBLICERRORTYPE_LIST \
	/* General: */ \
	LIST_ITEM(UNSPECIFIED = 0, "unspecified", "Unspecified error.") \
	LIST_ITEM(INTERNAL, "internal-error", "Unexpected internal error.") \
	LIST_ITEM(UNIMPLEMENTED, "unimplemented", "Behaviour unimplemented.") \
	LIST_ITEM(NON_EXISTENT_FILE, "non-existent-file", "File was not found on disk.") \
	LIST_ITEM(EMPTY_INPUT, "empty-input", "Input data to parser was empty.") \
	LIST_ITEM(ERROR_OPENING_FILE, "error-opening-file", "Could not open file.") \
	LIST_ITEM(EXCEEDED_ERROR_COUNT, "exceeded-error-count", "Terminated parsing due to number of errors.") \
	/* Tokenising stage: */ \
	LIST_ITEM(UNRECOGNISED_TOKEN, "unrecognised-token", "Unrecognised token.") \
	LIST_ITEM(UNTERMINATED_TOKEN, "unterminated-token", "Unterminated token.") \
	LIST_ITEM(UNEXPECTED_TOKEN, "unexpected-token", "Unexpected token.") \
	LIST_ITEM(INVALID_NUMERIC_LITERAL, "invalid-numeric-literal", "Invalid numeric literal.") \
	LIST_ITEM(INVALID_LABEL_REFERENCE, "invalid-label-reference", "Invalid label reference.") \
	LIST_ITEM(INVALID_LABEL_NAME, "invalid-label-definition", "Invalid label definition.") \
	/* Parsing stage: */ \
	LIST_ITEM(UNRECOGNISED_INSTRUCTION, "unrecognised-instruction", "Unrecognised instruction.") \
	LIST_ITEM(TOO_FEW_ARGUMENTS, "too-few-arguments", "Too few arguments for instruction.") \
	LIST_ITEM(DUPLICATE_LABEL, "duplicate-label", "Duplicate label encountered.") \
	LIST_ITEM(ADDRESS_SPACE_EXCEEDED, "address-space-exceeded", "Address space was exceeded.") \
	LIST_ITEM(INVALID_REGISTER_ID, "invalid-register-id", "Invalid register ID.") \
	LIST_ITEM(REGISTER_NOT_ALLOWED, "register-not-allowed", "Use of this register is not allowed.") \
	LIST_ITEM(INVALID_ARGUMENT_TYPE, "invalid-argument-type", "Invalid argument type.") \
	LIST_ITEM(LABEL_REF_OUT_OF_RANGE, "label-ref-out-of-range", "Value of label reference is out of range.")

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
