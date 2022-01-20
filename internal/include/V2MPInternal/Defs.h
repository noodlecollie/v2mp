#ifndef V2MPINTERNAL_DEFS_H
#define V2MPINTERNAL_DEFS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint16_t V2MP_Word;
typedef uint8_t V2MP_Byte;

typedef enum V2MP_Instruction
{
	V2MP_OP_ADD = 0x0,
	V2MP_OP_SUB = 0x1,
	V2MP_OP_MUL = 0x2,
	V2MP_OP_DIV = 0x3,
	V2MP_OP_ASGN = 0x4,
	V2MP_OP_SHFT = 0x5,
	V2MP_OP_BITW = 0x6,
	V2MP_OP_CBX = 0x7,
	V2MP_OP_LDST = 0x8,
	V2MP_OP_DPQ = 0x9,
	V2MP_OP_DPO = 0xA,
	V2MP_OP_STK = 0xB,
	V2MP_OP_UNASSIGNED4 = 0xC,
	V2MP_OP_UNASSIGNED5 = 0xD,
	V2MP_OP_UNASSIGNED6 = 0xE,
	V2MP_OP_HCF = 0xF
} V2MP_Instruction;

typedef enum V2MP_Fault
{
	V2MP_FAULT_NONE = 0x0,
	V2MP_FAULT_HCF = 0x1,
	V2MP_FAULT_RES = 0x2,
	V2MP_FAULT_ALGN = 0x3,
	V2MP_FAULT_SEG = 0x4,
	V2MP_FAULT_IDO = 0x5,
	V2MP_FAULT_INI = 0x6,
	V2MP_FAULT_SPV = 0x7,
	V2MP_FAULT_DEV = 0x8,
	V2MP_FAULT_SOF = 0x9
} V2MP_Fault;

typedef enum V2MP_RegisterIndex
{
	V2MP_REGID_R0 = 0x0,
	V2MP_REGID_R1 = 0x1,
	V2MP_REGID_LR = 0x2,
	V2MP_REGID_PC = 0x3
} V2MP_RegisterIndex;

typedef enum V2MP_BitwiseOp
{
	V2MP_BITOP_AND = 0x0,
	V2MP_BITOP_OR = 0x1,
	V2MP_BITOP_XOR = 0x2,
	V2MP_BITOP_NOT = 0x3
} V2MP_BitwiseOp;

typedef enum V2MP_DevicePortQueryType
{
	V2MP_DPQT_CONNECTED = 0x0,
	V2MP_DPQT_READABLE_NOT_BUSY = 0x1,
	V2MP_DPQT_WRITEABLE_NOT_BUSY = 0x2,
	V2MP_DPQT_EXHAUSTED = 0x3,
	V2MP_DPQT_BUSY = 0x4,
	V2MP_DPQT_CONTROLLED_BY_PROGRAM = 0x5,
} V2MP_DevicePortQueryType;

typedef enum V2MP_DevicePortOperationType
{
	V2MP_DPOT_USABLE_BYTE_COUNT = 0x0,
	V2MP_DPOT_RELINQUISH_MAILBOX = 0x1,
	V2MP_DPOT_READ = 0x2,
	V2MP_DPOT_WRITE = 0x3
} V2MP_DevicePortOperationType;

typedef enum V2MP_DeviceMailboxController
{
	V2MP_DPMC_DEVICE = 0,
	V2MP_DPMC_PROGRAM
} V2MP_DeviceMailboxController;

typedef enum V2MP_DevicePortMailboxState
{
	V2MP_DPMS_UNAVAILABLE = 0,
	V2MP_DPMS_READABLE,
	V2MP_DPMS_WRITEABLE,
	V2MP_DPMS_EXHAUSTED
} V2MP_DevicePortMailboxState;

#define V2MP_REGID_MAX 3
#define V2MP_REGID_MASK(val) ((val) & V2MP_REGID_MAX)

#define V2MP_CPU_SR_Z (1 << 0)
#define V2MP_CPU_SR_C (1 << 1)

#define V2MP_CPU_MAKE_FAULT_WORD(fault, args) ( (V2MP_Word)(((fault) << 12) | ((args) & 0x0FFF)) )
#define V2MP_CPU_FAULT_CODE(faultWord) (((faultWord) & 0xF000) >> 12)
#define V2MP_CPU_FAULT_ARGS(faultWord) ((faultWord) & 0x0FFF)

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MPINTERNAL_DEFS_H
