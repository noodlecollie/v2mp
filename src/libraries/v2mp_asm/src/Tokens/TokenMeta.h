#ifndef V2MPASM_TOKENMETA_H
#define V2MPASM_TOKENMETA_H

#include <stdbool.h>

#define V2MPASM_TOKEN_TYPE_LIST \
	LIST_ITEM(TOKEN_UNKNOWN = 0, NULL) \
	LIST_ITEM(TOKEN_LINE_COMMENT, &V2MPAsm_TokenMeta_LineComment) \
	LIST_ITEM(TOKEN_MULTILINE_COMMENT, &V2MPAsm_TokenMeta_MultilineComment) \
	LIST_ITEM(TOKEN_PREPROCESSOR, &V2MPAsm_TokenMeta_Preprocessor) \
	LIST_ITEM(TOKEN_NUMERIC_LITERAL, &V2MPAsm_TokenMeta_NumericLiteral) \
	LIST_ITEM(TOKEN_NAME, &V2MPAsm_TokenMeta_Name) \
	LIST_ITEM(TOKEN_LABEL, &V2MPAsm_TokenMeta_Label) \

#define LIST_ITEM(value, metaEntry) value,
typedef enum V2MPAsm_TokenType
{
	V2MPASM_TOKEN_TYPE_LIST
} V2MPAsm_TokenType;
#undef LIST_ITEM

typedef struct V2MPAsm_TokenMeta
{
	V2MPAsm_TokenType type;
	const char* typeName;

	// Returns pointer to first character after end of token,
	// or NULL if the token is unterminated.
	const char* (*findEndOfToken)(const char* token);
} V2MPAsm_TokenMeta;

const char* V2MPAsm_TokenMeta_GetTokenTypeString(V2MPAsm_TokenType tokenType);

V2MPAsm_TokenType V2MPAsm_TokenMeta_IdentifyToken(const char* str);
const V2MPAsm_TokenMeta* V2MPAsm_TokenMeta_GetMetaForTokenType(V2MPAsm_TokenType tokenType);

bool V2MPAsm_TokenMeta_IsComment(V2MPAsm_TokenType tokenType);

// These functions should be used, rather than calling function pointers directly on the metadata struct:

const char* V2MPAsm_TokenMeta_FindEndOfToken(const V2MPAsm_TokenMeta* metadata, const char* token);

#endif // V2MPASM_TOKENMETA_H
