#include "catch2/catch.hpp"
#include "V2MPAsm/Assembler.h"

SCENARIO("Dummy")
{
	GIVEN("An assembler instance and a valid program")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"DummyTest",

			"// This is a line comment\n"
			"/* This is a multiline comment */\n"
			"/*\n"
			"\n"
			"* This is a multiline comment that\n"
			"* spans multiple lines.\n"
			"*/\n"
			"\n"
			"add 0 0 0\n"
			"\n"
			":label\n"
			"add 0 0 ~:label\n"
			"add 0 0 <:label\n"
			"add 0 0 >:label\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler));

			THEN("There are no errors")
			{
				CHECK(V2MPAsm_Assembler_GetExceptionCount(assembler) == 0);
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}
