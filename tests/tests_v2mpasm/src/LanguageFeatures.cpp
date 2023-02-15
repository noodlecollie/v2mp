#include "catch2/catch.hpp"
#include "LibV2MPAsm/Assembler.h"
#include "LibV2MPAsm/Exception.h"
#include "ProgramVerification.h"
#include "ExceptionIDs.h"

SCENARIO("Assembly language feature sanity check", "[lang]")
{
	GIVEN("An assembler instance and a valid program")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"Language Features",

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
			"add 0 0 ~+:label\n"
			"add 0 0 <:label\n"
			"add 0 0 >:label\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler) == V2MPASM_COMPLETED_OK);

			THEN("A valid program is produced")
			{
				CheckProgramMatches(
					assembler,
					{
						Asm::ADDL(Asm::REG_R0, 0),
						Asm::ADDL(Asm::REG_R0, 1),
						Asm::ADDL(Asm::REG_R0, 0),
						Asm::ADDL(Asm::REG_R0, 2),
					}
				);
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("No newline at end of file")
{
	GIVEN("An assembler instance and a program with no trailing newline")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"No trailing newline",

			"nop"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler) == V2MPASM_COMPLETED_OK);

			THEN("A valid program is produced")
			{
				CheckProgramMatches(
					assembler,
					{
						Asm::NOP()
					}
				);
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}
