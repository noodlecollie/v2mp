#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>

namespace V2MPAsm
{
	enum class InstructionType : uint8_t
	{
		NOP = 0x0,
		ADD = 0x1,
		SUB = 0x2,
		MUL = 0x3,
		DIV = 0x4,
		ASGN = 0x5,
		SHFT = 0x6,
		BITW = 0x7,
		CBX = 0x8,
		LDST = 0x9,
		STK = 0xA,
		SIG = 0xB
	};

	struct ArgMeta
	{
		size_t highBit;
		size_t lowBit;
	};

	struct InstructionMeta
	{
		InstructionType type;
		std::vector<ArgMeta> args;
	};

	const InstructionMeta* GetInstructionMeta(const std::string& instructionName);
}
