#ifndef V2MPASM_CWD_LABEL_H
#define V2MPASM_CWD_LABEL_H

#include <stdbool.h>
#include "CodewordDescriptors/CWD_Factory.h"

struct V2MPAsm_CWDBase;

typedef struct V2MPAsm_CWDLabel
{
	char* name;
	size_t line;
} V2MPAsm_CWDLabel;

extern const V2MP_CWD_Factory V2MP_CWDLabel_Factory;

V2MPAsm_CWDLabel* V2MPAsm_CWDLabel_Cast(struct V2MPAsm_CWDBase* cwdBase);

const char* V2MPAsm_CWDLabel_GetName(const V2MPAsm_CWDLabel* label);
bool V2MPAsm_CWDLabel_SetName(V2MPAsm_CWDLabel* label, const char* name);

size_t V2MPAsm_CWDLabel_GetLine(const V2MPAsm_CWDLabel* label);
void V2MPAsm_CWDLabel_SetLine(V2MPAsm_CWDLabel* label, size_t line);

#endif // V2MPASM_CWD_LABEL_H
