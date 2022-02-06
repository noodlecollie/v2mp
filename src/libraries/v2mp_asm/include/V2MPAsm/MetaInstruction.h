#ifndef V2MPASM_METAINSTRUCTION_H
#define V2MPASM_METAINSTRUCTION_H

#include <cstdint>
#include "V2MPAsm/Defs.h"

namespace V2MPAsm
{
	class MetaInstruction
	{
	public:
		// The label string is assumed to be static.
		inline MetaInstruction(const char* label) :
			m_Instruction(reinterpret_cast<uintptr_t>(label))
		{
		}

		// We rely on the fact that a pointer will never (within our intended system domains)
		// be aligned to an odd memory address. Setting the lower 16 bits to ones allows us
		// to easily check for the presence of an instruction word in the higher bits later.
		inline MetaInstruction(V2MPAsm_Word instruction) :
			m_Instruction((static_cast<uintptr_t>(instruction) << 16) | INSTRUCTION_FLAG)
		{
		}

		inline bool IsInstruction() const
		{
			return (m_Instruction & INSTRUCTION_MASK) == INSTRUCTION_FLAG;
		}

		inline const char* GetLabel() const
		{
			return (!IsInstruction()) ? reinterpret_cast<const char*>(m_Instruction) : nullptr;
		}

		inline V2MPAsm_Word GetInstructionWord() const
		{
			return IsInstruction() ? static_cast<V2MPAsm_Word>(m_Instruction >> 16) : 0;
		}

	private:
		static constexpr uintptr_t INSTRUCTION_FLAG = 0xFFFF;
		static constexpr uintptr_t INSTRUCTION_MASK = 0xFFFF;

		uintptr_t m_Instruction = 0;
	};
}

#endif // V2MPASM_METAINSTRUCTION_H
