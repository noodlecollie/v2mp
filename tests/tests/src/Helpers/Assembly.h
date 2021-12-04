#pragma once

#include "V2MP/Defs.h"

namespace Asm
{
	constexpr uint8_t REG_R0 = 0;
	constexpr uint8_t REG_R1 = 1;
	constexpr uint8_t REG_LR = 2;
	constexpr uint8_t REG_PC = 3;

	constexpr V2MP_Word SR_Z = 1 << 0;
	constexpr V2MP_Word SR_C = 1 << 1;

	constexpr inline V2MP_Word FaultFromWord(V2MP_Word faultWord)
	{
		return (faultWord & 0xF000) >> 12;
	}

	constexpr inline V2MP_Word HCF()
	{
		return V2MP_OP_HCF << 12;
	}

	constexpr inline V2MP_Word LOAD(uint8_t destReg)
	{
		return (V2MP_OP_LDST << 12) | ((static_cast<V2MP_Word>(destReg & 0x3)) << 9);
	}

	constexpr inline V2MP_Word STOR(uint8_t sourceReg)
	{
		return (V2MP_OP_LDST << 12) | (1 << 11) | ((static_cast<V2MP_Word>(sourceReg) & 0x3) << 9);
	}
}
