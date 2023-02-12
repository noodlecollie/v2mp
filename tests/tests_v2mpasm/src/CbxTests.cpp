#include <string>
#include <sstream>
#include "catch2/catch.hpp"
#include "V2MPAsm/Assembler.h"
#include "V2MPAsm/Exception.h"
#include "ExceptionIDs.h"
#include "ProgramVerification.h"

SCENARIO("CBX: Too many arguments", "[cbx]")
{
	GIVEN("A program containing a CBX with too many arguments")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"CBX: Too many arguments",

			"cbx 0 1 2 3\n"
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

SCENARIO("CBX: Too few arguments", "[cbx]")
{
	GIVEN("A program containing a CBX with too few arguments")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"CBX: Too few arguments",

			"cbx 0 0\n"
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

SCENARIO("CBX: Non-numeric arguments", "[cbx]")
{
	GIVEN("A program containing a CBX with non-numeric arguments")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"CBX: Non-numeric arguments",

			"cbx zero one two\n"
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

SCENARIO("CBX: Arguments out of range", "[cbx]")
{
	GIVEN("A program containing a CBX with the first argument out of range")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"CBX: Argument out of range",

			"cbx -1 0 0\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler) == V2MPASM_COMPLETED_WITH_WARNINGS);

			THEN("An error should be raised that the argument is out of range")
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

	AND_GIVEN("A program containing a CBX with the second argument out of range")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"CBX: Argument out of range",

			"cbx 0 2 0\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler) == V2MPASM_COMPLETED_WITH_WARNINGS);

			THEN("An error should be raised that the argument is out of range")
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

SCENARIO("CBX: Literal argument out of range", "[cbx]")
{
	GIVEN("A program containing a CBX where the literal argument is too small")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"CBX: Literal argument too small",

			"cbx 0 0 -129\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler) == V2MPASM_COMPLETED_WITH_WARNINGS);

			THEN("An error should be raised that the argument is out of range")
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

	AND_GIVEN("A program containing a CBX where the literal argument is too small")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"CBX: Literal argument too small",

			"cbx 0 0 1234\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler) == V2MPASM_COMPLETED_WITH_WARNINGS);

			THEN("An error should be raised that the argument is out of range")
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

SCENARIO("CBX: PC delta value not zero", "[cbx]")
{
	GIVEN("A program containing a CBX where the PC delta value should be zero but is not")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"CBX: PC delta value not zero",

			"cbx 1 0 2\n"
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

SCENARIO("CBX: Label refs as arguments", "[cbx]")
{
	GIVEN("A program containing a CBX with a label ref as the first argument")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"CBX: Label refs as args",

			":label\n"
			"cbx <:label 0 0\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler) == V2MPASM_FAILED);

			THEN("An error should be raised that the argument is of an incorrect type")
			{
				CHECK(V2MPAsm_Assembler_GetExceptionCount(assembler) == 1);

				const V2MPAsm_Exception* exception = V2MPAsm_Assembler_GetException(assembler, 0);
				REQUIRE(exception);

				CHECK(V2MPAsm_Exception_GetType(exception) == V2MPASM_EXCEPTION_ERROR);
				CHECK(std::string(V2MPAsm_Exception_GetID(exception)) == std::string(EXCEPTION_ID_INVALID_ARG_TYPE));
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}

	AND_GIVEN("A program containing a CBX with a label ref as the second argument")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"CBX: Label refs as args",

			":label\n"
			"cbx 0 >:label 0\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler) == V2MPASM_FAILED);

			THEN("An error should be raised that the argument is of an incorrect type")
			{
				CHECK(V2MPAsm_Assembler_GetExceptionCount(assembler) == 1);

				const V2MPAsm_Exception* exception = V2MPAsm_Assembler_GetException(assembler, 0);
				REQUIRE(exception);

				CHECK(V2MPAsm_Exception_GetType(exception) == V2MPASM_EXCEPTION_ERROR);
				CHECK(std::string(V2MPAsm_Exception_GetID(exception)) == std::string(EXCEPTION_ID_INVALID_ARG_TYPE));
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}

	AND_GIVEN("A program containing a CBX with a label ref as the PC delta")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"CBX: Label refs as args",

			":loop\n"
			"add 0 0 0\n"
			"cbx 0 0 ~:loop\n"
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
						Asm::ADDL(Asm::REG_R0, 0),
						Asm::BXZL(-2),
					}
				);
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("CBX: Valid permutations", "[cbx]")
{
	GIVEN("A program containing all valid CBX permutations which don't involve literal values")
	{
		std::stringstream stream;

		// Literal, LR
		for ( int arg0 = 0; arg0 < 2; ++arg0 )
		{
			// SR[Z], SR[C]
			for ( int arg1 = 0; arg1 < 2; ++arg1 )
			{
				stream << "cbx " << arg0 << " " << arg1 << " 0\n";
			}
		}

		const std::string program = stream.str();

		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"CBX: Valid permutations",

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
						Asm::BXZL(0),
						Asm::BXCL(0),
						Asm::BXZR(),
						Asm::BXCR(),
					}
				);
			}
		}
	}
}
