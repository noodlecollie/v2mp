#ifndef V2MPASM_TOKENS_TOKENMETA_LABELREFERENCE_H
#define V2MPASM_TOKENS_TOKENMETA_LABELREFERENCE_H

#include "Tokens/TokenMeta.h"

typedef enum V2MPAsm_LabelReferenceType
{
	LABELREF_INVALID = 0,
	LABELREF_DISTANCE,
	LABELREF_MSB,
	LABELREF_LSB
} V2MPAsm_LabelReferenceType;

extern const V2MPAsm_TokenMeta V2MPAsm_TokenMeta_LabelReference;

V2MPAsm_LabelReferenceType V2MPAsm_LabelReference_GetReferenceType(const char* token);
const char* V2MPAsm_LabelReference_GetLabelName(const char* token);

#endif // V2MPASM_TOKENS_TOKENMETA_LABELREFERENCE_H
