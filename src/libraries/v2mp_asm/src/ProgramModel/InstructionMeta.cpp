#include <stdexcept>
#include "ProgramModel/InstructionMeta.h"

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
				{ 11, 10, ARGFLAG_SYMBOLIC },		// Source register
				{ 9, 8, ARGFLAG_SYMBOLIC },			// Dest register
				{ 7, 0, ARGFLAG_UNSIGNED_NUMBER }	// Increment literal
			}
		},
		{
			"sub",
			InstructionType::SUB,
			{
				{ 11, 10, ARGFLAG_SYMBOLIC },		// Source register
				{ 9, 8, ARGFLAG_SYMBOLIC },			// Dest register
				{ 7, 0, ARGFLAG_UNSIGNED_NUMBER }	// Decrement literal
			}
		},
		{
			"mul",
			InstructionType::MUL,
			{
				{ 11, 11, ARGFLAG_SYMBOLIC },			// Dest register
				{ 10, 10, ARGFLAG_SYMBOLIC },			// Source multiplicand
				{ 9, 9, ARGFLAG_SYMBOLIC },				// Signedness
				{ 7, 0, ARGFLAG_DYNAMIC_SIGNEDNESS }	// Multiplicand literal
			}
		},
		{
			"div",
			InstructionType::DIV,
			{
				{ 11, 11, ARGFLAG_SYMBOLIC },			// Dest register
				{ 10, 10, ARGFLAG_SYMBOLIC },			// Source divisor
				{ 9, 9, ARGFLAG_SYMBOLIC },				// Signedness
				{ 7, 0, ARGFLAG_DYNAMIC_SIGNEDNESS }	// Divisor literal
			}
		},
		{
			"asgn",
			InstructionType::ASGN,
			{
				{ 11, 10, ARGFLAG_SYMBOLIC },	// Source register
				{ 9, 8, ARGFLAG_SYMBOLIC },		// Dest register
				{ 7, 0, ARGFLAG_SIGNED }		// Assignment literal
			}
		},
		{
			"shft",
			InstructionType::SHFT,
			{
				{ 11, 10, ARGFLAG_SYMBOLIC },	// Magnitude register
				{ 9, 8, ARGFLAG_SYMBOLIC },		// Target register
				{ 4, 0, ARGFLAG_SIGNED }		// Shift literal
			}
		},
		{
			"bitw",
			InstructionType::BITW,
			{
				{ 11, 10, ARGFLAG_SYMBOLIC },		// Bitmask source register
				{ 9, 8, ARGFLAG_SYMBOLIC },			// Dest register
				{ 7, 6, ARGFLAG_SYMBOLIC },			// Operation type
				{ 5, 5, ARGFLAG_SYMBOLIC },			// Bitmask construction invert flag
				{ 3, 0, ARGFLAG_UNSIGNED_NUMBER }	// Bitmask construction shift magnitude
			}
		},
		{
			"cbx",
			InstructionType::CBX,
			{
				{ 11, 11, ARGFLAG_SYMBOLIC },	// PC assign method
				{ 10, 10, ARGFLAG_SYMBOLIC },	// Status condition
				{ 7, 0, ARGFLAG_SIGNED }		// PC offset literal
			}
		},
		{
			"ldst",
			InstructionType::LDST,
			{
				{ 11, 11, ARGFLAG_SYMBOLIC },	// Operation type
				{ 10, 9, ARGFLAG_SYMBOLIC }		// Target register
			}
		},
		{
			"stk",
			InstructionType::STK,
			{
				{ 11, 11, ARGFLAG_SYMBOLIC },	// Operation type
				{ 3, 0, ARGFLAG_SYMBOLIC },		// Register bitmask
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

	const InstructionMeta& GetInstructionMeta(InstructionType instructionType)
	{
		for ( const InstructionMeta& meta : g_Instructions )
		{
			if ( meta.type == instructionType )
			{
				return meta;
			}
		}

		throw std::logic_error("Missing instruction type");
	}
}
