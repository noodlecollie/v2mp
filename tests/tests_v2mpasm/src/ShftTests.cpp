#include <string>
#include <sstream>
#include "catch2/catch.hpp"
#include "LibV2MPAsm/Assembler.h"
#include "LibV2MPAsm/Exception.h"
#include "ExceptionIDs.h"
#include "ProgramVerification.h"

SCENARIO("SHFT: Too many arguments", "[shft]")
{
	GIVEN("A program containing a SHFT with too many arguments")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"SHFT: Too many arguments",

			"shft 0 1 0 3\n"
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

SCENARIO("SHFT: Too few arguments", "[shft]")
{
	GIVEN("A program containing a SHFT with too few arguments")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"SHFT: Too few arguments",

			"shft 0 0\n"
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

SCENARIO("SHFT: Non-numeric arguments", "[shft]")
{
	GIVEN("A program containing a SHFT with non-numeric arguments")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"SHFT: Non-numeric arguments",

			"shft zero one two\n"
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

SCENARIO("SHFT: Register arguments out of range", "[shft]")
{
	GIVEN("A program containing a SHFT with the first argument out of range")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"SHFT: Argument out of range",

			"shft -1 1 0\n"
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

	AND_GIVEN("A program containing a SHFT with the second argument out of range")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"SHFT: Argument out of range",

			"shft 0 12345 0\n"
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
}

SCENARIO("SHFT: Literal value out of range", "[shft]")
{
	GIVEN("A program containing a SHFT with a literal value that is too small")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"SHFT: Literal value out of range",

			"shft 0 0 -17\n"
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

	AND_GIVEN("A program containing a SHFT with a literal value that is too large")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"SHFT: Literal value out of range",

			"shft 0 0 16\n"
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
}

SCENARIO("SHFT: Literal value not zero", "[shft]")
{
	GIVEN("A program containing a SHFT where the literal value should be zero but is not")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"SHFT: Literal value not zero",

			"shft 0 1 5\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler) == V2MPASM_COMPLETED_WITH_WARNINGS);

			THEN("A warning should be raised that reserved bits were set")
			{
				CHECK(V2MPAsm_Assembler_GetExceptionCount(assembler) == 1);

				const V2MPAsm_Exception* exception = V2MPAsm_Assembler_GetException(assembler, 0);
				REQUIRE(exception);

				CHECK(V2MPAsm_Exception_GetType(exception) == V2MPASM_EXCEPTION_WARNING);
				CHECK(std::string(V2MPAsm_Exception_GetID(exception)) == std::string(EXCEPTION_ID_RESERVED_BITS_SET));
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("SHFT: Label refs as arguments", "[shft]")
{
	GIVEN("A program containing a SHFT with label refs used as register IDs")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"SHFT: Label refs as register IDs",

			":label\n"
			"shft <:label >:label 0\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler) == V2MPASM_FAILED);

			THEN("An error should be raised for every label ref usage")
			{
				CHECK(V2MPAsm_Assembler_GetExceptionCount(assembler) == 2);

				for ( size_t index = 0; index < 2; ++index )
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

	AND_GIVEN("A program containing a SHFT with a label ref as a literal value")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"SHFT: Label ref as literal value",

			":label\n"
			"shft 0 0 0\n"
			"shft 0 0 ~+:label\n"
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
						Asm::SHFTL(Asm::REG_R0, 0),
						Asm::SHFTL(Asm::REG_R0, 2),
					}
				);
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("SHFT: Valid permutations", "[shft]")
{
	GIVEN("A program containing all valid SHFT permutations")
	{
		std::stringstream stream;

		// R0, R1, LR, PC
		for ( int arg0 = 0; arg0 < 4; ++arg0 )
		{
			// R0, R1, LR, PC
			for ( int arg1 = 0; arg1 < 4; ++arg1 )
			{
				stream << "shft " << arg0 << " " << arg1 << " 0\n";
			}
		}

		const std::string program = stream.str();

		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"SHFT: Valid permutations",

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
						Asm::SHFTL(Asm::REG_R0, 0),
						Asm::SHFTR(Asm::REG_R0, Asm::REG_R1),
						Asm::SHFTR(Asm::REG_R0, Asm::REG_LR),
						Asm::SHFTR(Asm::REG_R0, Asm::REG_PC),

						Asm::SHFTR(Asm::REG_R1, Asm::REG_R0),
						Asm::SHFTL(Asm::REG_R1, 0),
						Asm::SHFTR(Asm::REG_R1, Asm::REG_LR),
						Asm::SHFTR(Asm::REG_R1, Asm::REG_PC),

						Asm::SHFTR(Asm::REG_LR, Asm::REG_R0),
						Asm::SHFTR(Asm::REG_LR, Asm::REG_R1),
						Asm::SHFTL(Asm::REG_LR, 0),
						Asm::SHFTR(Asm::REG_LR, Asm::REG_PC),

						Asm::SHFTR(Asm::REG_PC, Asm::REG_R0),
						Asm::SHFTR(Asm::REG_PC, Asm::REG_R1),
						Asm::SHFTR(Asm::REG_PC, Asm::REG_LR),
						Asm::SHFTL(Asm::REG_PC, 0),
					}
				);
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}

	AND_GIVEN("A program containing a SHFT with a literal value")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"SHFT: Literal value",

			"shft 0 0 5\n"
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
						Asm::SHFTL(Asm::REG_R0, 5)
					}
				);
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}
