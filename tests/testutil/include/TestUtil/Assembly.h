#pragma once

// This file duplicates the V2MP instruction defs by design.
// This set of definitions is completely separate from the
// V2MP library definitions, so that tests can define their
// expectations about how assembly should look without
// relying on what is defined in the library's headers.

namespace Asm
{
	using Byte = uint8_t;
	using Word = uint16_t;

	constexpr uint8_t REG_R0 = 0;
	constexpr uint8_t REG_R1 = 1;
	constexpr uint8_t REG_LR = 2;
	constexpr uint8_t REG_PC = 3;

	constexpr Word SR_Z = 1 << 0;
	constexpr Word SR_C = 1 << 1;

	enum class Instruction : Word
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
		SIG = 0xB,
		UNASSIGNED0 = 0xC,
		UNASSIGNED1 = 0xD,
		UNASSIGNED2 = 0xE,
		UNASSIGNED3 = 0xF
	};

	enum class BitwiseOp : uint8_t
	{
		AND = 0x0,
		OR = 0x1,
		XOR = 0x2,
		NOT = 0x3
	};

	constexpr inline Word Instr(Instruction in)
	{
		return static_cast<Word>(in) << 12;
	}

	constexpr inline Word FaultFromWord(Word faultWord)
	{
		return (faultWord & 0xF000) >> 12;
	}

	constexpr inline Word FaultArgsFromWord(Word faultWord)
	{
		return faultWord & 0x0FFF;
	}

	constexpr inline Word NOP()
	{
		return Instr(Instruction::NOP);
	}

	constexpr inline Word LOAD(uint8_t destReg)
	{
		return Instr(Instruction::LDST) | ((static_cast<Word>(destReg & 0x3)) << 9);
	}

	constexpr inline Word STOR(uint8_t sourceReg)
	{
		return Instr(Instruction::LDST) | (1 << 11) | ((static_cast<Word>(sourceReg) & 0x3) << 9);
	}

	constexpr inline Word ASGNR(uint8_t sourceReg, uint8_t destReg)
	{
		return Instr(Instruction::ASGN)
			| ((static_cast<Word>(sourceReg) & 0x3) << 10)
			| ((static_cast<Word>(destReg) & 0x3) << 8);
	}

	constexpr inline Word IASGNL(uint8_t destReg, int8_t literal)
	{
		return Instr(Instruction::ASGN)
			| ((static_cast<Word>(destReg) & 0x3) << 10)
			| ((static_cast<Word>(destReg) & 0x3) << 8)
			| static_cast<Word>(static_cast<uint8_t>(literal));
	}

	constexpr inline Word ASGNL(uint8_t destReg, uint8_t literal)
	{
		return Instr(Instruction::ASGN)
			| ((static_cast<Word>(destReg) & 0x3) << 10)
			| ((static_cast<Word>(destReg) & 0x3) << 8)
			| static_cast<Word>(literal);
	}

	constexpr inline Word ADDR(uint8_t sourceReg, uint8_t destReg)
	{
		return Instr(Instruction::ADD)
			| ((static_cast<Word>(sourceReg) & 0x3) << 10)
			| ((static_cast<Word>(destReg) & 0x3) << 8);
	}

	constexpr inline Word ADDL(uint8_t destReg, uint8_t literal)
	{
		return Instr(Instruction::ADD)
			| ((static_cast<Word>(destReg) & 0x3) << 10)
			| ((static_cast<Word>(destReg) & 0x3) << 8)
			| static_cast<Word>(literal);
	}

	constexpr inline Word SUBR(uint8_t sourceReg, uint8_t destReg)
	{
		return Instr(Instruction::SUB)
			| ((static_cast<Word>(sourceReg) & 0x3) << 10)
			| ((static_cast<Word>(destReg) & 0x3) << 8);
	}

	constexpr inline Word SUBL(uint8_t destReg, uint8_t literal)
	{
		return Instr(Instruction::SUB)
			| ((static_cast<Word>(destReg) & 0x3) << 10)
			| ((static_cast<Word>(destReg) & 0x3) << 8)
			| static_cast<Word>(literal);
	}

	constexpr inline Word SHFTR(uint8_t sourceReg, uint8_t destReg)
	{
		return Instr(Instruction::SHFT)
			| ((static_cast<Word>(sourceReg) & 0x3) << 10)
			| ((static_cast<Word>(destReg) & 0x3) << 8);
	}

	constexpr inline Word SHFTL(uint8_t destReg, int8_t shift)
	{
		return Instr(Instruction::SHFT)
			| ((static_cast<Word>(destReg) & 0x3) << 10)
			| ((static_cast<Word>(destReg) & 0x3) << 8)
			| static_cast<Word>(static_cast<uint8_t>(shift) & 0x1F);
	}

	constexpr inline Word BITWR(uint8_t sourceReg, uint8_t destReg, BitwiseOp operation)
	{
		return Instr(Instruction::BITW)
			| ((static_cast<Word>(sourceReg) & 0x3) << 10)
			| ((static_cast<Word>(destReg) & 0x3) << 8)
			| ((static_cast<Word>(operation) & 0x3) << 6);
	}

	constexpr inline Word BITWL(uint8_t destReg, BitwiseOp operation, uint8_t shift, bool negate)
	{
		return Instr(Instruction::BITW)
			| ((static_cast<Word>(destReg) & 0x3) << 10)
			| ((static_cast<Word>(destReg) & 0x3) << 8)
			| ((static_cast<Word>(operation) & 0x3) << 6)
			| (static_cast<Word>(negate ? 0x1 : 0x0) << 5)
			| (static_cast<Word>(shift) & 0xF);
	}

	constexpr inline Word BXZR()
	{
		return Instr(Instruction::CBX)
			| static_cast<Word>(1 << 11);
	}

	constexpr inline Word BXCR()
	{
		return Instr(Instruction::CBX)
			| static_cast<Word>(1 << 11)
			| static_cast<Word>(1 << 10);
	}

	constexpr inline Word BXZL(int8_t deltaWords)
	{
		return Instr(Instruction::CBX)
			| (static_cast<Word>(deltaWords) & 0x00FF);
	}

	constexpr inline Word BXCL(int8_t deltaWords)
	{
		return Instr(Instruction::CBX)
			| static_cast<Word>(1 << 10)
			| (static_cast<Word>(deltaWords) & 0x00FF);
	}

	constexpr inline Word PUSH(uint8_t regFlags)
	{
		return Instr(Instruction::STK)
			| (static_cast<Word>(1 << 11))
			| (static_cast<Word>(regFlags) & 0x000F);
	}

	constexpr inline Word POP(uint8_t regFlags)
	{
		return Instr(Instruction::STK)
			| (static_cast<Word>(regFlags) & 0x000F);
	}

	constexpr inline Word MULR(uint8_t destReg)
	{
		return Instr(Instruction::MUL)
			| (static_cast<Word>(destReg & 0x1) << 11);
	}

	constexpr inline Word MULL(uint8_t destReg, uint8_t multiplicand)
	{
		return Instr(Instruction::MUL)
			| (static_cast<Word>(destReg & 0x1) << 11)
			| static_cast<Word>(1 << 10)
			| (static_cast<Word>(multiplicand) & 0xFF);
	}

	constexpr inline Word IMULR(uint8_t destReg)
	{
		return Instr(Instruction::MUL)
			| (static_cast<Word>(destReg & 0x1) << 11)
			| static_cast<Word>(1 << 9);
	}

	constexpr inline Word IMULL(uint8_t destReg, int8_t multiplicand)
	{
		return Instr(Instruction::MUL)
			| (static_cast<Word>(destReg & 0x1) << 11)
			| static_cast<Word>(1 << 10)
			| static_cast<Word>(1 << 9)
			| (static_cast<Word>(multiplicand) & 0xFF);
	}

	constexpr inline Word DIVR(uint8_t destReg)
	{
		return Instr(Instruction::DIV)
			| (static_cast<Word>(destReg & 0x1) << 11);
	}

	constexpr inline Word DIVL(uint8_t destReg, uint8_t multiplicand)
	{
		return Instr(Instruction::DIV)
			| (static_cast<Word>(destReg & 0x1) << 11)
			| static_cast<Word>(1 << 10)
			| (static_cast<Word>(multiplicand) & 0xFF);
	}

	constexpr inline Word IDIVR(uint8_t destReg)
	{
		return Instr(Instruction::DIV)
			| (static_cast<Word>(destReg & 0x1) << 11)
			| static_cast<Word>(1 << 9);
	}

	constexpr inline Word IDIVL(uint8_t destReg, int8_t multiplicand)
	{
		return Instr(Instruction::DIV)
			| (static_cast<Word>(destReg & 0x1) << 11)
			| static_cast<Word>(1 << 10)
			| static_cast<Word>(1 << 9)
			| (static_cast<Word>(multiplicand) & 0xFF);
	}

	constexpr inline Word SIG()
	{
		return Instr(Instruction::SIG);
	}
}
