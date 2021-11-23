#ifndef V2MP_MCU_H
#define V2MP_MCU_H

#include <stdint.h>

#define MAX_IDC_PORTS 16

typedef uint16_t V2MP_Word;

typedef enum _V2MP_Opcode
{
	V2MP_OP_HALT = 0x0,
	V2MP_OP_LOAD = 0x1,
	V2MP_OP_STORE = 0x2,
	V2MP_OP_ADD = 0x3,
	V2MP_OP_SUBTRACT = 0x4,
	V2MP_OP_MULTIPLY = 0x5,
	V2MP_OP_DIVIDE = 0x6,
	V2MP_OP_LSHIFT = 0x7,
	V2MP_OP_RSHIFT = 0x8,
	V2MP_OP_BRANCH_IF_ZERO = 0x9,
	V2MP_OP_BRANCH_IF_NOT_ZERO = 0xA,
	V2MP_OP_JUMP = 0xB,
	V2MP_OP_PUSH = 0xC,
	V2MP_OP_POP = 0xD,
	V2MP_OP_IDC = 0xE,
	V2MP_OP_UNUSED1 = 0xF
} V2MP_Opcode;

typedef struct _V2MP_Mem_Segment
{
	V2MP_Word* data;
	V2MP_Word lengthInWords;
} V2MP_Mem_Segment;

typedef struct _V2MP_Mem_RSegment
{
	const V2MP_Word* data;
	V2MP_Word lengthInWords;
} V2MP_Mem_RSegment;

typedef struct _V2MP_MemMap
{
	V2MP_Mem_RSegment codeSeg;
	V2MP_Mem_Segment ramSeg;
} V2MP_MemMap;

// Inter-Device Request port
typedef struct _V2MP_IDCPort
{
	V2MP_Word reqOrdinal;
	V2MP_Word argsRamAddr;

	// To make a request, set the ordinal and address in RAM that
	// holds args (optional as per the request), then set bit 15.
	// Once bit 15 is clear again, the request is complete. The
	// other lower bits [0 14] will contain the response to the
	// request.
	V2MP_Word status;
} V2MP_IDCPort;

typedef struct _V2MP_IDC
{
	// By convention, port 0 is always the supervisor port,
	// and IDC request 0 on any port always returns the
	// version of the request interface being used.
	V2MP_IDCPort ports[MAX_IDC_PORTS];
} V2MP_IDC;

typedef struct _V2MP_CPU
{
	V2MP_Word programCounter;
	V2MP_Word instruction;
	V2MP_Word status;
	V2MP_Word register0;
	V2MP_Word register1;
} V2MP_CPU;

#endif // V2MP_MCU_H
