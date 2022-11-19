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
				{ 11, 10, ArgSignedness::ALWAYS_UNSIGNED },	// Source register
				{ 9, 8, ArgSignedness::ALWAYS_UNSIGNED },	// Dest register
				{ 7, 0, ArgSignedness::ALWAYS_UNSIGNED }	// Increment literal
			}
		},
		{
			"sub",
			InstructionType::SUB,
			{
				{ 11, 10, ArgSignedness::ALWAYS_UNSIGNED },	// Source register
				{ 9, 8, ArgSignedness::ALWAYS_UNSIGNED },	// Dest register
				{ 7, 0, ArgSignedness::ALWAYS_UNSIGNED }	// Decrement literal
			}
		},
		{
			"mul",
			InstructionType::MUL,
			{
				{ 11, 11, ArgSignedness::ALWAYS_UNSIGNED },	// Dest register
				{ 10, 10, ArgSignedness::ALWAYS_UNSIGNED },	// Source multiplicand
				{ 9, 9, ArgSignedness::ALWAYS_UNSIGNED },	// Signedness
				{ 7, 0, ArgSignedness::DYNAMIC_SIGNEDNESS }	// Multiplicand literal
			}
		},
		{
			"div",
			InstructionType::DIV,
			{
				{ 11, 11, ArgSignedness::ALWAYS_UNSIGNED },	// Dest register
				{ 10, 10, ArgSignedness::ALWAYS_UNSIGNED },	// Source divisor
				{ 9, 9, ArgSignedness::ALWAYS_UNSIGNED },	// Signedness
				{ 7, 0, ArgSignedness::DYNAMIC_SIGNEDNESS }	// Divisor literal
			}
		},
		{
			"asgn",
			InstructionType::ASGN,
			{
				{ 11, 10, ArgSignedness::ALWAYS_UNSIGNED },	// Source register
				{ 9, 8, ArgSignedness::ALWAYS_UNSIGNED },	// Dest register
				{ 7, 0, ArgSignedness::ALWAYS_SIGNED }		// Assignment literal
			}
		},
		{
			"shft",
			InstructionType::SHFT,
			{
				{ 11, 10, ArgSignedness::ALWAYS_UNSIGNED },	// Magnitude register
				{ 9, 8, ArgSignedness::ALWAYS_UNSIGNED },	// Target register
				{ 4, 0, ArgSignedness::ALWAYS_SIGNED }		// Shift literal
			}
		},
		{
			"bitw",
			InstructionType::BITW,
			{
				{ 11, 10, ArgSignedness::ALWAYS_UNSIGNED },	// Bitmask source register
				{ 9, 8, ArgSignedness::ALWAYS_UNSIGNED },	// Dest register
				{ 7, 6, ArgSignedness::ALWAYS_UNSIGNED },	// Operation type
				{ 5, 5, ArgSignedness::ALWAYS_UNSIGNED },	// Bitmask construction invert flag
				{ 3, 0, ArgSignedness::ALWAYS_UNSIGNED }	// Bitmask construction shift magnitude
			}
		},
		{
			"cbx",
			InstructionType::CBX,
			{
				{ 11, 11, ArgSignedness::ALWAYS_UNSIGNED },	// PC assign method
				{ 10, 10, ArgSignedness::ALWAYS_UNSIGNED },	// Status condition
				{ 7, 0, ArgSignedness::ALWAYS_SIGNED }		// PC offset literal
			}
		},
		{
			"ldst",
			InstructionType::LDST,
			{
				{ 11, 11, ArgSignedness::ALWAYS_UNSIGNED },	// Operation type
				{ 10, 9, ArgSignedness::ALWAYS_UNSIGNED }	// Target register
			}
		},
		{
			"stk",
			InstructionType::STK,
			{
				{ 11, 11, ArgSignedness::ALWAYS_UNSIGNED },	// Operation type
				{ 3, 0, ArgSignedness::ALWAYS_UNSIGNED },	// Register bitmask
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
