#ifndef V2MPASM_DEFS_H
#define V2MPASM_DEFS_H

#ifdef __cplusplus
extern "C" {
#endif

#define V2MPASM_WARNING_LIST \
	LIST_ITEM(V2MPASM_WARNING_NONE = 0, "None")

#define V2MPASM_ERROR_LIST \
	LIST_ITEM(V2MPASM_ERROR_NONE = 0, "None")

#define LIST_ITEM(value, desc) value,
typedef enum V2MPAsm_AsmWarningType
{
	V2MPASM_WARNING_LIST
} V2MPAsm_AsmWarningType;
#undef LIST_ITEM

#define LIST_ITEM(value, desc) value,
typedef enum V2MPAsm_AsmErrorType
{
	V2MPASM_ERROR_LIST
} V2MPAsm_AsmErrorType;
#undef LIST_ITEM

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MPASM_DEFS_H
