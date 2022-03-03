#include "Parser_ResolveReferences.h"
#include "Parser_Internal.h"
#include "ParseContext.h"
#include "BaseUtil/UTHash_V2MP.h"
#include "BaseUtil/Heap.h"
#include "CodewordDescriptors/CWD_Label.h"

typedef struct LabelEntry
{
	const char* labelName;
	V2MPAsm_CWDLabel* cwdLabel;
	UT_hash_handle hh;
} LabelEntry;

typedef struct LabelResolver
{
	V2MPAsm_ParseContext* context;
	V2MPAsm_CWDList* cwdList;
	LabelEntry* entryHead;
} LabelResolver;

static LabelResolver* CreateLabelResolver()
{
	LabelResolver* resolver = BASEUTIL_CALLOC_STRUCT(LabelResolver);

	if ( !resolver )
	{
		return NULL;
	}

	return resolver;
}

static void DestroyLabelResolver(LabelResolver* resolver)
{
	if ( !resolver )
	{
		return;
	}

	HASH_CLEAR(hh, resolver->entryHead);
	BASEUTIL_FREE(resolver);
}

static void AddLabel(LabelResolver* resolver, V2MPAsm_CWDLabel* cwdLabel)
{
	LabelEntry* entry = (LabelEntry*)BASEUTIL_MALLOC(sizeof(LabelEntry));

	if ( !entry )
	{
		return;
	}

	entry->labelName = cwdLabel->name;
	entry->cwdLabel = cwdLabel;

	HASH_ADD_STR(resolver->entryHead, labelName, entry);
}

static LabelEntry* FindLabel(LabelResolver* resolver, const char* name)
{
	LabelEntry* entry = NULL;

	HASH_FIND_STR(resolver->entryHead, name, entry);
	return entry;
}

static bool PopulateLabelTable(LabelResolver* resolver)
{
	size_t numCodewords;
	size_t index;
	bool encounteredError = false;

	numCodewords = V2MPAsm_CWDList_GetSize(resolver->cwdList);

	for ( index = 0; index < numCodewords; ++index )
	{
		V2MPAsm_CWDLabel* labelCWD;
		LabelEntry* entry;

		labelCWD = V2MPAsm_CWDLabel_Cast(V2MPAsm_CWDList_GetItem(resolver->cwdList, index));

		if ( !labelCWD )
		{
			continue;
		}

		entry = FindLabel(resolver, V2MPAsm_CWDLabel_GetName(labelCWD));

		if ( entry )
		{
			V2MPAsm_ParseContext_ExceptionNode* node;

			encounteredError = true;

			node = V2MPAsm_ParseContext_AppendException(resolver->context);

			if ( node )
			{
				V2MPAsm_ParseException_SetError(node->exception, PARSEERROR_DUPLICATE_LABEL);

				V2MPAsm_ParseException_SetLineAndColumn(
					node->exception,
					V2MPAsm_CWDLabel_GetLine(labelCWD),
					0
				);

				// TODO: Improve this error message with variadic arguments.
				V2MPAsm_ParseException_SetCustomDescription(node->exception, "Encountered duplicate label.");
			}
			else
			{
				PARSECONTEXT_INTERNAL_ERROR(
					resolver->context,
					"Could not allocate memory for exception."
				);

				break;
			}
		}
		else
		{
			AddLabel(resolver, labelCWD);
		}
	}

	return !encounteredError;
}

static bool ResolveLabelAddresses(LabelResolver* resolver)
{
	// TODO

	V2MPAsm_ParseContext_TerminateWithError(
		resolver->context,
		PARSEERROR_UNIMPLEMENTED,
		"Need to implement resolving label addresses."
	);

	return false;
}

static void ResolveLabelReferences(LabelResolver* resolver)
{
	if ( !PopulateLabelTable(resolver) )
	{
		return;
	}

	ResolveLabelAddresses(resolver);
}

void V2MPAsm_Parser_ResolveReferences(V2MPAsm_Parser* parser)
{
	LabelResolver* resolver;

	// Sanity:
	if ( V2MPAsm_ParseContext_GetParseState(parser->context) != PARSESTATE_RESOLVING_REFERENCES )
	{
		PARSECONTEXT_INTERNAL_ERROR(
			parser->context,
			"Reference resolution routines triggered when the parser was in an incorrect state."
		);

		return;
	}

	resolver = CreateLabelResolver();

	if ( !resolver )
	{
		PARSECONTEXT_INTERNAL_ERROR(
			parser->context,
			"Could not create label resolver."
		);

		return;
	}

	resolver->context = parser->context;
	resolver->cwdList = V2MPAsm_ParseContext_GetCWDList(parser->context);

	ResolveLabelReferences(resolver);

	V2MPAsm_ParseContext_SetParseState(resolver->context, PARSESTATE_TERMINATED);
	DestroyLabelResolver(resolver);
}
