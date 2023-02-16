#pragma once

#include <ostream>
#include <memory>

class V2MPAsmArgumentParser
{
public:
	V2MPAsmArgumentParser() noexcept;
	~V2MPAsmArgumentParser() noexcept;

	void PrintHelp(std::ostream& stream) const;
	void Parse(int argc, char** argv);

	std::string GetInputFile() const;
	std::string GetOutputFile() const;

private:
	class Impl;

	std::unique_ptr<Impl> m_Impl;
};
