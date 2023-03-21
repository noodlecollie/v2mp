#pragma once

#include <fstream>
#include <string>

namespace LibToolchainComponents
{
	class OutputFile
	{
	public:
		explicit OutputFile(const std::string& path);

		std::string GetPath() const;
		std::ofstream& GetStream();

	private:
		std::string m_Path;
		std::ofstream m_Stream;
	};
}
