#include <string>
#include <sstream>
#include "catch2/catch.hpp"
#include "V2MPAsm/Assembler.h"
#include "V2MPAsm/Exception.h"
#include "ExceptionIDs.h"
#include "ProgramVerification.h"

SCENARIO("SUB: Too many arguments", "[sub]")
{
	GIVEN("A program containing a SUB with too many arguments")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"SUB: Too many arguments",

			"sub 0 1 0 3\n"
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

SCENARIO("SUB: Too few arguments", "[sub]")
{
	GIVEN("A program containing a SUB with too few arguments")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"SUB: Too few arguments",

			"sub 0 0\n"
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

SCENARIO("SUB: Non-numeric arguments", "[sub]")
{
	GIVEN("A program containing a SUB with non-numeric arguments")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"SUB: Non-numeric arguments",

			"sub zero one two\n"
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

SCENARIO("SUB: Register arguments out of range", "[sub]")
{
	GIVEN("A program containing a SUB with the first argument out of range")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"SUB: Argument out of range",

			"sub -1 1 0\n"
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

	AND_GIVEN("A program containing a SUB with the second argument out of range")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"SUB: Argument out of range",

			"sub 0 12345 0\n"
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

SCENARIO("SUB: Literal value out of range", "[sub]")
{
	GIVEN("A program containing a SUB with a literal value that is too small")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"SUB: Literal value out of range",

			"sub 0 0 -10\n"
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

	AND_GIVEN("A program containing a SUB with a literal value that is too large")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"SUB: Literal value out of range",

			"sub 0 0 256\n"
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

SCENARIO("SUB: Literal value not zero", "[sub]")
{
	GIVEN("A program containing a SUB where the literal value should be zero but is not")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"SUB: Literal value out of range",

			"sub 0 1 5\n"
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
				CHECK(std::string(V2MPAsm_Exception_GetID(exception)) == std::string(EXCEPTION_ID_RESERVED_BITS_SET));
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("SUB: Label refs as arguments", "[sub]")
{
	GIVEN("A program containing a SUB with label refs used as register IDs")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"SUB: Label refs as register IDs",

			":label\n"
			"sub <:label >:label 0\n"
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

	AND_GIVEN("A program containing a SUB with a label ref as a literal value")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"SUB: Label ref as literal value",

			":label\n"
			"sub 0 0 0\n"
			"sub 0 0 ~+:label\n"
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
						Asm::SUBL(Asm::REG_R0, 0),
						Asm::SUBL(Asm::REG_R0, 2),
					}
				);
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("SUB: Valid permutations", "[sub]")
{
	GIVEN("A program containing all valid SUB permutations")
	{
		std::stringstream stream;

		// R0, R1, LR, PC
		for ( int arg0 = 0; arg0 < 4; ++arg0 )
		{
			// R0, R1, LR, PC
			for ( int arg1 = 0; arg1 < 4; ++arg1 )
			{
				stream << "sub " << arg0 << " " << arg1 << " 0\n";
			}
		}

		const std::string program = stream.str();

		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"SUB: Valid permutations",

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
						Asm::SUBL(Asm::REG_R0, 0),
						Asm::SUBR(Asm::REG_R0, Asm::REG_R1),
						Asm::SUBR(Asm::REG_R0, Asm::REG_LR),
						Asm::SUBR(Asm::REG_R0, Asm::REG_PC),

						Asm::SUBR(Asm::REG_R1, Asm::REG_R0),
						Asm::SUBL(Asm::REG_R1, 0),
						Asm::SUBR(Asm::REG_R1, Asm::REG_LR),
						Asm::SUBR(Asm::REG_R1, Asm::REG_PC),

						Asm::SUBR(Asm::REG_LR, Asm::REG_R0),
						Asm::SUBR(Asm::REG_LR, Asm::REG_R1),
						Asm::SUBL(Asm::REG_LR, 0),
						Asm::SUBR(Asm::REG_LR, Asm::REG_PC),

						Asm::SUBR(Asm::REG_PC, Asm::REG_R0),
						Asm::SUBR(Asm::REG_PC, Asm::REG_R1),
						Asm::SUBR(Asm::REG_PC, Asm::REG_LR),
						Asm::SUBL(Asm::REG_PC, 0),
					}
				);
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}

	AND_GIVEN("A program containing a SUB with a literal value")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"SUB: Literal value",

			"sub 0 0 123\n"
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
						Asm::SUBL(Asm::REG_R0, 123)
					}
				);
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}
