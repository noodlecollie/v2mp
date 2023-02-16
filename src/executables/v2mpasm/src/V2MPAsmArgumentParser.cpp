#include <string>
#include <set>
#include "V2MPAsmArgumentParser.h"
#include "argparse/argparse.hpp"
#include "CommandLineArgs.h"

static std::string RemovePrefix(const std::string& str, const char* prefix)
{
	return str.substr(std::string(prefix).length());
}

class V2MPAsmArgumentParser::Impl
{
public:
	Impl() :
		m_Parser("V2MPAsm")
	{
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
		const std::vector<std::string> argsNotParsed = m_Parser.parse_known_args(argc, argv);
		ManuallyParseRemainingArgs(argsNotParsed);
		ValidateArgs();
	}

	void PrintHelp(std::ostream& stream)
	{
		stream << m_Parser;
	}

private:
	void AddCommandLineArgs()
	{
		m_Parser
			.add_argument(CmdArgs::INPUT_FILE)
			.help("Assembly source file to parse.");

		m_Parser
			.add_argument(CmdArgs::OUTPUT_FILE_SHORT, CmdArgs::OUTPUT_FILE)
			.help("Output object file to write.")
			.required();
	}

	void ManuallyParseRemainingArgs(const std::vector<std::string>& args)
	{
		for ( const std::string& arg : args )
		{
			if ( arg.find(CmdArgs::PREFIX_WARNING_MODIFIER) == 0 )
			{
				HandleWarningSwitch(RemovePrefix(arg, CmdArgs::PREFIX_WARNING_MODIFIER));
			}
			else
			{
				throw std::runtime_error("Unrecognised argument: " + arg);
			}
		}
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

	void HandleWarningSwitch(const std::string& arg)
	{
		if ( arg.find(CmdArgs::WMOD_DISABLE) == 0 )
		{
			// -Wno-...
			throw std::runtime_error("TODO: Disabling warnings is not yet implemented");
		}
		else if ( arg.find(CmdArgs::WMOD_MAKE_ERROR) == 0 )
		{
			// -Werror=...
			throw std::runtime_error("TODO: Converting warnings to errors is not yet implemented");
		}
		else if ( arg == CmdArgs::WMOD_TREAT_WARNINGS_AS_ERRORS )
		{
			// -Werror
			throw std::runtime_error("TODO: Treating warnings as errors is not yet implemented");
		}
		else
		{
			throw std::runtime_error("Unknown warning modifier: " + std::string(CmdArgs::PREFIX_WARNING_MODIFIER) + arg);
		}
	}

	argparse::ArgumentParser m_Parser;
};

V2MPAsmArgumentParser::V2MPAsmArgumentParser() noexcept :
	m_Impl(std::make_unique<Impl>())
{
}

V2MPAsmArgumentParser::~V2MPAsmArgumentParser() noexcept
{
}

void V2MPAsmArgumentParser::PrintHelp(std::ostream& stream) const
{
	m_Impl->PrintHelp(stream);
}

void V2MPAsmArgumentParser::Parse(int argc, char** argv)
{
	m_Impl->Parse(argc, argv);
}

std::string V2MPAsmArgumentParser::GetInputFile() const
{
	return m_Impl->GetParser().get<std::string>(CmdArgs::INPUT_FILE);
}

std::string V2MPAsmArgumentParser::GetOutputFile() const
{
	return m_Impl->GetParser().get<std::string>(CmdArgs::OUTPUT_FILE);
}
