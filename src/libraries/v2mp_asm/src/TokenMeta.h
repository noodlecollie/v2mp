#ifndef V2MPASM_TOKENMETA_H
#define V2MPASM_TOKENMETA_H

#define V2MPASM_TOKEN_TYPE_LIST \
	LIST_ITEM(TOKEN_UNKNOWN = 0, "Unknown") \
	LIST_ITEM(TOKEN_LINE_COMMENT, "Line comment") \
	LIST_ITEM(TOKEN_MULTILINE_COMMENT, "Multiline comment") \
	LIST_ITEM(TOKEN_PREPROCESSOR, "Preprocessor") \
	LIST_ITEM(TOKEN_NUMERIC_LITERAL, "Numeric literal") \
	LIST_ITEM(TOKEN_NAME, "Name")

#define V2MPASM_TOKEN_CONTEXT_LIST \
	LIST_ITEM(TOKENCTX_DEFAULT = 0, "Default")

#define LIST_ITEM(value, name) value,
typedef enum V2MPAsm_TokenType
{
	V2MPASM_TOKEN_TYPE_LIST
} V2MPAsm_TokenType;
#undef LIST_ITEM

#define LIST_ITEM(value, name) value,
typedef enum V2MPAsm_TokenContext
{
	V2MPASM_TOKEN_CONTEXT_LIST
} V2MPAsm_TokenContext;
#undef LIST_ITEM

typedef struct V2MPAsm_TokenMeta
{
	V2MPAsm_TokenType type;

	const char* (*findEndOfToken)(const char* token, V2MPAsm_TokenContext context);
} V2MPAsm_TokenMeta;

const char* V2MPAsm_TokenMeta_GetTokenTypeString(V2MPAsm_TokenType tokenType);
const char* V2MPAsm_TokenMeta_GetTokenContextString(V2MPAsm_TokenContext tokencontext);

V2MPAsm_TokenType V2MPAsm_TokenMeta_IdentifyToken(const char* str, V2MPAsm_TokenContext context);
const V2MPAsm_TokenMeta* V2MPAsm_TokenMeta_GetMetaForTokenType(V2MPAsm_TokenType tokenType);

#endif // V2MPASM_TOKENMETA_H
