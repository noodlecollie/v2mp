#include "catch2/catch.hpp"
#include "LibV2MPAsm/Assembler.h"
#include "TestUtil/Assembly.h"
#include "ProgramVerification.h"

static constexpr Asm::Word INVALID_WORD = static_cast<Asm::Word>(~0);

SCENARIO("Assembling a single NOP instruction produces a valid program binary", "[nop]")
{
	GIVEN("An assembler instance and a valid program")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"NOP Test",
			"nop\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler) == V2MPASM_COMPLETED_OK);

			THEN("The correct program binary is produced")
			{
				CheckProgramMatches(assembler, { Asm::NOP() });
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("Assembling a single LDST instruction produces a valid program binary", "[ldst]")
{
	GIVEN("An assembler instance and a valid program")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"LDST Test",
			"ldst 0 0\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler) == V2MPASM_COMPLETED_OK);

			THEN("The correct program binary is produced")
			{
				CheckProgramMatches(assembler, { Asm::LOAD(0) });
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("Assembling a single ASGN instruction produces a valid program binary", "[asgn]")
{
	GIVEN("An assembler instance and a valid program")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"ASGN Test",
			"asgn 0 0 0\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler) == V2MPASM_COMPLETED_OK);

			THEN("The correct program binary is produced")
			{
				CheckProgramMatches(assembler, { Asm::ASGNL(0, 0) });
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("Assembling a single ADD instruction produces a valid program binary", "[add]")
{
	GIVEN("An assembler instance and a valid program")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"ADD Test",
			"add 0 0 0\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler) == V2MPASM_COMPLETED_OK);

			THEN("The correct program binary is produced")
			{
				CheckProgramMatches(assembler, { Asm::ADDL(0, 0) });
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("Assembling a single SUB instruction produces a valid program binary", "[sub]")
{
	GIVEN("An assembler instance and a valid program")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"SUB Test",
			"sub 0 0 0\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler) == V2MPASM_COMPLETED_OK);

			THEN("The correct program binary is produced")
			{
				CheckProgramMatches(assembler, { Asm::SUBL(0, 0) });
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("Assembling a single SHFT instruction produces a valid program binary", "[shft]")
{
	GIVEN("An assembler instance and a valid program")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"SHFT Test",
			"shft 0 0 0\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler) == V2MPASM_COMPLETED_OK);

			THEN("The correct program binary is produced")
			{
				CheckProgramMatches(assembler, { Asm::SHFTL(0, 0) });
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("Assembling a single BITW instruction produces a valid program binary", "[bitw]")
{
	GIVEN("An assembler instance and a valid program")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"BITW Test",
			"bitw 0 0 0 0 0\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler) == V2MPASM_COMPLETED_OK);

			THEN("The correct program binary is produced")
			{
				CheckProgramMatches(assembler, { Asm::BITWL(0, Asm::BitwiseOp::AND, 0, false) });
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("Assembling a single CBX instruction produces a valid program binary", "[cbx]")
{
	GIVEN("An assembler instance and a valid program")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"CBX Test",
			"cbx 0 0 0\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler) == V2MPASM_COMPLETED_OK);

			THEN("The correct program binary is produced")
			{
				CheckProgramMatches(assembler, { Asm::BXZL(0) });
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("Assembling a single STK instruction produces a valid program binary", "[stk]")
{
	GIVEN("An assembler instance and a valid program")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"STK Test",
			"stk 0 1\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler) == V2MPASM_COMPLETED_OK);

			THEN("The correct program binary is produced")
			{
				CheckProgramMatches(assembler, { Asm::POP(1 << Asm::REG_R0) });
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("Assembling a single MUL instruction produces a valid program binary", "[mul]")
{
	GIVEN("An assembler instance and a valid program")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"MUL Test",
			"mul 0 0 0 0\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler) == V2MPASM_COMPLETED_OK);

			THEN("The correct program binary is produced")
			{
				CheckProgramMatches(assembler, { Asm::MULR(0) });
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("Assembling a single DIV instruction produces a valid program binary", "[div]")
{
	GIVEN("An assembler instance and a valid program")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"DIV Test",
			"div 0 0 0 0\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler) == V2MPASM_COMPLETED_OK);

			THEN("The correct program binary is produced")
			{
				CheckProgramMatches(assembler, { Asm::DIVR(0) });
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("Assembling a single SIG instruction produces a valid program binary", "[sig]")
{
	GIVEN("An assembler instance and a valid program")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"SIG Test",
			"sig\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler) == V2MPASM_COMPLETED_OK);

			THEN("The correct program binary is produced")
			{
				CheckProgramMatches(assembler, { Asm::SIG() });
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}
