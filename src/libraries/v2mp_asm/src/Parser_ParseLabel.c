#include "Parser_ParseLabel.h"
#include "CodewordDescriptors/CWD_Label.h"

void V2MPAsm_Parser_ParseLabel(V2MPAsm_Parser* parser)
{
	const char* begin;
	const char* end;
	V2MPAsm_TokenType tokenType;
	const V2MPAsm_TokenMeta* tokenMeta;
	V2MPAsm_CWDLabel* cwdLabel;

	// Sanity:
	if ( V2MPAsm_ParseContext_GetParseState(parser->context) != PARSESTATE_RECORDING_LABEL )
	{
		PARSECONTEXT_INTERNAL_ERROR(
			parser->context,
			"Label parsing routines triggered when the parser was in an incorrect state."
		);

		return;
	}

	V2MPAsm_ParseContext_SkipWhitespace(parser->context);
	begin = V2MPAsm_ParseContext_GetInputCursor(parser->context);
	tokenType = V2MPAsm_TokenMeta_IdentifyToken(begin);
	tokenMeta = V2MPAsm_TokenMeta_GetMetaForTokenType(tokenType);
	end = V2MPAsm_TokenMeta_FindEndOfToken(tokenMeta, begin);

	if ( !begin )
	{
		PARSECONTEXT_INTERNAL_ERROR(
			parser->context,
			"No token to read."
		);

		return;
	}

	if ( !end )
	{
		V2MPAsm_ParseContext_TerminateWithError(
			parser->context,
			PARSEERROR_UNRECOGNISED_TOKEN,
			"Unterminated token of type \"%s\".",
			V2MPAsm_TokenMeta_GetTokenTypeString(tokenType)
		);

		return;
	}

	begin = V2MPAsm_ParseContext_SetCurrentToken(parser->context, begin, end);

	if ( !begin )
	{
		PARSECONTEXT_INTERNAL_ERROR(
			parser->context,
			"Could not allocate buffer for label token."
		);

		return;
	}

	if ( tokenType != TOKEN_LABEL )
	{
		V2MPAsm_ParseContext_TerminateWithError(
			parser->context,
			PARSEERROR_UNEXPECTED_TOKEN,
			"Expected label token, but got \"%s\".",
			begin
		);

		return;
	}

	cwdLabel = V2MPAsm_CWDLabel_Cast(
		V2MPAsm_ParseContext_AppendNewCWDAsCurrent(parser->context, V2MPASM_CWD_LABEL)
	);

	if ( !cwdLabel )
	{
		PARSECONTEXT_INTERNAL_ERRORV(
			parser->context,
			"Could not create label data for \"%s\"",
			begin
		);

		return;
	}

	if ( !V2MPAsm_CWDLabel_SetName(cwdLabel, begin) )
	{
		PARSECONTEXT_INTERNAL_ERRORV(
			parser->context,
			"Could not initialise label data for \"%s\"",
			begin
		);

		return;
	}

	V2MPAsm_ParseContext_SeekInput(parser->context, end);
	V2MPAsm_ParseContext_SetParseState(parser->context, PARSESTATE_DEFAULT);
}
