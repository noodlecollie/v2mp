#include <string>
#include <sstream>
#include "catch2/catch.hpp"
#include "V2MPAsm/Assembler.h"
#include "V2MPAsm/Exception.h"
#include "ExceptionIDs.h"
#include "ProgramVerification.h"

SCENARIO("ADD: Too many arguments")
{
	GIVEN("A program containing an ADD with too many arguments")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"ADD: Too many arguments",

			"add 0 1 0 3\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK_FALSE(V2MPAsm_Assembler_Run(assembler));

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

SCENARIO("ADD: Too few arguments")
{
	GIVEN("A program containing an ADD with too few arguments")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"ADD: Too few arguments",

			"add 0 0\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK_FALSE(V2MPAsm_Assembler_Run(assembler));

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

SCENARIO("ADD: Non-numeric arguments")
{
	GIVEN("A program containing an ADD with non-numeric arguments")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"ADD: Non-numeric arguments",

			"add zero one two\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK_FALSE(V2MPAsm_Assembler_Run(assembler));

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

SCENARIO("ADD: Register arguments out of range")
{
	GIVEN("A program containing an ADD with the first argument out of range")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"ADD: Argument out of range",

			"add -1 1 0\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK_FALSE(V2MPAsm_Assembler_Run(assembler));

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

	AND_GIVEN("A program containing an ADD with the second argument out of range")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"ADD: Argument out of range",

			"add 0 12345 0\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK_FALSE(V2MPAsm_Assembler_Run(assembler));

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

SCENARIO("ADD: Literal value out of range")
{
	GIVEN("A program containing an ADD with a literal value that is too small")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"ADD: Literal value out of range",

			"add 0 0 -10\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK_FALSE(V2MPAsm_Assembler_Run(assembler));

			THEN("An error should be raised that the argument was out of range")
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

	AND_GIVEN("A program containing an ADD with a literal value that is too large")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"ADD: Literal value out of range",

			"add 0 0 256\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK_FALSE(V2MPAsm_Assembler_Run(assembler));

			THEN("An error should be raised that the argument was out of range")
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

SCENARIO("ADD: Literal value not zero")
{
	GIVEN("A program containing an ADD where the literal value should be zero but is not")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"ADD: Literal value out of range",

			"add 0 1 5\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK_FALSE(V2MPAsm_Assembler_Run(assembler));

			THEN("An error should be raised that the argument was out of range")
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

SCENARIO("ADD: Label refs as arguments")
{
	GIVEN("A program containing an ADD with label refs used as register IDs")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"ADd: Label refs as register IDs",

			":label\n"
			"add <:label >:label 0\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK_FALSE(V2MPAsm_Assembler_Run(assembler));

			THEN("An error should be raised for every label ref usage")
			{
				CHECK(V2MPAsm_Assembler_GetExceptionCount(assembler) == 2);

				for ( size_t index = 0; index < 4; ++index )
				{
					const V2MPAsm_Exception* exception = V2MPAsm_Assembler_GetException(assembler, 0);
					REQUIRE(exception);

					CHECK(V2MPAsm_Exception_GetType(exception) == V2MPASM_EXCEPTION_ERROR);
					CHECK(std::string(V2MPAsm_Exception_GetID(exception)) == std::string(EXCEPTION_ID_INVALID_ARG_TYPE));
				}
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}

	AND_GIVEN("A program containing an ADD with a label ref as a literal value")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"ADd: Label ref as literal value",

			":label\n"
			"add 0 0 0\n"
			"add 0 0 ~:label\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler));

			THEN("A valid program should be produced")
			{
				CheckProgramMatches(
					assembler,
					{
						Asm::ADDL(Asm::REG_R0, 0),
						Asm::ADDL(Asm::REG_R0, 1),
					}
				);
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("ADD: Valid permutations")
{
	GIVEN("A program containing all valid ADD permutations")
	{
		std::stringstream stream;

		// R0, R1, LR, PC
		for ( int arg0 = 0; arg0 < 4; ++arg0 )
		{
			// R0, R1, LR, PC
			for ( int arg1 = 0; arg1 < 4; ++arg1 )
			{
				stream << "add " << arg0 << " " << arg1 << " 0\n";
			}
		}

		const std::string program = stream.str();

		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"ADD: Valid permutations",

			program.c_str()
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler));

			THEN("A valid program should be produced")
			{
				CheckProgramMatches(
					assembler,
					{
						Asm::ADDR(Asm::REG_R0, Asm::REG_R0),
						Asm::ADDR(Asm::REG_R0, Asm::REG_R1),
						Asm::ADDR(Asm::REG_R0, Asm::REG_LR),
						Asm::ADDR(Asm::REG_R0, Asm::REG_PC),

						Asm::ADDR(Asm::REG_R1, Asm::REG_R0),
						Asm::ADDR(Asm::REG_R1, Asm::REG_R1),
						Asm::ADDR(Asm::REG_R1, Asm::REG_LR),
						Asm::ADDR(Asm::REG_R1, Asm::REG_PC),

						Asm::ADDR(Asm::REG_LR, Asm::REG_R0),
						Asm::ADDR(Asm::REG_LR, Asm::REG_R1),
						Asm::ADDR(Asm::REG_LR, Asm::REG_LR),
						Asm::ADDR(Asm::REG_LR, Asm::REG_PC),

						Asm::ADDR(Asm::REG_PC, Asm::REG_R0),
						Asm::ADDR(Asm::REG_PC, Asm::REG_R1),
						Asm::ADDR(Asm::REG_PC, Asm::REG_LR),
						Asm::ADDR(Asm::REG_PC, Asm::REG_PC),
					}
				);
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}

	AND_GIVEN("A program containing an ADD with a literal value")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"ADD: Literal value",

			"add 0 0 123\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler));

			THEN("A valid program should be produced")
			{
				CheckProgramMatches(
					assembler,
					{
						Asm::ADDL(Asm::REG_R0, 123)
					}
				);
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}
