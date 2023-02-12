#include "catch2/catch.hpp"
#include "V2MPAsm/Assembler.h"
#include "V2MPAsm/Exception.h"
#include "ExceptionIDs.h"
#include "ProgramVerification.h"

SCENARIO("SIG: Too many arguments")
{
	GIVEN("A program containing a SIG with too many arguments")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"SIG: Too many arguments",

			"sig 1 2 3\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler) == V2MPASM_COMPLETED_WITH_WARNINGS);

			THEN("A warning should be raised that there were too many arguments")
			{
				CHECK(V2MPAsm_Assembler_GetExceptionCount(assembler) == 1);

				const V2MPAsm_Exception* exception = V2MPAsm_Assembler_GetException(assembler, 0);
				REQUIRE(exception);

				CHECK(V2MPAsm_Exception_GetType(exception) == V2MPASM_EXCEPTION_WARNING);
				CHECK(std::string(V2MPAsm_Exception_GetID(exception)) == std::string(EXCEPTION_ID_TOO_MANY_ARGUMENTS));
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("SIG: Ten of them")
{
	GIVEN("A program containing ten SIGs on individual lines")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"SIG: Ten of them",

			"sig\n"
			"sig\n"
			"sig\n"
			"sig\n"
			"sig\n"
			"sig\n"
			"sig\n"
			"sig\n"
			"sig\n"
			"sig\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler) == V2MPASM_COMPLETED_OK);

			THEN("A valid program should be produced")
			{
				CheckProgramMatches(
					assembler,
					{
						Asm::SIG(),
						Asm::SIG(),
						Asm::SIG(),
						Asm::SIG(),
						Asm::SIG(),
						Asm::SIG(),
						Asm::SIG(),
						Asm::SIG(),
						Asm::SIG(),
						Asm::SIG(),
					}
				);
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}
