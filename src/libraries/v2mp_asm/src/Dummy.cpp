#include <iostream>
#include "V2MPAsm/MetaInstruction.h"

namespace V2MPAsm
{
	static const MetaInstruction INSTRUCTIONS[2] =
	{
		"This is a label",
		0x1234
	};

	void Dummy(void)
	{
		static_assert(SIZEOF_VOID >= 4, "V2MPAsm is not supported on systems with a word size of less than 32 bits.");

		for ( size_t index = 0; index < 2; ++index )
		{
			const MetaInstruction& instr = INSTRUCTIONS[index];
			const char* label = "<none>";

			if ( !instr.IsInstruction() )
			{
				label = instr.GetLabel();
			}

			std::cout << "Index " << index << ":" << std::endl;
			std::cout << "  Is instruction: " << (instr.IsInstruction() ? "true" : "false") << std::endl;
			std::cout << "  Label: " << label << std::endl;
			std::cout << "  Instruction word: 0x" << std::hex << instr.GetInstructionWord() << std::dec << std::endl;
		}
	}
}
