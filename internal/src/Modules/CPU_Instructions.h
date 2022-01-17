#ifndef V2MP_MODULES_CPU_INSTRUCTIONS_H
#define V2MP_MODULES_CPU_INSTRUCTIONS_H

#include <stdbool.h>
#include "V2MPInternal/Defs.h"
#include "V2MPInternal/Modules/CPU.h"

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
#define V2MP_OP_BITW_FLIPMASK(instr) (((instr) & 0x0020) != 0)
#define V2MP_OP_BITW_MASKSHIFT(instr) ((instr) & 0x000F)
#define V2MP_OP_BITW_RESBITS(instr) ((instr) & 0x0010)

#define V2MP_OP_CBX_LR_IS_TARGET(instr) (((instr) & 0x0800) != 0)
#define V2MP_OP_CBX_BRANCH_ON_SR_Z(instr) (((instr) & 0x0400) == 0)
#define V2MP_OP_CBX_BRANCH_ON_SR_C(instr) (((instr) & 0x0400) != 0)
#define V2MP_OP_CBX_OFFSET(instr) ((instr) & 0x00FF)
#define V2MP_OP_CBX_RESBITS(instr) ((instr) & 0x0300)

#define V2MP_OP_DPQ_QUERY_TYPE(instr) ((instr) & 0x0007)
#define V2MP_OP_DPQ_RESBITS(instr) ((instr) & 0x0FF8)

#define V2MP_OP_DPO_INDIRECT_TRANSFER(instr) (((instr) & 0x0800) != 0)
#define V2MP_OP_DPO_OPERATION_TYPE(instr) ((instr) & 0x0003)
#define V2MP_OP_DPO_RESBITS(instr) ((instr) & 0x07FC)

#define V2MP_OP_STK_PUSH(instr) (((instr) & 0x0800) != 0)
#define V2MP_OP_STK_R0(instr) (((instr) & 0x0008) != 0)
#define V2MP_OP_STK_R1(instr) (((instr) & 0x0004) != 0)
#define V2MP_OP_STK_LR(instr) (((instr) & 0x0002) != 0)
#define V2MP_OP_STK_PC(instr) (((instr) & 0x0001) != 0)
#define V2MP_OP_STK_RESBITS(instr) ((instr) & 0x07F0)

bool V2MP_CPU_ExecuteInstructionInternal(V2MP_CPU* cpu);

#endif // V2MP_MODULES_CPU_INSTRUCTIONS_H
