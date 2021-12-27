#ifndef V2MP_DEFS_H
#define V2MP_DEFS_H

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
	V2MP_OP_ASGN = 0x2,
	V2MP_OP_SHFT = 0x3,
	V2MP_OP_BITW = 0x4,
	V2MP_OP_CBX = 0x5,
	V2MP_OP_LDST = 0x6,
	V2MP_OP_DPQ = 0x7,
	V2MP_OP_DPO = 0x8,
	V2MP_OP_UNASSIGNED1 = 0x9,
	V2MP_OP_UNASSIGNED2 = 0xA,
	V2MP_OP_UNASSIGNED3 = 0xB,
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
	V2MP_FAULT_INO = 0x7,
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

#define V2MP_REGID_MASK(val) ((val) & 0x3)

#define V2MP_CPU_SR_Z (1 << 0)
#define V2MP_CPU_SR_C (1 << 1)

#define V2MP_CPU_MAKE_FAULT_WORD(fault, args) ( (V2MP_Word)(((fault) << 12) | ((args) & 0x0FFF)) )
#define V2MP_CPU_FAULT_CODE(faultWord) (((faultWord) & 0xF000) >> 12)
#define V2MP_CPU_FAULT_ARGS(faultWord) ((faultWord) & 0x0FFF)

#define V2MP_OPCODE(instr) ((instr) >> 12)

#define V2MP_OP_LDST_IS_STORE(instr) ((instr) & (1 << 11))
#define V2MP_OP_LDST_REGINDEX(instr) (((instr) & 0x0600) >> 9)
#define V2MP_OP_LDST_RESBITS(instr) ((instr) & 0x01FF)

#define V2MP_OP_ASGN_SREGINDEX(instr) (((instr) & 0x0C00) >> 10)
#define V2MP_OP_ASGN_DREGINDEX(instr) (((instr) & 0x0300) >> 8)
#define V2MP_OP_ASGN_VALUE(instr) ((instr) & 0x00FF)

#define V2MP_OP_ADDSUB_SREGINDEX(instr) (((instr) & 0x0C00) >> 10)
#define V2MP_OP_ADDSUB_DREGINDEX(instr) (((instr) & 0x0300) >> 8)
#define V2MP_OP_ADDSUB_VALUE(instr) ((instr) & 0x00FF)

#define V2MP_OP_SHFT_SREGINDEX(instr) (((instr) & 0x0C00) >> 10)
#define V2MP_OP_SHFT_DREGINDEX(instr) (((instr) & 0x0300) >> 8)
#define V2MP_OP_SHFT_VALUE(instr) ((instr) & 0x001F)
#define V2MP_OP_SHFT_RESBITS(instr) ((instr) & 0x00E0)

#define V2MP_OP_BITW_SREGINDEX(instr) (((instr) & 0x0C00) >> 10)
#define V2MP_OP_BITW_DREGINDEX(instr) (((instr) & 0x0300) >> 8)
#define V2MP_OP_BITW_OPTYPE(instr) (((instr) & 0x00C0) >> 6)
#define V2MP_OP_BITW_FLIPMASK(instr) ((instr) & 0x0020)
#define V2MP_OP_BITW_MASKSHIFT(instr) ((instr) & 0x000F)
#define V2MP_OP_BITW_RESBITS(instr) ((instr) & 0x0010)

#define V2MP_OP_CBX_LR_IS_TARGET(instr) ((instr) & 0x0800)
#define V2MP_OP_CBX_BRANCH_ON_SR_Z(instr) (((instr) & 0x0400) == 0)
#define V2MP_OP_CBX_BRANCH_ON_SR_C(instr) (((instr) & 0x0400) != 0)
#define V2MP_OP_CBX_OFFSET(instr) ((instr) & 0x00FF)
#define V2MP_OP_CBX_RESBITS(instr) ((instr) & 0x0300)

#define V2MP_OP_DPQ_QUERY_TYPE(instr) ((instr) & 0x0007)
#define V2MP_OP_DPQ_RESBITS(instr) ((instr) & 0x0FF8)

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MP_DEFS_H
