#ifndef V2MPASM_CWD_BASE_H
#define V2MPASM_CWD_BASE_H

typedef enum V2MP_CWD_Type
{
	CWD_INVALID = 0,
	CWD_INSTRUCTION,
	CWD_LABEL
} V2MP_CWD_Type;

typedef struct V2MP_CWD_Base
{
	V2MP_CWD_Type type;
} V2MP_CWD_Base;

V2MP_CWD_Type V2MP_CWD_GetType(const V2MP_CWD_Base* cwdBase);

#endif // V2MPASM_CWD_BASE_H
