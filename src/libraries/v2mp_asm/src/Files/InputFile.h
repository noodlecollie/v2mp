#pragma once

#include <fstream>
#include <string>

namespace V2MPAsm
{
	class InputFile
	{
	public:
		explicit InputFile(const std::string& path);

		std::string GetPath() const;
		std::ifstream& GetStream();

	private:
		std::string m_Path;
		std::ifstream m_Stream;
	};
}
