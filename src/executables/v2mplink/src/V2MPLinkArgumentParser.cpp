#include <string>
#include <set>
#include "V2MPLinkArgumentParser.h"
#include "argparse/argparse.hpp"
#include "Version.gen.h"
#include "LibV2MPLink/Version.h"
#include "CommandLineArgs.h"

class V2MPLinkArgumentParser::Impl
{
public:
	Impl() :
		m_Parser("V2MPLink", VERSION_STRING)
	{
		m_Parser.add_description(
			"Frontend " + std::string(VERSION_STRING) +
			", Lib: " + std::string(V2MPLink_Version_GetVersionString())
		);

		AddCommandLineArgs();
	}

	argparse::ArgumentParser& GetParser()
	{
		return m_Parser;
	}

	const argparse::ArgumentParser& GetParser() const
	{
		return m_Parser;
	}

	void Parse(int argc, char** argv)
	{
		m_Parser.parse_known_args(argc, argv);
		ValidateArgs();
	}

	void PrintHelp(std::ostream& stream)
	{
		stream << m_Parser;
	}

private:
	void AddCommandLineArgs()
	{
		// TODO: Update these once the linker becomes more complex
		m_Parser
			.add_argument(CmdArgs::INPUT_FILE)
			.help("Input object file.");

		m_Parser
			.add_argument(CmdArgs::OUTPUT_FILE_SHORT, CmdArgs::OUTPUT_FILE)
			.help("Output executable file to create.")
			.required();
	}

	void ValidateArgs()
	{
		if ( m_Parser.get<std::string>(CmdArgs::INPUT_FILE).empty() )
		{
			throw std::runtime_error("No input file specified.");
		}

		if ( m_Parser.get<std::string>(CmdArgs::OUTPUT_FILE).empty() )
		{
			throw std::runtime_error("No output file specified.");
		}
	}

	argparse::ArgumentParser m_Parser;
};

V2MPLinkArgumentParser::V2MPLinkArgumentParser() noexcept :
	m_Impl(std::make_unique<Impl>())
{
}

V2MPLinkArgumentParser::~V2MPLinkArgumentParser() noexcept
{
}

void V2MPLinkArgumentParser::PrintHelp(std::ostream& stream) const
{
	m_Impl->PrintHelp(stream);
}

void V2MPLinkArgumentParser::Parse(int argc, char** argv)
{
	m_Impl->Parse(argc, argv);
}

std::string V2MPLinkArgumentParser::GetInputFile() const
{
	return m_Impl->GetParser().get<std::string>(CmdArgs::INPUT_FILE);
}

std::string V2MPLinkArgumentParser::GetOutputFile() const
{
	return m_Impl->GetParser().get<std::string>(CmdArgs::OUTPUT_FILE);
}
