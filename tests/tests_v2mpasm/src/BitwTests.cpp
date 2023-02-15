#include <string>
#include <sstream>
#include "catch2/catch.hpp"
#include "LibV2MPAsm/Assembler.h"
#include "LibV2MPAsm/Exception.h"
#include "ExceptionIDs.h"
#include "ProgramVerification.h"

SCENARIO("BITW: Too many arguments", "[bitw]")
{
	GIVEN("A program containing a BITW with too many arguments")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"BITW: Too many arguments",

			"bitw 0 1 0 0 0 0\n"
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

SCENARIO("BITW: Too few arguments", "[bitw]")
{
	GIVEN("A program containing a BITW with too few arguments")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"BITW: Too few arguments",

			"bitw 0 0\n"
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

SCENARIO("BITW: Non-numeric arguments", "[bitw]")
{
	GIVEN("A program containing a BITW with non-numeric arguments")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"BITW: Non-numeric arguments",

			"bitw zero one two 0 0\n"
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

SCENARIO("BITW: Register arguments out of range", "[bitw]")
{
	GIVEN("A program containing a BITW with the first argument out of range")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"BITW: Argument out of range",

			"bitw -1 1 0 0 0\n"
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

	AND_GIVEN("A program containing a BITW with the second argument out of range")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"BITW: Argument out of range",

			"bitw 0 12345 0 0 0\n"
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

SCENARIO("BITW: Non-register arguments out of range", "[bitw]")
{
	GIVEN("A program containing a BITW with the mode argument out of range")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"BITW: Argument out of range",

			"bitw 0 1 10 0 0\n"
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

	AND_GIVEN("A program containing a BITW with the inversion argument out of range")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"BITW: Argument out of range",

			"bitw 0 0 0 -1 0\n"
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

	AND_GIVEN("A program containing a BITW with the shift argument out of range")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"BITW: Argument out of range",

			"bitw 0 0 0 0 16\n"
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

SCENARIO("BITW: Shift value not zero", "[bitw]")
{
	GIVEN("A program containing a BITW where the shift value should be zero but is not")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"BITW: Literal value not zero",

			"bitw 0 1 1 0 2\n"
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

SCENARIO("BITW: Inversion value not zero", "[bitw]")
{
	GIVEN("A program containing a BITW where the inversion value should be zero but is not")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"BITW: Literal value not zero",

			"bitw 0 1 1 1 0\n"
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

SCENARIO("BITW: Label refs as arguments", "[bitw]")
{
	GIVEN("A program containing a BITW with label refs used as register IDs")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"BITW: Label refs as register IDs",

			":label\n"
			"bitw <:label >:label 0 0 0\n"
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

	AND_GIVEN("A program containing a BITW with a label ref as the inversion value")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"BITW: Label refs as inversion value",

			":label\n"
			"bitw 0 0 0 0 0\n"
			"bitw 0 0 0 ~:label 1\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler) == V2MPASM_FAILED);

			THEN("An error should be raised that the argument type is invalid")
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

	AND_GIVEN("A program containing a BITW with a label ref as a shift value")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"BITW: Label ref as shift value",

			":label\n"
			"bitw 0 0 0 0 0\n"
			"bitw 0 0 0 0 ~+:label\n"
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
						Asm::BITWL(Asm::REG_R0, Asm::BitwiseOp::AND, 0, false),
						Asm::BITWL(Asm::REG_R0, Asm::BitwiseOp::AND, 2, false),
					}
				);
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("BITW: Valid permutations", "[bitw]")
{
	GIVEN("A program containing all valid BITW permutations which don't involve literal values")
	{
		std::stringstream stream;

		// R0, R1, LR, PC
		for ( int arg0 = 0; arg0 < 4; ++arg0 )
		{
			// R0, R1, LR, PC
			for ( int arg1 = 0; arg1 < 4; ++arg1 )
			{
				if ( arg0 == arg1 )
				{
					continue;
				}

				// AND, OR, XOR, NOT
				for ( int arg2 = 0; arg2 < 4; ++arg2 )
				{
					stream << "bitw " << arg0 << " " << arg1 << " " << arg2 << " 0 0\n";
				}
			}
		}

		const std::string program = stream.str();

		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"BITW: Valid permutations",

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
						Asm::BITWR(Asm::REG_R0, Asm::REG_R1, Asm::BitwiseOp::AND),
						Asm::BITWR(Asm::REG_R0, Asm::REG_R1, Asm::BitwiseOp::OR),
						Asm::BITWR(Asm::REG_R0, Asm::REG_R1, Asm::BitwiseOp::XOR),
						Asm::BITWR(Asm::REG_R0, Asm::REG_R1, Asm::BitwiseOp::NOT),

						Asm::BITWR(Asm::REG_R0, Asm::REG_LR, Asm::BitwiseOp::AND),
						Asm::BITWR(Asm::REG_R0, Asm::REG_LR, Asm::BitwiseOp::OR),
						Asm::BITWR(Asm::REG_R0, Asm::REG_LR, Asm::BitwiseOp::XOR),
						Asm::BITWR(Asm::REG_R0, Asm::REG_LR, Asm::BitwiseOp::NOT),

						Asm::BITWR(Asm::REG_R0, Asm::REG_PC, Asm::BitwiseOp::AND),
						Asm::BITWR(Asm::REG_R0, Asm::REG_PC, Asm::BitwiseOp::OR),
						Asm::BITWR(Asm::REG_R0, Asm::REG_PC, Asm::BitwiseOp::XOR),
						Asm::BITWR(Asm::REG_R0, Asm::REG_PC, Asm::BitwiseOp::NOT),

						Asm::BITWR(Asm::REG_R1, Asm::REG_R0, Asm::BitwiseOp::AND),
						Asm::BITWR(Asm::REG_R1, Asm::REG_R0, Asm::BitwiseOp::OR),
						Asm::BITWR(Asm::REG_R1, Asm::REG_R0, Asm::BitwiseOp::XOR),
						Asm::BITWR(Asm::REG_R1, Asm::REG_R0, Asm::BitwiseOp::NOT),

						Asm::BITWR(Asm::REG_R1, Asm::REG_LR, Asm::BitwiseOp::AND),
						Asm::BITWR(Asm::REG_R1, Asm::REG_LR, Asm::BitwiseOp::OR),
						Asm::BITWR(Asm::REG_R1, Asm::REG_LR, Asm::BitwiseOp::XOR),
						Asm::BITWR(Asm::REG_R1, Asm::REG_LR, Asm::BitwiseOp::NOT),

						Asm::BITWR(Asm::REG_R1, Asm::REG_PC, Asm::BitwiseOp::AND),
						Asm::BITWR(Asm::REG_R1, Asm::REG_PC, Asm::BitwiseOp::OR),
						Asm::BITWR(Asm::REG_R1, Asm::REG_PC, Asm::BitwiseOp::XOR),
						Asm::BITWR(Asm::REG_R1, Asm::REG_PC, Asm::BitwiseOp::NOT),

						Asm::BITWR(Asm::REG_LR, Asm::REG_R0, Asm::BitwiseOp::AND),
						Asm::BITWR(Asm::REG_LR, Asm::REG_R0, Asm::BitwiseOp::OR),
						Asm::BITWR(Asm::REG_LR, Asm::REG_R0, Asm::BitwiseOp::XOR),
						Asm::BITWR(Asm::REG_LR, Asm::REG_R0, Asm::BitwiseOp::NOT),

						Asm::BITWR(Asm::REG_LR, Asm::REG_R1, Asm::BitwiseOp::AND),
						Asm::BITWR(Asm::REG_LR, Asm::REG_R1, Asm::BitwiseOp::OR),
						Asm::BITWR(Asm::REG_LR, Asm::REG_R1, Asm::BitwiseOp::XOR),
						Asm::BITWR(Asm::REG_LR, Asm::REG_R1, Asm::BitwiseOp::NOT),

						Asm::BITWR(Asm::REG_LR, Asm::REG_PC, Asm::BitwiseOp::AND),
						Asm::BITWR(Asm::REG_LR, Asm::REG_PC, Asm::BitwiseOp::OR),
						Asm::BITWR(Asm::REG_LR, Asm::REG_PC, Asm::BitwiseOp::XOR),
						Asm::BITWR(Asm::REG_LR, Asm::REG_PC, Asm::BitwiseOp::NOT),

						Asm::BITWR(Asm::REG_PC, Asm::REG_R0, Asm::BitwiseOp::AND),
						Asm::BITWR(Asm::REG_PC, Asm::REG_R0, Asm::BitwiseOp::OR),
						Asm::BITWR(Asm::REG_PC, Asm::REG_R0, Asm::BitwiseOp::XOR),
						Asm::BITWR(Asm::REG_PC, Asm::REG_R0, Asm::BitwiseOp::NOT),

						Asm::BITWR(Asm::REG_PC, Asm::REG_R1, Asm::BitwiseOp::AND),
						Asm::BITWR(Asm::REG_PC, Asm::REG_R1, Asm::BitwiseOp::OR),
						Asm::BITWR(Asm::REG_PC, Asm::REG_R1, Asm::BitwiseOp::XOR),
						Asm::BITWR(Asm::REG_PC, Asm::REG_R1, Asm::BitwiseOp::NOT),

						Asm::BITWR(Asm::REG_PC, Asm::REG_LR, Asm::BitwiseOp::AND),
						Asm::BITWR(Asm::REG_PC, Asm::REG_LR, Asm::BitwiseOp::OR),
						Asm::BITWR(Asm::REG_PC, Asm::REG_LR, Asm::BitwiseOp::XOR),
						Asm::BITWR(Asm::REG_PC, Asm::REG_LR, Asm::BitwiseOp::NOT),
					}
				);
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}

	AND_GIVEN("A program containing all valid BITW permutations which do involve literal values")
	{
		std::stringstream stream;

		// R0, R1, LR, PC
		for ( int arg0 = 0; arg0 < 4; ++arg0 )
		{
			// AND, OR, XOR, NOT
			for ( int arg1 = 0; arg1 < 4; ++arg1 )
			{
				// Normal, inverted
				for ( int arg2 = 0; arg2 < 2; ++ arg2 )
				{
					stream << "bitw " << arg0 << " " << arg0 << " " << arg1 << " " << arg2 << " 4\n";
				}
			}
		}

		const std::string program = stream.str();

		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"BITW: Valid permutations",

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
						Asm::BITWL(Asm::REG_R0, Asm::BitwiseOp::AND, 4, false),
						Asm::BITWL(Asm::REG_R0, Asm::BitwiseOp::AND, 4, true),
						Asm::BITWL(Asm::REG_R0, Asm::BitwiseOp::OR, 4, false),
						Asm::BITWL(Asm::REG_R0, Asm::BitwiseOp::OR, 4, true),
						Asm::BITWL(Asm::REG_R0, Asm::BitwiseOp::XOR, 4, false),
						Asm::BITWL(Asm::REG_R0, Asm::BitwiseOp::XOR, 4, true),
						Asm::BITWL(Asm::REG_R0, Asm::BitwiseOp::NOT, 4, false),
						Asm::BITWL(Asm::REG_R0, Asm::BitwiseOp::NOT, 4, true),

						Asm::BITWL(Asm::REG_R1, Asm::BitwiseOp::AND, 4, false),
						Asm::BITWL(Asm::REG_R1, Asm::BitwiseOp::AND, 4, true),
						Asm::BITWL(Asm::REG_R1, Asm::BitwiseOp::OR, 4, false),
						Asm::BITWL(Asm::REG_R1, Asm::BitwiseOp::OR, 4, true),
						Asm::BITWL(Asm::REG_R1, Asm::BitwiseOp::XOR, 4, false),
						Asm::BITWL(Asm::REG_R1, Asm::BitwiseOp::XOR, 4, true),
						Asm::BITWL(Asm::REG_R1, Asm::BitwiseOp::NOT, 4, false),
						Asm::BITWL(Asm::REG_R1, Asm::BitwiseOp::NOT, 4, true),

						Asm::BITWL(Asm::REG_LR, Asm::BitwiseOp::AND, 4, false),
						Asm::BITWL(Asm::REG_LR, Asm::BitwiseOp::AND, 4, true),
						Asm::BITWL(Asm::REG_LR, Asm::BitwiseOp::OR, 4, false),
						Asm::BITWL(Asm::REG_LR, Asm::BitwiseOp::OR, 4, true),
						Asm::BITWL(Asm::REG_LR, Asm::BitwiseOp::XOR, 4, false),
						Asm::BITWL(Asm::REG_LR, Asm::BitwiseOp::XOR, 4, true),
						Asm::BITWL(Asm::REG_LR, Asm::BitwiseOp::NOT, 4, false),
						Asm::BITWL(Asm::REG_LR, Asm::BitwiseOp::NOT, 4, true),

						Asm::BITWL(Asm::REG_PC, Asm::BitwiseOp::AND, 4, false),
						Asm::BITWL(Asm::REG_PC, Asm::BitwiseOp::AND, 4, true),
						Asm::BITWL(Asm::REG_PC, Asm::BitwiseOp::OR, 4, false),
						Asm::BITWL(Asm::REG_PC, Asm::BitwiseOp::OR, 4, true),
						Asm::BITWL(Asm::REG_PC, Asm::BitwiseOp::XOR, 4, false),
						Asm::BITWL(Asm::REG_PC, Asm::BitwiseOp::XOR, 4, true),
						Asm::BITWL(Asm::REG_PC, Asm::BitwiseOp::NOT, 4, false),
						Asm::BITWL(Asm::REG_PC, Asm::BitwiseOp::NOT, 4, true),
					}
				);
			}
		}
	}
}
