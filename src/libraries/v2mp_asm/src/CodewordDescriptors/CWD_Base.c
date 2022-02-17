#include "CodewordDescriptors/CWD_Base.h"

V2MP_CWD_Type V2MP_CWD_GetType(const V2MP_CWD_Base* cwdBase)
{
	return cwdBase ? cwdBase->type : CWD_INVALID;
}
