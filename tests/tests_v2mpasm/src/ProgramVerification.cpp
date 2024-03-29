#include <string>
#include <sstream>
#include <iomanip>
#include "ProgramVerification.h"
#include "catch2/catch.hpp"

static std::string WordAsHex(Asm::Word in)
{
	std::stringstream stream;
	stream << "0x" << std::setw(4) << std::setfill('0') << std::hex << static_cast<uint32_t>(in);
	return stream.str();
}

void CheckProgramMatches(V2MPAsm_Assembler* assembler, const std::vector<Asm::Word>& expected)
{
	REQUIRE(assembler);
	REQUIRE(V2MPAsm_Assembler_GetExceptionCount(assembler) == 0);

	CHECK(V2MPAsm_Assembler_InMemoryOutputBufferSizeInBytes(assembler) == expected.size() * sizeof(Asm::Word));

	std::vector<Asm::Word> actual(expected.size(), 0);
	const size_t outputSize = V2MPAsm_Assembler_TakeInMemoryOutputBuffer(assembler, actual.data(), actual.size() * sizeof(Asm::Word));

	CHECK(outputSize == actual.size() * sizeof(Asm::Word));

	// Just in case the above check fails - we still want to check as many words as we can.
	const size_t count = std::min<size_t>(expected.size(), actual.size());

	for ( size_t index = 0; index < count; ++index )
	{
		DYNAMIC_SECTION( "Verifying code word at address: " << (index * sizeof(Asm::Word)) )
		{
			// Convert the values to hex strings, to make it easier to interpret when a test fails.
			CHECK(WordAsHex(expected[index]) == WordAsHex(actual[index]));
		}
	}
}
