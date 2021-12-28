#pragma once

#ifdef OLD_TEST
#include "V2MP/Defs.h"
#else
#include "V2MPInternal/Defs.h"
#endif

namespace Asm
{
	constexpr uint8_t REG_R0 = 0;
	constexpr uint8_t REG_R1 = 1;
	constexpr uint8_t REG_LR = 2;
	constexpr uint8_t REG_PC = 3;

	constexpr V2MP_Word SR_Z = 1 << 0;
	constexpr V2MP_Word SR_C = 1 << 1;

	enum class BitwiseOp : uint8_t
	{
		AND = 0x0,
		OR = 0x1,
		XOR = 0x2,
		NOT = 0x3
	};

	constexpr inline V2MP_Word FaultFromWord(V2MP_Word faultWord)
	{
		return (faultWord & 0xF000) >> 12;
	}

	constexpr inline V2MP_Word FaultArgsFromWord(V2MP_Word faultWord)
	{
		return faultWord & 0x0FFF;
	}

	constexpr inline V2MP_Word HCF(V2MP_Word args = 0)
	{
		return (V2MP_OP_HCF << 12) | (args & 0x0FFF);
	}

	constexpr inline V2MP_Word LOAD(uint8_t destReg)
	{
		return (V2MP_OP_LDST << 12) | ((static_cast<V2MP_Word>(destReg & 0x3)) << 9);
	}

	constexpr inline V2MP_Word STOR(uint8_t sourceReg)
	{
		return (V2MP_OP_LDST << 12) | (1 << 11) | ((static_cast<V2MP_Word>(sourceReg) & 0x3) << 9);
	}

	constexpr inline V2MP_Word ASGNR(uint8_t sourceReg, uint8_t destReg)
	{
		return (V2MP_OP_ASGN << 12)
			| ((static_cast<V2MP_Word>(sourceReg) & 0x3) << 10)
			| ((static_cast<V2MP_Word>(destReg) & 0x3) << 8);
	}

	constexpr inline V2MP_Word ASGNL(uint8_t destReg, int8_t literal)
	{
		return (V2MP_OP_ASGN << 12)
			| ((static_cast<V2MP_Word>(destReg) & 0x3) << 10)
			| ((static_cast<V2MP_Word>(destReg) & 0x3) << 8)
			| static_cast<V2MP_Word>(static_cast<uint8_t>(literal));
	}

	constexpr inline V2MP_Word ADDR(uint8_t sourceReg, uint8_t destReg)
	{
		return (V2MP_OP_ADD << 12)
			| ((static_cast<V2MP_Word>(sourceReg) & 0x3) << 10)
			| ((static_cast<V2MP_Word>(destReg) & 0x3) << 8);
	}

	constexpr inline V2MP_Word ADDL(uint8_t destReg, uint8_t literal)
	{
		return (V2MP_OP_ADD << 12)
			| ((static_cast<V2MP_Word>(destReg) & 0x3) << 10)
			| ((static_cast<V2MP_Word>(destReg) & 0x3) << 8)
			| static_cast<V2MP_Word>(literal);
	}

	constexpr inline V2MP_Word SUBR(uint8_t sourceReg, uint8_t destReg)
	{
		return (V2MP_OP_SUB << 12)
			| ((static_cast<V2MP_Word>(sourceReg) & 0x3) << 10)
			| ((static_cast<V2MP_Word>(destReg) & 0x3) << 8);
	}

	constexpr inline V2MP_Word SUBL(uint8_t destReg, uint8_t literal)
	{
		return (V2MP_OP_SUB << 12)
			| ((static_cast<V2MP_Word>(destReg) & 0x3) << 10)
			| ((static_cast<V2MP_Word>(destReg) & 0x3) << 8)
			| static_cast<V2MP_Word>(literal);
	}

	constexpr inline V2MP_Word SHFTR(uint8_t sourceReg, uint8_t destReg)
	{
		return (V2MP_OP_SHFT << 12)
			| ((static_cast<V2MP_Word>(sourceReg) & 0x3) << 10)
			| ((static_cast<V2MP_Word>(destReg) & 0x3) << 8);
	}

	constexpr inline V2MP_Word SHFTL(uint8_t destReg, int8_t shift)
	{
		return (V2MP_OP_SHFT << 12)
			| ((static_cast<V2MP_Word>(destReg) & 0x3) << 10)
			| ((static_cast<V2MP_Word>(destReg) & 0x3) << 8)
			| static_cast<V2MP_Word>(static_cast<uint8_t>(shift) & 0x1F);
	}

	constexpr inline V2MP_Word BITWR(uint8_t sourceReg, uint8_t destReg, BitwiseOp operation)
	{
		return (V2MP_OP_BITW << 12)
			| ((static_cast<V2MP_Word>(sourceReg) & 0x3) << 10)
			| ((static_cast<V2MP_Word>(destReg) & 0x3) << 8)
			| ((static_cast<V2MP_Word>(operation) & 0x3) << 6);
	}

	constexpr inline V2MP_Word BITWL(uint8_t destReg, BitwiseOp operation, uint8_t shift, bool negate)
	{
		return (V2MP_OP_BITW << 12)
			| ((static_cast<V2MP_Word>(destReg) & 0x3) << 10)
			| ((static_cast<V2MP_Word>(destReg) & 0x3) << 8)
			| ((static_cast<V2MP_Word>(operation) & 0x3) << 6)
			| (static_cast<V2MP_Word>(negate ? 0x1 : 0x0) << 5)
			| (static_cast<V2MP_Word>(shift) & 0xF);
	}

	constexpr inline V2MP_Word BXZR()
	{
		return (V2MP_OP_CBX << 12)
			| static_cast<V2MP_Word>(1 << 11);
	}

	constexpr inline V2MP_Word BXCR()
	{
		return (V2MP_OP_CBX << 12)
			| static_cast<V2MP_Word>(1 << 11)
			| static_cast<V2MP_Word>(1 << 10);
	}

	constexpr inline V2MP_Word BXZL(int8_t deltaWords)
	{
		return (V2MP_OP_CBX << 12)
			| (static_cast<V2MP_Word>(deltaWords) & 0x00FF);
	}

	constexpr inline V2MP_Word BXCL(int8_t deltaWords)
	{
		return (V2MP_OP_CBX << 12)
			| static_cast<V2MP_Word>(1 << 10)
			| (static_cast<V2MP_Word>(deltaWords) & 0x00FF);
	}
}
