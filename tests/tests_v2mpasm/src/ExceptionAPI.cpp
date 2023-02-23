#include <set>
#include <string>
#include "catch2/catch.hpp"
#include "LibV2MPAsm/Exception.h"

static std::vector<std::string> GetWarningIDStringList()
{
	const size_t numItems = V2MPAsm_ExceptionList_GetWarningsListSize();

	std::vector<std::string> items;
	items.reserve(numItems);

	for ( size_t index = 0; index < numItems; ++index )
	{
		const char* str = V2MPAsm_ExceptionList_GetWarningStringID(index);
		items.emplace_back(str ? str : "");
	}

	return items;
}

static std::vector<std::string> GetErrorIDStringList()
{
	const size_t numItems = V2MPAsm_ExceptionList_GetErrorsListSize();

	std::vector<std::string> items;
	items.reserve(numItems);

	for ( size_t index = 0; index < numItems; ++index )
	{
		const char* str = V2MPAsm_ExceptionList_GetErrorStringID(index);
		items.emplace_back(str ? str : "");
	}

	return items;
}

static std::set<std::string> GetWarningIDStringSet()
{
	std::set<std::string> out;
	const std::vector<std::string> list = GetWarningIDStringList();

	for ( const std::string& str : list )
	{
		out.insert(str);
	}

	return out;
}

static std::set<std::string> GetErrorIDStringSet()
{
	std::set<std::string> out;
	const std::vector<std::string> list = GetErrorIDStringList();

	for ( const std::string& str : list )
	{
		out.insert(str);
	}

	return out;
}

SCENARIO("Validity of warning ID strings", "[exception_api]")
{
	GIVEN("A list of all warning ID strings")
	{
		const std::vector<std::string> warningIDs = GetWarningIDStringList();

		WHEN("The list is checked")
		{
			std::set<std::string> set;

			for ( const std::string& str : warningIDs )
			{
				THEN("Each string should be valid")
				{
					CHECK(!str.empty());
				}

				AND_THEN("Each string should be unique")
				{
					CHECK(set.find(str) == set.end());
				}

				set.insert(str);
			}
		}
	}
}

SCENARIO("Validity of error ID strings", "[exception_api]")
{
	GIVEN("A list of all error ID strings")
	{
		const std::vector<std::string> errorIDs = GetErrorIDStringList();

		WHEN("The list is checked")
		{
			std::set<std::string> set;

			for ( const std::string& str : errorIDs )
			{
				THEN("Each string should be valid")
				{
					CHECK(!str.empty());
				}

				AND_THEN("Each string should be unique")
				{
					CHECK(set.find(str) == set.end());
				}

				set.insert(str);
			}
		}
	}
}

SCENARIO("Warning and error ID strings should not overlap", "[exception_api]")
{
	GIVEN("The sets of warning and error ID strings")
	{
		const std::set<std::string> warnings = GetWarningIDStringSet();
		const std::set<std::string> errors = GetErrorIDStringSet();

		WHEN("The sets are compared")
		{
			for ( const std::string& warning : warnings )
			{
				THEN("No warning string should be present in the error set")
				{
					CHECK(errors.find(warning) == errors.end());
				}
			}

			for ( const std::string& error : errors )
			{
				AND_THEN("No error string should be present in the warning set")
				{
					CHECK(warnings.find(error) == warnings.end());
				}
			}
		}
	}
}
