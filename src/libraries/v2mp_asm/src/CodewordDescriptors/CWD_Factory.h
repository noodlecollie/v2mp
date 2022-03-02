#ifndef V2MPASM_CWD_FACTORY_H
#define V2MPASM_CWD_FACTORY_H

#include <stdlib.h>

#define V2MP_CWD_TYPE_LIST \
	LIST_ITEM(V2MPASM_CWD_INVALID = 0, NULL) \
	LIST_ITEM(V2MPASM_CWD_INSTRUCTION, &V2MP_CWDInstruction_Factory) \
	LIST_ITEM(V2MPASM_CWD_LABEL, &V2MP_CWDLabel_Factory)

#define LIST_ITEM(value, factory) value,
typedef enum V2MPAsm_CWD_Type
{
	V2MP_CWD_TYPE_LIST
} V2MPAsm_CWD_Type;
#undef LIST_ITEM

typedef struct V2MP_CWD_Factory
{
	void* (*create)(void);
	void (*destroy)(void* ptr);
} V2MP_CWD_Factory;

const V2MP_CWD_Factory* V2MPAsm_CWD_GetFactory(V2MPAsm_CWD_Type type);

#endif // V2MPASM_CWD_FACTORY_H
