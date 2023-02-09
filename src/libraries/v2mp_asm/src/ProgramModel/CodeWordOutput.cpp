#include <stdexcept>
#include "ProgramModel/CodeWordOutput.h"
#include "ProgramModel/CodeWord.h"
#include "ProgramModel/CodeWordArg.h"
#include "ProgramModel/InstructionMeta.h"
#include "Utils/BitUtils.h"

namespace V2MPAsm
{
	static void ProcessArg(const CodeWordArg& arg, const ArgMeta& argMeta, uint16_t& outValue)
	{
		const size_t numBits = argMeta.highBit - argMeta.lowBit + 1;
		const uint16_t argValue = (static_cast<uint16_t>(arg.GetValue()) & BitMask(numBits)) << argMeta.lowBit;

		outValue |= (argValue & 0x0FFF);
	}

	uint16_t ProduceCodeWord(const CodeWord& codeWord)
	{
		uint16_t outValue = static_cast<uint16_t>(codeWord.GetInstructionType()) << 12;
		const size_t argCount = codeWord.GetArgumentCount();

		for ( size_t index = 0; index < argCount; ++index )
		{
			const CodeWordArg* arg = codeWord.GetArgument(index);

			if ( !arg )
			{
				throw std::runtime_error("Encountered unexpected null code word argument.");
			}

			const InstructionMeta& instructionMeta = GetInstructionMeta(codeWord.GetInstructionType());

			if ( index >= instructionMeta.args.size() )
			{
				throw std::runtime_error("Argument index was out of range for instruction meta.");
			}

			ProcessArg(*arg, instructionMeta.args[index], outValue);
		}

		return outValue;
	}

	void EmitCodeWordToStream(const CodeWord& codeWord, std::ostream& outStream)
	{
		const uint16_t outValue = ProduceCodeWord(codeWord);
		outStream.write(reinterpret_cast<const char*>(&outValue), sizeof(outValue));
	}
}
