#include <string>
#include <sstream>
#include "catch2/catch.hpp"
#include "V2MPAsm/Assembler.h"
#include "V2MPAsm/Exception.h"
#include "ExceptionIDs.h"
#include "ProgramVerification.h"

static uint8_t RegisterFlags(bool r0, bool r1, bool lr, bool pc)
{
	uint8_t out = 0;

	if ( r0 )
	{
		out |= 1 << Asm::REG_R0;
	}

	if ( r1 )
	{
		out |= 1 << Asm::REG_R1;
	}

	if ( lr )
	{
		out |= 1 << Asm::REG_LR;
	}

	if ( pc )
	{
		out |= 1 << Asm::REG_PC;
	}

	return out;
}

SCENARIO("STK: Too many arguments", "[stk]")
{
	GIVEN("A program containing a STK with too many arguments")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"STK: Too many arguments",

			"stk 0 1 2\n"
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

SCENARIO("STK: Too few arguments", "[stk]")
{
	GIVEN("A program containing a STK with too few arguments")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"STK: Too few arguments",

			"stk 0\n"
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

SCENARIO("STK: Non-numeric arguments", "[stk]")
{
	GIVEN("A program containing a STK with non-numeric arguments")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"STK: Non-numeric arguments",

			"stk zero one\n"
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

SCENARIO("STK: Arguments out of range", "[stk]")
{
	GIVEN("A program containing a STK with the first argument out of range")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"STK: Argument out of range",

			"stk -1 1\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler) == V2MPASM_COMPLETED_WITH_WARNINGS);

			THEN("A warning should be raised that the argument is out of range")
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

	AND_GIVEN("A program containing a STK with the second argument out of range")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"STK: Argument out of range",

			"stk 0 17\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler) == V2MPASM_COMPLETED_WITH_WARNINGS);

			THEN("A warning should be raised that the argument is out of range")
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

SCENARIO("STK: Argument truncation results in invalid register flags", "[stk]")
{
	GIVEN("A program containing a STK where register flag value is truncated to zero")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"STK: Truncate to invalid flags",

			"stk 0 16\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler) == V2MPASM_COMPLETED_WITH_WARNINGS);

			THEN("Two warnings should be raised")
			{
				CHECK(V2MPAsm_Assembler_GetExceptionCount(assembler) == 2);

				const V2MPAsm_Exception* exception = V2MPAsm_Assembler_GetException(assembler, 0);
				REQUIRE(exception);

				CHECK(V2MPAsm_Exception_GetType(exception) == V2MPASM_EXCEPTION_WARNING);
				CHECK(std::string(V2MPAsm_Exception_GetID(exception)) == std::string(EXCEPTION_ID_ARG_OUT_OF_RANGE));

				exception = V2MPAsm_Assembler_GetException(assembler, 1);
				REQUIRE(exception);

				CHECK(V2MPAsm_Exception_GetType(exception) == V2MPASM_EXCEPTION_WARNING);
				CHECK(std::string(V2MPAsm_Exception_GetID(exception)) == std::string(EXCEPTION_ID_INVALID_ARG_VALUE));
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("STK: Valid permutations", "[stk]")
{
	GIVEN("A program containing all valid STK permutations")
	{
		std::stringstream stream;

		for ( int arg0 = 1; arg0 < 16; ++arg0 )
		{
			stream << "stk 0 " << arg0 << "\n";
			stream << "stk 1 " << arg0 << "\n";
		}

		const std::string program = stream.str();

		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"STK: Valid permutations",

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
						Asm::POP(RegisterFlags(true, false, false, false)),
						Asm::PUSH(RegisterFlags(true, false, false, false)),

						Asm::POP(RegisterFlags(false, true, false, false)),
						Asm::PUSH(RegisterFlags(false, true, false, false)),

						Asm::POP(RegisterFlags(true, true, false, false)),
						Asm::PUSH(RegisterFlags(true, true, false, false)),

						Asm::POP(RegisterFlags(false, false, true, false)),
						Asm::PUSH(RegisterFlags(false, false, true, false)),

						Asm::POP(RegisterFlags(true, false, true, false)),
						Asm::PUSH(RegisterFlags(true, false, true, false)),

						Asm::POP(RegisterFlags(false, true, true, false)),
						Asm::PUSH(RegisterFlags(false, true, true, false)),

						Asm::POP(RegisterFlags(true, true, true, false)),
						Asm::PUSH(RegisterFlags(true, true, true, false)),

						Asm::POP(RegisterFlags(false, false, false, true)),
						Asm::PUSH(RegisterFlags(false, false, false, true)),

						Asm::POP(RegisterFlags(true, false, false, true)),
						Asm::PUSH(RegisterFlags(true, false, false, true)),

						Asm::POP(RegisterFlags(false, true, false, true)),
						Asm::PUSH(RegisterFlags(false, true, false, true)),

						Asm::POP(RegisterFlags(true, true, false, true)),
						Asm::PUSH(RegisterFlags(true, true, false, true)),

						Asm::POP(RegisterFlags(false, false, true, true)),
						Asm::PUSH(RegisterFlags(false, false, true, true)),

						Asm::POP(RegisterFlags(true, false, true, true)),
						Asm::PUSH(RegisterFlags(true, false, true, true)),

						Asm::POP(RegisterFlags(false, true, true, true)),
						Asm::PUSH(RegisterFlags(false, true, true, true)),

						Asm::POP(RegisterFlags(true, true, true, true)),
						Asm::PUSH(RegisterFlags(true, true, true, true)),
					}
				);
			}
		}
	}
}
