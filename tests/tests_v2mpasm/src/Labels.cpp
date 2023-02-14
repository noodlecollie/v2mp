#include "catch2/catch.hpp"
#include "V2MPAsm/Assembler.h"
#include "V2MPAsm/Exception.h"
#include "ProgramVerification.h"
#include "ExceptionIDs.h"

SCENARIO("Label with no subsequent instruction", "[labels]")
{
	GIVEN("An assembler instance and a program with a label at the end")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"Label at end",

			"nop\n"
			":discarded_label\n"
			"// The label above should be discarded, even with this comment here\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler) == V2MPASM_COMPLETED_WITH_WARNINGS);

			THEN("A warning should be raised that the label will be discarded")
			{
				CHECK(V2MPAsm_Assembler_GetExceptionCount(assembler) == 1);

				const V2MPAsm_Exception* exception = V2MPAsm_Assembler_GetException(assembler, 0);
				REQUIRE(exception);

				CHECK(V2MPAsm_Exception_GetType(exception) == V2MPASM_EXCEPTION_WARNING);
				CHECK(std::string(V2MPAsm_Exception_GetID(exception)) == std::string(EXCEPTION_ID_LABEL_DISCARDED));
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("Consecutive duplicate labels", "[labels]")
{
	GIVEN("An assembler instance and a program with two sequential labels of the same name")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"Duplicate label",

			":twice\n"
			":twice\n"
			"nop\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler) == V2MPASM_COMPLETED_WITH_WARNINGS);

			THEN("A warning should be raised that the second label is redundant")
			{
				CHECK(V2MPAsm_Assembler_GetExceptionCount(assembler) == 1);

				const V2MPAsm_Exception* exception = V2MPAsm_Assembler_GetException(assembler, 0);
				REQUIRE(exception);

				CHECK(V2MPAsm_Exception_GetType(exception) == V2MPASM_EXCEPTION_WARNING);
				CHECK(std::string(V2MPAsm_Exception_GetID(exception)) == std::string(EXCEPTION_ID_REDUNDANT_LABEL));
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("Duplicate labels", "[labels]")
{
	GIVEN("An assembler instance and a program with two non-sequential labels of the same name")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"Duplicate label",

			":twice\n"
			"nop\n"
			":twice\n"
			"nop\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler) == V2MPASM_FAILED);

			THEN("An error should be raised that the second label's name already exists")
			{
				CHECK(V2MPAsm_Assembler_GetExceptionCount(assembler) == 1);

				const V2MPAsm_Exception* exception = V2MPAsm_Assembler_GetException(assembler, 0);
				REQUIRE(exception);

				CHECK(V2MPAsm_Exception_GetType(exception) == V2MPASM_EXCEPTION_ERROR);
				CHECK(std::string(V2MPAsm_Exception_GetID(exception)) == std::string(EXCEPTION_ID_DUPLICATE_LABEL));
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("Multiple labels for same instruction", "[labels]")
{
	GIVEN("An assembler instance and a program with two sequential labels with different names")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"Sequential labels",

			":first\n"
			":second\n"
			"asgn 0 0 ~:first\n"
			"asgn 1 1 ~:second\n"
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
						Asm::IASGNL(Asm::REG_R0, -1),
						Asm::IASGNL(Asm::REG_R1, -2),
					}
				);
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}
