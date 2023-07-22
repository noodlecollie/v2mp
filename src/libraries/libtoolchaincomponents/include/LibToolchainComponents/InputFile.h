#pragma once

#include <fstream>
#include <string>

namespace LibToolchainComponents
{
	class InputFile
	{
	public:
		explicit InputFile(const std::string& path);

		std::string GetPath() const;
		std::ifstream& GetStream();
		size_t CalculateFileSize();

	private:
		std::string m_Path;
		std::ifstream m_Stream;
	};
}
