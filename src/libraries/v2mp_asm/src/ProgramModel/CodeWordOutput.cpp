#include <stdexcept>
#include "ProgramModel/CodeWordOutput.h"
#include "ProgramModel/CodeWord.h"
#include "ProgramModel/CodeWordArg.h"
#include "ProgramModel/InstructionMeta.h"

namespace V2MPAsm
{
	static constexpr size_t NUM_BITS_IN_WORD = sizeof(uint16_t) * 8;
	static constexpr size_t MAX_LEFT_SHIFT = NUM_BITS_IN_WORD - 1;

	static uint16_t TrimRightBits(uint16_t in, size_t trim)
	{
		return (in >> trim) << trim;
	}

	static uint16_t TrimLeftBits(uint16_t in, size_t trim)
	{
		return (in << trim) >> trim;
	}

	static void ProcessArg(const CodeWordArg& arg, const ArgMeta& argMeta, uint16_t& outValue)
	{
		uint16_t argValue = static_cast<uint16_t>(arg.GetValue());

		argValue = TrimRightBits(argValue, argMeta.lowBit);
		argValue = TrimLeftBits(argValue, MAX_LEFT_SHIFT - std::min<size_t>(MAX_LEFT_SHIFT, argMeta.highBit));

		outValue |= argValue;
	}

	void EmitCodeWord(const CodeWord& codeWord, std::ostream& outStream)
	{
		uint16_t outValue = 0;
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

		outStream.write(reinterpret_cast<const char*>(&outValue), sizeof(outValue));
	}
}
