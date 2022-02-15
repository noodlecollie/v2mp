#include "Instructions/InstructionMeta_Add.h"
#include "BaseUtil/Util.h"

static const V2MPAsm_InstructionArg Args[] =
{
	{ "SrcReg", 11, 2 },
	{ "DestReg", 9, 2 },
	{ "Literal", 7, 8 },
};

const V2MPAsm_InstructionMeta V2MPAsm_InstructionMeta_Add =
{
	"add",
	Args,
	BASEUTIL_ARRAY_SIZE(Args)
};
