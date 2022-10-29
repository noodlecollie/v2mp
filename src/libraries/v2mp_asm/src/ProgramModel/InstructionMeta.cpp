#include "ProgramModel/InstructionMeta.h"
#include "Utils/StringUtils.h"

namespace V2MPAsm
{
	const std::vector<InstructionMeta> g_Instructions =
	{
		{
			"nop",
			InstructionType::NOP,
			{
			}
		},
		{
			"add",
			InstructionType::ADD,
			{
				{ 11, 10 },	// Source register
				{ 9, 8 },	// Dest register
				{ 7, 0 }	// Increment literal
			}
		},
		{
			"sub",
			InstructionType::SUB,
			{
				{ 11, 10 },	// Source register
				{ 9, 8 },	// Dest register
				{ 7, 0 }	// Decrement literal
			}
		},
		{
			"mul",
			InstructionType::MUL,
			{
				{ 11, 11 },	// Dest register
				{ 10, 10 },	// Source multiplicand
				{ 9, 9 },	// Signedness
				{ 7, 0 }	// Multiplicand literal
			}
		},
		{
			"div",
			InstructionType::DIV,
			{
				{ 11, 11 },	// Dest register
				{ 10, 10 },	// Source divisor
				{ 9, 9 },	// Signedness
				{ 7, 0 }	// Divisor literal
			}
		},
		{
			"asgn",
			InstructionType::ASGN,
			{
				{ 11, 10 },	// Source register
				{ 9, 8 },	// Dest register
				{ 7, 0 }	// Assignment literal
			}
		},
		{
			"shft",
			InstructionType::SHFT,
			{
				{ 11, 10 },	// Magnitude register
				{ 9, 8 },	// Target register
				{ 4, 0 }	// Shift literal
			}
		},
		{
			"bitw",
			InstructionType::BITW,
			{
				{ 11, 10 },	// Bitmask source register
				{ 9, 8 },	// Dest register
				{ 7, 6 },	// Operation type
				{ 5, 5 },	// Bitmask construction invert flag
				{ 3, 0 }	// Bitmask construction shift magnitude
			}
		},
		{
			"cbx",
			InstructionType::CBX,
			{
				{ 11, 11 },	// PC assign method
				{ 10, 10 },	// Status condition
				{ 7, 0 }	// PC offset literal
			}
		},
		{
			"ldst",
			InstructionType::LDST,
			{
				{ 11, 11 },	// Operation type
				{ 10, 9 }	// Target register
			}
		},
		{
			"stk",
			InstructionType::STK,
			{
				{ 11, 11 },	// Operation type
				{ 3, 0 },	// Register bitmask
			}
		},
		{
			"sig",
			InstructionType::SIG,
			{
			}
		}
	};

	const InstructionMeta* GetInstructionMeta(const std::string& instructionName)
	{
		for ( const InstructionMeta& meta : g_Instructions )
		{
			if ( meta.key == instructionName )
			{
				return &meta;
			}
		}

		return nullptr;
	}
}
