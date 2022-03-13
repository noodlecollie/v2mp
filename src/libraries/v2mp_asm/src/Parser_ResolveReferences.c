#include "Parser_ResolveReferences.h"
#include "Parser_Internal.h"
#include "ParseContext.h"
#include "BaseUtil/UTHash_V2MP.h"
#include "BaseUtil/Heap.h"
#include "CodewordDescriptors/CWD_Label.h"
#include "CodewordDescriptors/CWD_Instruction.h"

typedef struct LabelEntry
{
	const char* labelName;
	V2MPAsm_CWDLabel* cwdLabel;
	V2MPAsm_Word address;
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

static void AddLabel(LabelResolver* resolver, V2MPAsm_CWDLabel* cwdLabel, V2MPAsm_Word address)
{
	LabelEntry* entry = (LabelEntry*)BASEUTIL_MALLOC(sizeof(LabelEntry));

	if ( !entry )
	{
		return;
	}

	entry->labelName = cwdLabel->name;
	entry->cwdLabel = cwdLabel;
	entry->address = address;

	HASH_ADD_STR(resolver->entryHead, labelName, entry);
}

static LabelEntry* FindLabel(LabelResolver* resolver, const char* name)
{
	LabelEntry* entry = NULL;

	HASH_FIND_STR(resolver->entryHead, name, entry);
	return entry;
}

static void NotifyAddressSpaceExceeded(LabelResolver* resolver, size_t line)
{
	V2MPAsm_ParseContext_ExceptionNode* node;

	node = V2MPAsm_ParseContext_AppendException(resolver->context);

	if ( node )
	{
		V2MPAsm_ParseException_SetError(node->exception, PARSEERROR_ADDRESS_SPACE_EXCEEDED);

		V2MPAsm_ParseException_SetLineAndColumn(
			node->exception,
			line,
			0
		);

		V2MPAsm_ParseException_FormatCustomDescription(
			node->exception,
			"Program address space exceeded - maximum %u codewords.",
			(((V2MPAsm_Word)(~0) - 1) / 2)
		);
	}
	else
	{
		PARSECONTEXT_INTERNAL_ERROR(
			resolver->context,
			"Could not allocate memory for exception."
		);
	}
}

static bool PopulateLabelTable(LabelResolver* resolver)
{
	size_t numCodewords;
	size_t index;
	size_t currentAddress = 0;
	bool encounteredError = false;

	numCodewords = V2MPAsm_CWDList_GetSize(resolver->cwdList);

	for ( index = 0; index < numCodewords; ++index )
	{
		V2MPAsm_CWDLabel* labelCWD;
		LabelEntry* entry;

		labelCWD = V2MPAsm_CWDLabel_Cast(V2MPAsm_CWDList_GetItem(resolver->cwdList, index));

		if ( !labelCWD )
		{
			currentAddress += sizeof(V2MPAsm_Word);

			if ( currentAddress & ~(0xFFFF) )
			{
				// The codeword address space has been exceeded.
				encounteredError = true;
				NotifyAddressSpaceExceeded(resolver, V2MPAsm_CWDLabel_GetLine(labelCWD));
				break;
			}

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

				V2MPAsm_ParseException_FormatCustomDescription(
					node->exception,
					"Encountered duplicate label \"%s\". Label was originally declared on line %zu.",
					entry->labelName,
					V2MPAsm_CWDLabel_GetLine(entry->cwdLabel)
				);
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
			if ( currentAddress >= 0xFFFE )
			{
				NotifyAddressSpaceExceeded(resolver, V2MPAsm_CWDLabel_GetLine(labelCWD));
				break;
			}

			// The address that the label points to is that of the next instruction.
			AddLabel(resolver, labelCWD, (V2MPAsm_Word)(currentAddress + 2));
		}
	}

	return !encounteredError;
}

static bool ResolveLabelAddressesForArg(
	LabelResolver* resolver,
	V2MPAsm_CWDInstruction* instructionCWD,
	size_t codewordIndex,
	size_t argIndex
)
{
	V2MPAsm_CWDInstruction_Arg* arg;
	const V2MPAsm_InstructionArg* metaArg;
	size_t numBits;

	arg = V2MPAsm_CWDInstruction_GetInstructionArg(instructionCWD, argIndex);

	if ( !arg->isLabelRef )
	{
		// Nothing to do.
		return true;
	}

	metaArg = V2MPAsm_InstructionMeta_GetArg(V2MPAsm_CWDInstruction_GetInstructionMeta(instructionCWD), argIndex);

	if ( !metaArg )
	{
		PARSECONTEXT_INTERNAL_ERRORV(
			resolver->context,
			"Could not get meta-arg for codeword %zu argument %zu.",
			codewordIndex,
			argIndex
		);

		return false;
	}

	numBits = V2MPasm_InstructionArg_GetNumBits(metaArg);

	switch ( arg->value.labelRef.refType )
	{
		case LABELREF_DISTANCE:
		{
			// TODO: We need to know the address of the current instruction.
			break;
		}

		case LABELREF_MSB:
		{
			// TODO
			break;
		}

		case LABELREF_LSB:
		{
			// TODO
			break;
		}

		default:
		{
			PARSECONTEXT_INTERNAL_ERRORV(
				resolver->context,
				"Unrecognised label reference type %d for codeword %zu argument %zu.",
				arg->value.labelRef.refType,
				codewordIndex,
				argIndex
			);

			return false;
		}
	}
}

static bool ResolveLabelAddresses(LabelResolver* resolver)
{
	size_t numCodewords;
	size_t index;
	bool encounteredError = false;

	numCodewords = V2MPAsm_CWDList_GetSize(resolver->cwdList);

	for ( index = 0; index < numCodewords; ++index )
	{
		V2MPAsm_CWDInstruction* instructionCWD;
		size_t numArgs;
		size_t argIndex;

		instructionCWD = V2MPAsm_CWDInstruction_Cast(V2MPAsm_CWDList_GetItem(resolver->cwdList, index));

		if ( !instructionCWD )
		{
			continue;
		}

		numArgs = V2MPAsm_CWDInstruction_GetInstructionArgCount(instructionCWD);

		for ( argIndex = 0; argIndex < numArgs; ++argIndex )
		{
			if ( !ResolveLabelAddressesForArg(resolver, instructionCWD, index, argIndex) )
			{
				encounteredError = true;
			}

			if ( V2MPAsm_ParseContext_GetParseState(resolver->context) == PARSESTATE_TERMINATED )
			{
				// Something went badly wrong.
				break;
			}
		}

		if ( V2MPAsm_ParseContext_GetParseState(resolver->context) == PARSESTATE_TERMINATED )
		{
			// Something went badly wrong.
			break;
		}
	}

	return encounteredError;
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
