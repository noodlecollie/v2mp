#include "catch2/catch.hpp"
#include "V2MPAsm/Assembler.h"
#include "TestUtil/Assembly.h"

static constexpr V2MP_Word INVALID_WORD = static_cast<V2MP_Word>(~0);

SCENARIO("Assembling a single NOP instruction produces a valid program binary")
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
			CHECK(V2MPAsm_Assembler_Run(assembler));

			THEN("The correct program binary is produced")
			{
				CHECK(V2MPAsm_Assembler_GetExceptionCount(assembler) == 0);
				CHECK(V2MPAsm_Assembler_InMemoryOutputBufferSize(assembler) == sizeof(V2MP_Word));

				V2MP_Word output = INVALID_WORD;
				const size_t outputSize = V2MPAsm_Assembler_TakeInMemoryOutputBuffer(assembler, &output, sizeof(output));

				CHECK(outputSize == sizeof(output));
				CHECK(output == Asm::NOP());
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("Assembling a single LDST instruction produces a valid program binary")
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
			CHECK(V2MPAsm_Assembler_Run(assembler));

			THEN("The correct program binary is produced")
			{
				CHECK(V2MPAsm_Assembler_GetExceptionCount(assembler) == 0);
				CHECK(V2MPAsm_Assembler_InMemoryOutputBufferSize(assembler) == sizeof(V2MP_Word));

				V2MP_Word output = INVALID_WORD;
				const size_t outputSize = V2MPAsm_Assembler_TakeInMemoryOutputBuffer(assembler, &output, sizeof(output));

				CHECK(outputSize == sizeof(output));
				CHECK(output == Asm::LOAD(0));
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("Assembling a single ASGN instruction produces a valid program binary")
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
			CHECK(V2MPAsm_Assembler_Run(assembler));

			THEN("The correct program binary is produced")
			{
				CHECK(V2MPAsm_Assembler_GetExceptionCount(assembler) == 0);
				CHECK(V2MPAsm_Assembler_InMemoryOutputBufferSize(assembler) == sizeof(V2MP_Word));

				V2MP_Word output = INVALID_WORD;
				const size_t outputSize = V2MPAsm_Assembler_TakeInMemoryOutputBuffer(assembler, &output, sizeof(output));

				CHECK(outputSize == sizeof(output));
				CHECK(output == Asm::ASGNL(0, 0));
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("Assembling a single ADD instruction produces a valid program binary")
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
			CHECK(V2MPAsm_Assembler_Run(assembler));

			THEN("The correct program binary is produced")
			{
				CHECK(V2MPAsm_Assembler_GetExceptionCount(assembler) == 0);
				CHECK(V2MPAsm_Assembler_InMemoryOutputBufferSize(assembler) == sizeof(V2MP_Word));

				V2MP_Word output = INVALID_WORD;
				const size_t outputSize = V2MPAsm_Assembler_TakeInMemoryOutputBuffer(assembler, &output, sizeof(output));

				CHECK(outputSize == sizeof(output));
				CHECK(output == Asm::ADDL(0, 0));
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("Assembling a single SUB instruction produces a valid program binary")
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
			CHECK(V2MPAsm_Assembler_Run(assembler));

			THEN("The correct program binary is produced")
			{
				CHECK(V2MPAsm_Assembler_GetExceptionCount(assembler) == 0);
				CHECK(V2MPAsm_Assembler_InMemoryOutputBufferSize(assembler) == sizeof(V2MP_Word));

				V2MP_Word output = INVALID_WORD;
				const size_t outputSize = V2MPAsm_Assembler_TakeInMemoryOutputBuffer(assembler, &output, sizeof(output));

				CHECK(outputSize == sizeof(output));
				CHECK(output == Asm::SUBL(0, 0));
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("Assembling a single SHFT instruction produces a valid program binary")
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
			CHECK(V2MPAsm_Assembler_Run(assembler));

			THEN("The correct program binary is produced")
			{
				CHECK(V2MPAsm_Assembler_GetExceptionCount(assembler) == 0);
				CHECK(V2MPAsm_Assembler_InMemoryOutputBufferSize(assembler) == sizeof(V2MP_Word));

				V2MP_Word output = INVALID_WORD;
				const size_t outputSize = V2MPAsm_Assembler_TakeInMemoryOutputBuffer(assembler, &output, sizeof(output));

				CHECK(outputSize == sizeof(output));
				CHECK(output == Asm::SHFTL(0, 0));
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("Assembling a single BITW instruction produces a valid program binary")
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
			CHECK(V2MPAsm_Assembler_Run(assembler));

			THEN("The correct program binary is produced")
			{
				CHECK(V2MPAsm_Assembler_GetExceptionCount(assembler) == 0);
				CHECK(V2MPAsm_Assembler_InMemoryOutputBufferSize(assembler) == sizeof(V2MP_Word));

				V2MP_Word output = INVALID_WORD;
				const size_t outputSize = V2MPAsm_Assembler_TakeInMemoryOutputBuffer(assembler, &output, sizeof(output));

				CHECK(outputSize == sizeof(output));
				CHECK(output == Asm::BITWL(0, Asm::BitwiseOp::AND, 0, false));
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("Assembling a single CBX instruction produces a valid program binary")
{
	GIVEN("An assembler instance and a valid program")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"CBX Test",
			"cbx 0 0\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler));

			THEN("The correct program binary is produced")
			{
				CHECK(V2MPAsm_Assembler_GetExceptionCount(assembler) == 0);
				CHECK(V2MPAsm_Assembler_InMemoryOutputBufferSize(assembler) == sizeof(V2MP_Word));

				V2MP_Word output = INVALID_WORD;
				const size_t outputSize = V2MPAsm_Assembler_TakeInMemoryOutputBuffer(assembler, &output, sizeof(output));

				CHECK(outputSize == sizeof(output));
				CHECK(output == Asm::BXZL(0));
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("Assembling a single STK instruction produces a valid program binary")
{
	GIVEN("An assembler instance and a valid program")
	{
		V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromMemory(
			"STK Test",
			"stk 0 0\n"
		);

		REQUIRE(assembler);

		WHEN("The assembler is run")
		{
			CHECK(V2MPAsm_Assembler_Run(assembler));

			THEN("The correct program binary is produced")
			{
				CHECK(V2MPAsm_Assembler_GetExceptionCount(assembler) == 0);
				CHECK(V2MPAsm_Assembler_InMemoryOutputBufferSize(assembler) == sizeof(V2MP_Word));

				V2MP_Word output = INVALID_WORD;
				const size_t outputSize = V2MPAsm_Assembler_TakeInMemoryOutputBuffer(assembler, &output, sizeof(output));

				CHECK(outputSize == sizeof(output));
				CHECK(output == Asm::POP(0));
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("Assembling a single MUL instruction produces a valid program binary")
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
			CHECK(V2MPAsm_Assembler_Run(assembler));

			THEN("The correct program binary is produced")
			{
				CHECK(V2MPAsm_Assembler_GetExceptionCount(assembler) == 0);
				CHECK(V2MPAsm_Assembler_InMemoryOutputBufferSize(assembler) == sizeof(V2MP_Word));

				V2MP_Word output = INVALID_WORD;
				const size_t outputSize = V2MPAsm_Assembler_TakeInMemoryOutputBuffer(assembler, &output, sizeof(output));

				CHECK(outputSize == sizeof(output));
				CHECK(output == Asm::MULR(0));
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("Assembling a single DIV instruction produces a valid program binary")
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
			CHECK(V2MPAsm_Assembler_Run(assembler));

			THEN("The correct program binary is produced")
			{
				CHECK(V2MPAsm_Assembler_GetExceptionCount(assembler) == 0);
				CHECK(V2MPAsm_Assembler_InMemoryOutputBufferSize(assembler) == sizeof(V2MP_Word));

				V2MP_Word output = INVALID_WORD;
				const size_t outputSize = V2MPAsm_Assembler_TakeInMemoryOutputBuffer(assembler, &output, sizeof(output));

				CHECK(outputSize == sizeof(output));
				CHECK(output == Asm::DIVR(0));
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}

SCENARIO("Assembling a single SIG instruction produces a valid program binary")
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
			CHECK(V2MPAsm_Assembler_Run(assembler));

			THEN("The correct program binary is produced")
			{
				CHECK(V2MPAsm_Assembler_GetExceptionCount(assembler) == 0);
				CHECK(V2MPAsm_Assembler_InMemoryOutputBufferSize(assembler) == sizeof(V2MP_Word));

				V2MP_Word output = INVALID_WORD;
				const size_t outputSize = V2MPAsm_Assembler_TakeInMemoryOutputBuffer(assembler, &output, sizeof(output));

				CHECK(outputSize == sizeof(output));
				CHECK(output == Asm::SIG());
			}
		}

		V2MPAsm_Assembler_Destroy(assembler);
	}
}
