#include <unordered_map>
#include "ProgramModel/InstructionMeta.h"
#include "Utils/StringUtils.h"

namespace V2MPAsm
{
	using InstructionMap = std::unordered_map<std::string, InstructionMeta>;

	InstructionMap g_InstructionMap;

	static void CreateMap()
	{
		g_InstructionMap.clear();

		g_InstructionMap["nop"] =
		{
			InstructionType::NOP,
			{
			}
		};

		g_InstructionMap["add"] =
		{
			InstructionType::ADD,
			{
				{ 11, 10 },
				{ 9, 8 }
			}
		};
	}

	const InstructionMeta* GetInstructionMeta(const std::string& instructionName)
	{
		if ( g_InstructionMap.empty() )
		{
			CreateMap();
		}

		const InstructionMap::const_iterator it = g_InstructionMap.find(ToLowercase(instructionName));

		return it != g_InstructionMap.end() ? &it->second : nullptr;
	}
}
