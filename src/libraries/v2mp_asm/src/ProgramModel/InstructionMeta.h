#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>

namespace V2MPAsm
{
	// TODO: Would it be better to link the V2MP and V2MPAsm defs headers?
	// Otherwise we have this duplicated across both libraries...

	static constexpr uint32_t REG_ID_MASK = 0x3;
	static constexpr uint8_t REG_ID_R0 = 0;
	static constexpr uint8_t REG_ID_R1 = 1;
	static constexpr uint8_t REG_ID_LR = 2;
	static constexpr uint8_t REG_ID_PC = 3;

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

	enum InstructionArgFlag : uint32_t
	{
		// Presence = always signed, absence = always unsigned
		ARGFLAG_SIGNED = (1 << 0),

		// If set, signedness depends on other arguments in the code word
		ARGFLAG_DYNAMIC_SIGNEDNESS = (1 << 1),

		// Argument is symbolic (eg. a register ID)
		ARGFLAG_SYMBOLIC = (1 << 2),

		// Explicit placeholder for "no flags"
		ARGFLAG_UNSIGNED_NUMBER = 0
	};

	struct ArgMeta
	{
		uint8_t highBit;
		uint8_t lowBit;
		uint32_t flags;
	};

	struct InstructionMeta
	{
		std::string key;
		InstructionType type;
		std::vector<ArgMeta> args;
	};

	const InstructionMeta* GetInstructionMeta(const std::string& instructionName);
	const InstructionMeta& GetInstructionMeta(InstructionType instructionType);
}
