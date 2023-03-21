#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include "LibToolchainComponents/InputFile.h"

namespace LibToolchainComponents
{
	class FilePool
	{
	public:
		std::shared_ptr<InputFile> OpenInputFile(const std::string& path);
		void CloseInputFile(const std::string& path);
		bool IsInputFileOpen(const std::string& path) const;

	private:
		using InputFileMap = std::unordered_map<std::string, std::shared_ptr<InputFile>>;

		InputFileMap m_InputFileMap;
	};
}
