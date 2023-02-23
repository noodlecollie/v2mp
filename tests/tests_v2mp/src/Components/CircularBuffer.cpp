#include <cstring>
#include "catch2/catch.hpp"
#include "LibSharedComponents/CircularBuffer.h"

static constexpr size_t DEFAULT_CAPACITY = 32;

SCENARIO("Initialising a circular buffer", "[components]")
{
	WHEN("A circular buffer is initialised with a capacity of zero")
	{
		V2MPSC_CircularBuffer* cb = V2MPSC_CircularBuffer_AllocateAndInit(0);

		THEN("A null pointer is returned")
		{
			REQUIRE(cb == nullptr);
			REQUIRE_NOTHROW(V2MPSC_CircularBuffer_DeinitAndFree(cb));
		}
	}

	AND_WHEN("A circular buffer is initialised with a non-zero capacity")
	{
		V2MPSC_CircularBuffer* cb = V2MPSC_CircularBuffer_AllocateAndInit(DEFAULT_CAPACITY);

		THEN("The circular buffer returned is valid")
		{
			REQUIRE(cb != nullptr);
		}

		V2MPSC_CircularBuffer_DeinitAndFree(cb);
	}
}

SCENARIO("Querying circular buffer capacity", "[components]")
{
	GIVEN("A valid circular buffer")
	{
		V2MPSC_CircularBuffer* cb = V2MPSC_CircularBuffer_AllocateAndInit(DEFAULT_CAPACITY);
		REQUIRE(cb);

		WHEN("The circular buffer's capacity is fetched")
		{
			const size_t capacity = V2MPSC_CircularBuffer_Capacity(cb);

			THEN("The reported capacity matches the capacity the buffer was initialised with")
			{
				REQUIRE(capacity == DEFAULT_CAPACITY);
			}

			AND_WHEN("The circular buffer's free space is fetched")
			{
				const size_t freeSpace = V2MPSC_CircularBuffer_BytesFree(cb);

				THEN("The reported free space is correct")
				{
					REQUIRE(freeSpace == DEFAULT_CAPACITY);
					REQUIRE(freeSpace == capacity);
				}
			}

			AND_WHEN("The circular buffer's number of used bytes is fetched")
			{
				size_t usedBytes = V2MPSC_CircularBuffer_BytesUsed(cb);

				THEN("The number of used bytes is zero")
				{
					REQUIRE(usedBytes == 0);
				}
			}

			AND_WHEN("The circular buffer is queried about being full")
			{
				const bool isFull = V2MPSC_CircularBuffer_IsFull(cb);

				THEN("The buffer is not reported as full.")
				{
					REQUIRE_FALSE(isFull);
				}
			}

			AND_WHEN("The circular buffer is queried about being empty")
			{
				const bool isEmpty = V2MPSC_CircularBuffer_IsEmpty(cb);

				THEN("The buffer is reported as empty.")
				{
					REQUIRE(isEmpty);
				}
			}
		}

		V2MPSC_CircularBuffer_DeinitAndFree(cb);
	}
}

SCENARIO("Modifying circular buffer contents", "[components]")
{
	GIVEN("A valid circular buffer")
	{
		V2MPSC_CircularBuffer* cb = V2MPSC_CircularBuffer_AllocateAndInit(DEFAULT_CAPACITY);
		REQUIRE(cb);

		WHEN("A small amount of data is written to the buffer")
		{
			const std::string dataString("This is an ASCII message");

			std::vector<char> origData;
			origData.resize(dataString.length() + 1);
			std::memcpy(origData.data(), dataString.c_str(), origData.size());

			const size_t bytesWritten = V2MPSC_CircularBuffer_WriteData(
				cb,
				reinterpret_cast<const uint8_t*>(origData.data()),
				origData.size()
			);

			THEN("The amount of data written is correct")
			{
				const size_t used = V2MPSC_CircularBuffer_BytesUsed(cb);
				const size_t free = V2MPSC_CircularBuffer_BytesFree(cb);

				REQUIRE(bytesWritten == origData.size());
				REQUIRE(used == origData.size());
				REQUIRE(used == bytesWritten);
				REQUIRE(free == DEFAULT_CAPACITY - origData.size());
				REQUIRE_FALSE(V2MPSC_CircularBuffer_IsEmpty(cb));
				REQUIRE_FALSE(V2MPSC_CircularBuffer_IsFull(cb));
			}

			AND_WHEN("The data is read from the buffer")
			{
				std::vector<char> readData;
				readData.resize(origData.size());

				const size_t bytesRead = V2MPSC_CircularBuffer_ReadData(
					cb,
					reinterpret_cast<uint8_t*>(readData.data()),
					readData.size()
				);

				THEN("The read data matches the original data")
				{
					REQUIRE(bytesRead == readData.size());
					REQUIRE(origData == readData);
				}
			}
		}

		AND_WHEN("Too much data is written to the buffer")
		{
			const std::string dataString("This is a message which is longer than the capacity of the buffer");

			std::vector<char> origData;
			origData.resize(dataString.length() + 1);
			std::memcpy(origData.data(), dataString.c_str(), origData.size());

			const size_t bytesWritten = V2MPSC_CircularBuffer_WriteData(
				cb,
				reinterpret_cast<const uint8_t*>(origData.data()),
				origData.size()
			);

			THEN("The amount of data written is equal to the capacity of the buffer")
			{
				REQUIRE(bytesWritten != origData.size());
				REQUIRE(bytesWritten == DEFAULT_CAPACITY);
				REQUIRE(bytesWritten == V2MPSC_CircularBuffer_Capacity(cb));
				REQUIRE_FALSE(V2MPSC_CircularBuffer_IsEmpty(cb));
				REQUIRE(V2MPSC_CircularBuffer_IsFull(cb));
			}

			AND_WHEN("The data is read from the buffer")
			{
				std::vector<char> readData;
				readData.resize(origData.size());

				const size_t bytesRead = V2MPSC_CircularBuffer_ReadData(
					cb,
					reinterpret_cast<uint8_t*>(readData.data()),
					readData.size()
				);

				THEN("The data read back matches the beginning of the original data")
				{
					REQUIRE(bytesRead == V2MPSC_CircularBuffer_Capacity(cb));
					REQUIRE(bytesRead != origData.size());

					std::vector<char> origDataPrefix(origData.cbegin(), origData.cbegin() + DEFAULT_CAPACITY);
					origDataPrefix.resize(readData.size());

					REQUIRE(readData == origDataPrefix);
				}
			}
		}

		AND_WHEN("Invalid data is written to the buffer")
		{
			std::vector<char> data;

			const size_t bytesWritten = V2MPSC_CircularBuffer_WriteData(
				cb,
				reinterpret_cast<const uint8_t*>(data.data()),
				data.size()
			);

			THEN("The amount of data written is zero")
			{
				REQUIRE(bytesWritten == 0);
				REQUIRE(V2MPSC_CircularBuffer_IsEmpty(cb));
				REQUIRE_FALSE(V2MPSC_CircularBuffer_IsFull(cb));
			}

			AND_WHEN("The data is read from the buffer")
			{
				std::vector<char> readData(DEFAULT_CAPACITY, 'A');

				const size_t bytesRead = V2MPSC_CircularBuffer_ReadData(
					cb,
					reinterpret_cast<uint8_t*>(readData.data()),
					readData.size()
				);

				THEN("No data is read")
				{
					REQUIRE(bytesRead == 0);
					REQUIRE(readData == std::vector<char>(DEFAULT_CAPACITY, 'A'));
				}
			}
		}

		V2MPSC_CircularBuffer_DeinitAndFree(cb);
	}
}
