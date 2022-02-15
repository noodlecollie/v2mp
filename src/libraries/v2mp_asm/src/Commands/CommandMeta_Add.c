#include "Commands/CommandMeta_Add.h"
#include "BaseUtil/Util.h"

static const V2MPAsm_CommandArg Args[] =
{
	{ "SrcReg", 11, 2 },
	{ "DestReg", 9, 2 },
	{ "Literal", 7, 8 },
};

const V2MPAsm_CommandMeta V2MPAsm_CommandMeta_Add =
{
	"add",
	Args,
	BASEUTIL_ARRAY_SIZE(Args)
};
