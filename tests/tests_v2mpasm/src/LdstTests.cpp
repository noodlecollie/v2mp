#include <string>
#include <sstream>
#include "catch2/catch.hpp"
#include "V2MPAsm/Assembler.h"
#include "V2MPAsm/Exception.h"
#include "ExceptionIDs.h"
#include "ProgramVerification.h"

SCENARIO("LDST: Too many arguments", "[ldst]")
{
	GIVEN("A program containing an LDST with too many arguments")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"LDST: Too many arguments",

			"ldst 0 1 2\n"
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

SCENARIO("LDST: Too few arguments", "[ldst]")
{
	GIVEN("A program containing an LDST with too few arguments")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"LDST: Too few arguments",

			"ldst 0\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler) == V2MPASM_FAILED);

			THEN("An error should be raised that there were too few arguments")
			{
				CHECK(V2MPAsm_Assembler_GetExceptionCount(assembler) == 1);

				const V2MPAsm_Exception* exception = V2MPAsm_Assembler_GetException(assembler, 0);
				REQUIRE(exception);

				CHECK(V2MPAsm_Exception_GetType(exception) == V2MPASM_EXCEPTION_ERROR);
				CHECK(std::string(V2MPAsm_Exception_GetID(exception)) == std::string(EXCEPTION_ID_TOO_FEW_ARGUMENTS));
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("LDST: Non-numeric arguments", "[ldst]")
{
	GIVEN("A program containing an LDST with non-numeric arguments")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"LDST: Non-numeric arguments",

			"ldst zero one\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler) == V2MPASM_FAILED);

			THEN("An error should be raised that an unexpected token was encountered")
			{
				CHECK(V2MPAsm_Assembler_GetExceptionCount(assembler) == 1);

				const V2MPAsm_Exception* exception = V2MPAsm_Assembler_GetException(assembler, 0);
				REQUIRE(exception);

				CHECK(V2MPAsm_Exception_GetType(exception) == V2MPASM_EXCEPTION_ERROR);
				CHECK(std::string(V2MPAsm_Exception_GetID(exception)) == std::string(EXCEPTION_ID_UNEXPECTED_TOKEN));
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("LDST: Arguments out of range", "[ldst]")
{
	GIVEN("A program containing an LDST with the first argument out of range")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"LDST: Argument out of range",

			"ldst -1 1\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler) == V2MPASM_COMPLETED_WITH_WARNINGS);

			THEN("A warning should be raised that the argument was out of range")
			{
				CHECK(V2MPAsm_Assembler_GetExceptionCount(assembler) == 1);

				const V2MPAsm_Exception* exception = V2MPAsm_Assembler_GetException(assembler, 0);
				REQUIRE(exception);

				CHECK(V2MPAsm_Exception_GetType(exception) == V2MPASM_EXCEPTION_WARNING);
				CHECK(std::string(V2MPAsm_Exception_GetID(exception)) == std::string(EXCEPTION_ID_ARG_OUT_OF_RANGE));
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}

	AND_GIVEN("A program containing an LDST with the second argument out of range")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"LDST: Argument out of range",

			"ldst 0 10\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler) == V2MPASM_FAILED);

			THEN("An error should be raised that the register ID is invalid")
			{
				CHECK(V2MPAsm_Assembler_GetExceptionCount(assembler) == 1);

				const V2MPAsm_Exception* exception = V2MPAsm_Assembler_GetException(assembler, 0);
				REQUIRE(exception);

				CHECK(V2MPAsm_Exception_GetType(exception) == V2MPASM_EXCEPTION_ERROR);
				CHECK(std::string(V2MPAsm_Exception_GetID(exception)) == std::string(EXCEPTION_ID_INVALID_REGISTER_ID));
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}

	AND_GIVEN("A program containing an LDST with both arguments out of range")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"LDST: Argument out of range",

			"ldst 5 -1\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler) == V2MPASM_FAILED);

			THEN("Appropriate errors should be reported")
			{
				CHECK(V2MPAsm_Assembler_GetExceptionCount(assembler) == 2);

				const V2MPAsm_Exception* exception = V2MPAsm_Assembler_GetException(assembler, 0);
				REQUIRE(exception);

				CHECK(V2MPAsm_Exception_GetType(exception) == V2MPASM_EXCEPTION_WARNING);
				CHECK(std::string(V2MPAsm_Exception_GetID(exception)) == std::string(EXCEPTION_ID_ARG_OUT_OF_RANGE));

				exception = V2MPAsm_Assembler_GetException(assembler, 1);
				REQUIRE(exception);

				CHECK(V2MPAsm_Exception_GetType(exception) == V2MPASM_EXCEPTION_ERROR);
				CHECK(std::string(V2MPAsm_Exception_GetID(exception)) == std::string(EXCEPTION_ID_INVALID_REGISTER_ID));
			}
		}
	}
}

SCENARIO("LDST: Label refs as arguments", "[ldst]")
{
	GIVEN("A program containing an LDST with label refs as arguments")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"LDST: Label ref args",

			":label\n"
			"ldst <:label >:label\n"
			"ldst ~:label ~:label\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler) == V2MPASM_FAILED);

			THEN("An error should be raised for every label ref usage")
			{
				CHECK(V2MPAsm_Assembler_GetExceptionCount(assembler) == 4);

				for ( size_t index = 0; index < 4; ++index )
				{
					const V2MPAsm_Exception* exception = V2MPAsm_Assembler_GetException(assembler, index);
					REQUIRE(exception);

					CHECK(V2MPAsm_Exception_GetType(exception) == V2MPASM_EXCEPTION_ERROR);
					CHECK(std::string(V2MPAsm_Exception_GetID(exception)) == std::string(EXCEPTION_ID_INVALID_ARG_TYPE));
				}
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("LDST: Valid permutations", "[ldst]")
{
	GIVEN("A program containing all valid LDST permutations")
	{
		std::stringstream stream;

		// Load, store
		for ( int arg0 = 0; arg0 < 2; ++arg0 )
		{
			// R0, R1, LR, PC
			for ( int arg1 = 0; arg1 < 4; ++arg1 )
			{
				stream << "ldst " << arg0 << " " << arg1 << "\n";
			}
		}

		const std::string program = stream.str();

		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"LDST: Valid permutations",

			program.c_str()
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
						Asm::LOAD(Asm::REG_R0),
						Asm::LOAD(Asm::REG_R1),
						Asm::LOAD(Asm::REG_LR),
						Asm::LOAD(Asm::REG_PC),
						Asm::STOR(Asm::REG_R0),
						Asm::STOR(Asm::REG_R1),
						Asm::STOR(Asm::REG_LR),
						Asm::STOR(Asm::REG_PC),
					}
				);
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}
