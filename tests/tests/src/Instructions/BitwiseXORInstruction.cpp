#include "catch2/catch.hpp"
#include "Helpers/TestHarnessVM.h"
#include "Helpers/Assembly.h"

static constexpr V2MP_Word ALTERNATE_BITS_A = 0x5A5A;
static constexpr V2MP_Word ALTERNATE_BITS_B = 0xA5A5;
static constexpr V2MP_Word ALL_ONES = 0xFFFF;
static constexpr V2MP_Word ALL_ZEROES = 0x0000;

SCENARIO("BITW: Performing a bitwise XOR between two registers results in the correct value in the destination register", "[instructions]")
{
	GIVEN("A virtual machine")
	{
		TestHarnessVM vm;

		AND_GIVEN("The destination register is R0")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_R0;

			vm.SetR0(ALTERNATE_BITS_A);

			AND_GIVEN("The source register is R1")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_R1;

				WHEN("A bitwise XOR is performed where the source register's value is all ones")
				{
					vm.SetR1(ALL_ONES);

					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::XOR)));

					THEN("The destination register's bits should all be toggled, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == ALTERNATE_BITS_B);
						CHECK(vm.GetR1() == ALL_ONES);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}

				AND_WHEN("A bitwise XOR is performed where the source register's value is all zeroes")
				{
					vm.SetR1(ALL_ZEROES);

					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::XOR)));

					THEN("The destination register's value should be unchanged, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == ALTERNATE_BITS_A);
						CHECK(vm.GetR1() == ALL_ZEROES);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}

			AND_GIVEN("The source register is LR")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_LR;

				WHEN("A bitwise XOR is performed where the source register's value is all ones")
				{
					vm.SetLR(ALL_ONES);

					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::XOR)));

					THEN("The destination register's bits should all be toggled, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == ALTERNATE_BITS_B);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == ALL_ONES);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}

				AND_WHEN("A bitwise XOR is performed where the source register's value is all zeroes")
				{
					vm.SetLR(ALL_ZEROES);

					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::XOR)));

					THEN("The destination register's value should be unchanged, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == ALTERNATE_BITS_A);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == ALL_ZEROES);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}

			AND_GIVEN("The source register is PC")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_PC;

				WHEN("A bitwise XOR is performed where the source register's value is all ones")
				{
					vm.SetPC(ALL_ONES);

					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::XOR)));

					THEN("The destination register's bits should all be toggled, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == ALTERNATE_BITS_B);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == ALL_ONES);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}

				AND_WHEN("A bitwise XOR is performed where the source register's value is all zeroes")
				{
					vm.SetPC(ALL_ZEROES);

					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::XOR)));

					THEN("The destination register's value should be unchanged, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == ALTERNATE_BITS_A);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == ALL_ZEROES);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}
		}

		AND_GIVEN("The destination register is R1")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_R1;

			vm.SetR1(ALTERNATE_BITS_A);

			AND_GIVEN("The source register is R0")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_R0;

				WHEN("A bitwise XOR is performed where the source register's value is all ones")
				{
					vm.SetR0(ALL_ONES);

					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::XOR)));

					THEN("The destination register's bits should all be toggled, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == ALL_ONES);
						CHECK(vm.GetR1() == ALTERNATE_BITS_B);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}

				AND_WHEN("A bitwise XOR is performed where the source register's value is all zeroes")
				{
					vm.SetR0(ALL_ZEROES);

					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::XOR)));

					THEN("The destination register's value should be unchanged, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == ALL_ZEROES);
						CHECK(vm.GetR1() == ALTERNATE_BITS_A);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}

			AND_GIVEN("The source register is LR")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_LR;

				WHEN("A bitwise XOR is performed where the source register's value is all ones")
				{
					vm.SetLR(ALL_ONES);

					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::XOR)));

					THEN("The destination register's bits should all be toggled, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == ALTERNATE_BITS_B);
						CHECK(vm.GetLR() == ALL_ONES);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}

				AND_WHEN("A bitwise XOR is performed where the source register's value is all zeroes")
				{
					vm.SetLR(ALL_ZEROES);

					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::XOR)));

					THEN("The destination register's value should be unchanged, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == ALTERNATE_BITS_A);
						CHECK(vm.GetLR() == ALL_ZEROES);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}

			AND_GIVEN("The source register is PC")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_PC;

				WHEN("A bitwise XOR is performed where the source register's value is all ones")
				{
					vm.SetPC(ALL_ONES);

					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::XOR)));

					THEN("The destination register's bits should all be toggled, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == ALTERNATE_BITS_B);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == ALL_ONES);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}

				AND_WHEN("A bitwise XOR is performed where the source register's value is all zeroes")
				{
					vm.SetPC(ALL_ZEROES);

					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::XOR)));

					THEN("The destination register's value should be unchanged, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == ALTERNATE_BITS_A);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == ALL_ZEROES);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}
		}

		AND_GIVEN("The destination register is LR")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_LR;

			vm.SetLR(ALTERNATE_BITS_A);

			AND_GIVEN("The source register is R0")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_R0;

				WHEN("A bitwise XOR is performed where the source register's value is all ones")
				{
					vm.SetR0(ALL_ONES);

					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::XOR)));

					THEN("The destination register's bits should all be toggled, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == ALL_ONES);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == ALTERNATE_BITS_B);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}

				AND_WHEN("A bitwise XOR is performed where the source register's value is all zeroes")
				{
					vm.SetR0(ALL_ZEROES);

					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::XOR)));

					THEN("The destination register's value should be unchanged, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == ALL_ZEROES);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == ALTERNATE_BITS_A);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}

			AND_GIVEN("The source register is R1")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_R1;

				WHEN("A bitwise XOR is performed where the source register's value is all ones")
				{
					vm.SetR1(ALL_ONES);

					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::XOR)));

					THEN("The destination register's bits should all be toggled, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == ALL_ONES);
						CHECK(vm.GetLR() == ALTERNATE_BITS_B);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}

				AND_WHEN("A bitwise XOR is performed where the source register's value is all zeroes")
				{
					vm.SetR1(ALL_ZEROES);

					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::XOR)));

					THEN("The destination register's value should be unchanged, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == ALL_ZEROES);
						CHECK(vm.GetLR() == ALTERNATE_BITS_A);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}

			AND_GIVEN("The source register is PC")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_PC;

				WHEN("A bitwise XOR is performed where the source register's value is all ones")
				{
					vm.SetPC(ALL_ONES);

					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::XOR)));

					THEN("The destination register's bits should all be toggled, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == ALTERNATE_BITS_B);
						CHECK(vm.GetPC() == ALL_ONES);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}

				AND_WHEN("A bitwise XOR is performed where the source register's value is all zeroes")
				{
					vm.SetPC(ALL_ZEROES);

					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::XOR)));

					THEN("The destination register's value should be unchanged, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == ALTERNATE_BITS_A);
						CHECK(vm.GetPC() == ALL_ZEROES);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}
		}

		AND_GIVEN("The destination register is PC")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_PC;

			vm.SetPC(ALTERNATE_BITS_A);

			AND_GIVEN("The source register is R0")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_R0;

				WHEN("A bitwise XOR is performed where the source register's value is all ones")
				{
					vm.SetR0(ALL_ONES);

					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::XOR)));

					THEN("The destination register's bits should all be toggled, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == ALL_ONES);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == ALTERNATE_BITS_B);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}

				AND_WHEN("A bitwise XOR is performed where the source register's value is all zeroes")
				{
					vm.SetR0(ALL_ZEROES);

					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::XOR)));

					THEN("The destination register's value should be unchanged, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == ALL_ZEROES);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == ALTERNATE_BITS_A);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}

			AND_GIVEN("The source register is R1")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_R1;

				WHEN("A bitwise XOR is performed where the source register's value is all ones")
				{
					vm.SetR1(ALL_ONES);

					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::XOR)));

					THEN("The destination register's bits should all be toggled, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == ALL_ONES);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == ALTERNATE_BITS_B);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}

				AND_WHEN("A bitwise XOR is performed where the source register's value is all zeroes")
				{
					vm.SetR1(ALL_ZEROES);

					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::XOR)));

					THEN("The destination register's value should be unchanged, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == ALL_ZEROES);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == ALTERNATE_BITS_A);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}

			AND_GIVEN("The source register is LR")
			{
				static constexpr uint8_t REG_SRC = Asm::REG_LR;

				WHEN("A bitwise XOR is performed where the source register's value is all ones")
				{
					vm.SetLR(ALL_ONES);

					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::XOR)));

					THEN("The destination register's bits should all be toggled, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == ALL_ONES);
						CHECK(vm.GetPC() == ALTERNATE_BITS_B);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}

				AND_WHEN("A bitwise XOR is performed where the source register's value is all zeroes")
				{
					vm.SetLR(ALL_ZEROES);

					REQUIRE(vm.Execute(Asm::BITWR(REG_SRC, REG_DEST, Asm::BitwiseOp::XOR)));

					THEN("The destination register's value should be unchanged, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == ALL_ZEROES);
						CHECK(vm.GetPC() == ALTERNATE_BITS_A);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}
		}
	}
}

SCENARIO("BITW: Performing a bitwise XOR using a constructed mask results in the correct value in the destination register", "[instructions]")
{
	GIVEN("A virtual machine")
	{
		TestHarnessVM vm;

		AND_GIVEN("The destination register is R0, filled with all-zeroes")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_R0;

			for ( uint8_t shift = 0; shift < 16; ++shift )
			{
				vm.SetR0(ALL_ZEROES);

				WHEN("A bitwise XOR is performed with a specific bit chosen")
				{
					REQUIRE(vm.Execute(Asm::BITWL(REG_DEST, Asm::BitwiseOp::XOR, shift, false)));

					THEN("Only this bit should be present in the register, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == static_cast<V2MP_Word>(1 << shift));
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}

			for ( uint8_t shift = 0; shift < 16; ++shift )
			{
				vm.SetR0(ALL_ZEROES);

				AND_WHEN("A bitwise XOR is performed with a specific bit excluded")
				{
					REQUIRE(vm.Execute(Asm::BITWL(REG_DEST, Asm::BitwiseOp::XOR, shift, true)));

					THEN("Only this bit should be absent in the register, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == static_cast<V2MP_Word>(~static_cast<V2MP_Word>(1 << shift)));
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}
		}

		AND_GIVEN("The destination register is R0, filled with all-ones")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_R0;

			for ( uint8_t shift = 0; shift < 16; ++shift )
			{
				vm.SetR0(ALL_ONES);

				WHEN("A bitwise XOR is performed with a specific bit chosen")
				{
					REQUIRE(vm.Execute(Asm::BITWL(REG_DEST, Asm::BitwiseOp::XOR, shift, false)));

					THEN("Only this bit should be absent in the register, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == static_cast<V2MP_Word>(~static_cast<V2MP_Word>(1 << shift)));
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}

			for ( uint8_t shift = 0; shift < 16; ++shift )
			{
				vm.SetR0(ALL_ONES);

				AND_WHEN("A bitwise XOR is performed with a specific bit excluded")
				{
					REQUIRE(vm.Execute(Asm::BITWL(REG_DEST, Asm::BitwiseOp::XOR, shift, true)));

					THEN("Only this bit should be present in the register, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == static_cast<V2MP_Word>(1 << shift));
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}
		}

		AND_GIVEN("The destination register is R1, filled with all-zeroes")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_R1;

			for ( uint8_t shift = 0; shift < 16; ++shift )
			{
				vm.SetR1(ALL_ZEROES);

				WHEN("A bitwise XOR is performed with a specific bit chosen")
				{
					REQUIRE(vm.Execute(Asm::BITWL(REG_DEST, Asm::BitwiseOp::XOR, shift, false)));

					THEN("Only this bit should be present in the register, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == static_cast<V2MP_Word>(1 << shift));
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}

			for ( uint8_t shift = 0; shift < 16; ++shift )
			{
				vm.SetR1(ALL_ZEROES);

				AND_WHEN("A bitwise XOR is performed with a specific bit excluded")
				{
					REQUIRE(vm.Execute(Asm::BITWL(REG_DEST, Asm::BitwiseOp::XOR, shift, true)));

					THEN("Only this bit should be absent in the register, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == static_cast<V2MP_Word>(~static_cast<V2MP_Word>(1 << shift)));
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}
		}

		AND_GIVEN("The destination register is R1, filled with all-ones")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_R1;

			for ( uint8_t shift = 0; shift < 16; ++shift )
			{
				vm.SetR1(ALL_ONES);

				WHEN("A bitwise XOR is performed with a specific bit chosen")
				{
					REQUIRE(vm.Execute(Asm::BITWL(REG_DEST, Asm::BitwiseOp::XOR, shift, false)));

					THEN("Only this bit should be absent in the register, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == static_cast<V2MP_Word>(~static_cast<V2MP_Word>(1 << shift)));
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}

			for ( uint8_t shift = 0; shift < 16; ++shift )
			{
				vm.SetR1(ALL_ONES);

				AND_WHEN("A bitwise XOR is performed with a specific bit excluded")
				{
					REQUIRE(vm.Execute(Asm::BITWL(REG_DEST, Asm::BitwiseOp::XOR, shift, true)));

					THEN("Only this bit should be present in the register, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == static_cast<V2MP_Word>(1 << shift));
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}
		}

		AND_GIVEN("The destination register is LR, filled with all-zeroes")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_LR;

			for ( uint8_t shift = 0; shift < 16; ++shift )
			{
				vm.SetLR(ALL_ZEROES);

				WHEN("A bitwise XOR is performed with a specific bit chosen")
				{
					REQUIRE(vm.Execute(Asm::BITWL(REG_DEST, Asm::BitwiseOp::XOR, shift, false)));

					THEN("Only this bit should be present in the register, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == static_cast<V2MP_Word>(1 << shift));
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}

			for ( uint8_t shift = 0; shift < 16; ++shift )
			{
				vm.SetLR(ALL_ZEROES);

				AND_WHEN("A bitwise XOR is performed with a specific bit excluded")
				{
					REQUIRE(vm.Execute(Asm::BITWL(REG_DEST, Asm::BitwiseOp::XOR, shift, true)));

					THEN("Only this bit should be absent in the register, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == static_cast<V2MP_Word>(~static_cast<V2MP_Word>(1 << shift)));
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}
		}

		AND_GIVEN("The destination register is LR, filled with all-ones")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_LR;

			for ( uint8_t shift = 0; shift < 16; ++shift )
			{
				vm.SetLR(ALL_ONES);

				WHEN("A bitwise XOR is performed with a specific bit chosen")
				{
					REQUIRE(vm.Execute(Asm::BITWL(REG_DEST, Asm::BitwiseOp::XOR, shift, false)));

					THEN("Only this bit should be absent in the register, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == static_cast<V2MP_Word>(~static_cast<V2MP_Word>(1 << shift)));
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}

			for ( uint8_t shift = 0; shift < 16; ++shift )
			{
				vm.SetLR(ALL_ONES);

				AND_WHEN("A bitwise XOR is performed with a specific bit excluded")
				{
					REQUIRE(vm.Execute(Asm::BITWL(REG_DEST, Asm::BitwiseOp::XOR, shift, true)));

					THEN("Only this bit should be present in the register, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == static_cast<V2MP_Word>(1 << shift));
						CHECK(vm.GetPC() == 0);
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}
		}

		AND_GIVEN("The destination register is PC, filled with all-zeroes")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_PC;

			for ( uint8_t shift = 0; shift < 16; ++shift )
			{
				vm.SetPC(ALL_ZEROES);

				WHEN("A bitwise XOR is performed with a specific bit chosen")
				{
					REQUIRE(vm.Execute(Asm::BITWL(REG_DEST, Asm::BitwiseOp::XOR, shift, false)));

					THEN("Only this bit should be present in the register, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == static_cast<V2MP_Word>(1 << shift));
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}

			for ( uint8_t shift = 0; shift < 16; ++shift )
			{
				vm.SetPC(ALL_ZEROES);

				AND_WHEN("A bitwise XOR is performed with a specific bit excluded")
				{
					REQUIRE(vm.Execute(Asm::BITWL(REG_DEST, Asm::BitwiseOp::XOR, shift, true)));

					THEN("Only this bit should be absent in the register, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == static_cast<V2MP_Word>(~static_cast<V2MP_Word>(1 << shift)));
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}
		}

		AND_GIVEN("The destination register is PC, filled with all-ones")
		{
			static constexpr uint8_t REG_DEST = Asm::REG_PC;

			for ( uint8_t shift = 0; shift < 16; ++shift )
			{
				vm.SetPC(ALL_ONES);

				WHEN("A bitwise XOR is performed with a specific bit chosen")
				{
					REQUIRE(vm.Execute(Asm::BITWL(REG_DEST, Asm::BitwiseOp::XOR, shift, false)));

					THEN("Only this bit should be absent in the register, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == static_cast<V2MP_Word>(~static_cast<V2MP_Word>(1 << shift)));
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}

			for ( uint8_t shift = 0; shift < 16; ++shift )
			{
				vm.SetPC(ALL_ONES);

				AND_WHEN("A bitwise XOR is performed with a specific bit excluded")
				{
					REQUIRE(vm.Execute(Asm::BITWL(REG_DEST, Asm::BitwiseOp::XOR, shift, true)));

					THEN("Only this bit should be present in the register, and all other registers should be unchanged")
					{
						CHECK(vm.GetR0() == 0);
						CHECK(vm.GetR1() == 0);
						CHECK(vm.GetLR() == 0);
						CHECK(vm.GetPC() == static_cast<V2MP_Word>(1 << shift));
						CHECK_FALSE(vm.CPUHasFault());

						CHECK((vm.GetSR() & Asm::SR_Z) == 0);
						CHECK((vm.GetSR() & Asm::SR_C) == 0);
						CHECK((vm.GetSR() & ~(Asm::SR_Z | Asm::SR_C)) == 0);
					}
				}
			}
		}
	}
}

SCENARIO("BITW: Performing a bitwise XOR between two registers with other operand bits set raises a RES fault", "[instructions]")
{
	GIVEN("A virtual machine with different values in different registers")
	{
		static constexpr V2MP_Word VAL_R0 = 0x0001;
		static constexpr V2MP_Word VAL_R1 = 0x0002;
		static constexpr V2MP_Word VAL_LR = 0x0003;
		static constexpr V2MP_Word VAL_PC = 0x0004;

		TestHarnessVM vm;

		vm.SetR0(VAL_R0);
		vm.SetR1(VAL_R1);
		vm.SetLR(VAL_LR);
		vm.SetPC(VAL_PC);

		for ( size_t index = 0; index <= 5; ++index )
		{
			WHEN("A bitwise XOR is performed with a reserved bit set")
			{
				REQUIRE(vm.Execute(Asm::BITWR(Asm::REG_R1, Asm::REG_R0, Asm::BitwiseOp::XOR) | (1 << index)));

				THEN("A RES fault is raised, and all registers are left unchanged")
				{
					CHECK(vm.CPUHasFault());
					CHECK(Asm::FaultFromWord(vm.GetCPUFaultWord()) == V2MP_FAULT_RES);
					CHECK(vm.GetR0() == VAL_R0);
					CHECK(vm.GetR1() == VAL_R1);
					CHECK(vm.GetLR() == VAL_LR);
					CHECK(vm.GetPC() == VAL_PC);
					CHECK(vm.GetSR() == 0);
				}
			}
		}
	}
}
