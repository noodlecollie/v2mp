#include "BaseUtil/Util.h"
#include "CodewordDescriptors/CWD_Factory.h"
#include "CodewordDescriptors/CWD_Instruction.h"

const V2MP_CWD_Factory* V2MPAsm_CWD_GetFactory(V2MPAsm_CWD_Type type)
{
#define LIST_ITEM(value, factory) factory,
	static const V2MP_CWD_Factory* FACTORIES[] =
	{
		V2MP_CWD_TYPE_LIST
	};
#undef LIST_ITEM

	return (size_t)type < BASEUTIL_ARRAY_SIZE(FACTORIES)
		? FACTORIES[(size_t)type]
		: NULL;
}
