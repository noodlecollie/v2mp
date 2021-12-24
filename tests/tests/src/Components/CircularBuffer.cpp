#include <cstring>
#include "catch2/catch.hpp"
#include "V2MPComponents/CircularBuffer.h"

static constexpr size_t DEFAULT_CAPACITY = 1337;

SCENARIO("Initialising a circular buffer", "[components]")
{
	WHEN("A circular buffer is initialised with a capacity of zero")
	{
		V2MPCpts_CircularBuffer* cb = V2MPCpts_CircularBuffer_AllocateAndInit(0);

		THEN("A null pointer is returned")
		{
			REQUIRE(cb == nullptr);
		}

		AND_THEN("Deinitialising the null pointer is safe")
		{
			REQUIRE_NOTHROW(V2MPCpts_CircularBuffer_DeinitAndFree(cb));
		}
	}

	AND_WHEN("A circular buffer is initialised with a non-zero capacity")
	{
		V2MPCpts_CircularBuffer* cb = V2MPCpts_CircularBuffer_AllocateAndInit(DEFAULT_CAPACITY);

		THEN("The circular buffer returned is valid")
		{
			REQUIRE(cb != nullptr);
		}

		V2MPCpts_CircularBuffer_DeinitAndFree(cb);
	}
}

SCENARIO("Querying circular buffer capacity", "[components]")
{
	GIVEN("A valid circular buffer")
	{
		V2MPCpts_CircularBuffer* cb = V2MPCpts_CircularBuffer_AllocateAndInit(DEFAULT_CAPACITY);
		REQUIRE(cb);

		WHEN("The circular buffer's capacity is fetched")
		{
			const size_t capacity = V2MPCpts_CircularBuffer_Capacity(cb);

			THEN("The reported capacity matches the capacity the buffer was initialised with")
			{
				REQUIRE(capacity == DEFAULT_CAPACITY);
			}

			AND_WHEN("The circular buffer's free space is fetched")
			{
				const size_t freeSpace = V2MPCpts_CircularBuffer_BytesFree(cb);

				THEN("The reported free space matches the capacity the buffer was initialised with")
				{
					REQUIRE(freeSpace == DEFAULT_CAPACITY);
				}

				AND_THEN("The reported free space matches the reported capacity")
				{
					REQUIRE(freeSpace == capacity);
				}
			}

			AND_WHEN("The circular buffer's number of used bytes is fetched")
			{
				size_t usedBytes = V2MPCpts_CircularBuffer_BytesUsed(cb);

				THEN("The number of used bytes is zero")
				{
					REQUIRE(usedBytes == 0);
				}
			}

			AND_WHEN("The circular buffer is queried about being full")
			{
				const bool isFull = V2MPCpts_CircularBuffer_IsFull(cb);

				THEN("The buffer is not reported as full.")
				{
					REQUIRE_FALSE(isFull);
				}
			}

			AND_WHEN("The circular buffer is queried about being empty")
			{
				const bool isEmpty = V2MPCpts_CircularBuffer_IsEmpty(cb);

				THEN("The buffer is reported as empty.")
				{
					REQUIRE(isEmpty);
				}
			}
		}

		V2MPCpts_CircularBuffer_DeinitAndFree(cb);
	}
}

SCENARIO("Modifying circular buffer contents", "[components]")
{
	GIVEN("A valid circular buffer")
	{
		V2MPCpts_CircularBuffer* cb = V2MPCpts_CircularBuffer_AllocateAndInit(DEFAULT_CAPACITY);
		REQUIRE(cb);

		WHEN("Data is written to the buffer")
		{
			const std::string dataString("This is an ASCII message");

			std::vector<char> origData;
			origData.resize(dataString.length() + 1);
			std::memcpy(origData.data(), dataString.c_str(), origData.size());

			const size_t bytesWritten = V2MPCpts_CircularBuffer_WriteData(
				cb,
				reinterpret_cast<const uint8_t*>(origData.data()),
				origData.size()
			);

			THEN("The amount of data written matches the length of the input data")
			{
				REQUIRE(bytesWritten == origData.size());
			}

			AND_THEN("The number of used bytes in the buffer matches the length of the input data")
			{
				const size_t used = V2MPCpts_CircularBuffer_BytesUsed(cb);

				REQUIRE(used == origData.size());
				REQUIRE(used == bytesWritten);
			}

			AND_WHEN("The data is read from the buffer")
			{
				std::vector<char> readData;
				readData.resize(origData.size());

				const size_t bytesRead = V2MPCpts_CircularBuffer_ReadData(
					cb,
					reinterpret_cast<uint8_t*>(readData.data()),
					readData.size()
				);

				THEN("The amount of data read should match the amount requested")
				{
					REQUIRE(bytesRead == readData.size());
				}

				AND_THEN("The data read back should match the data that was written")
				{
					REQUIRE(origData == readData);
				}
			}
		}

		V2MPCpts_CircularBuffer_DeinitAndFree(cb);
	}
}
