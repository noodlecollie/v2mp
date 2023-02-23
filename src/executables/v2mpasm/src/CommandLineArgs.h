#pragma once

#include <string>

namespace CmdArgs
{
	static constexpr const char* const INPUT_FILE = "input_file";
	static constexpr const char* const OUTPUT_FILE = "--output_file";
	static constexpr const char* const OUTPUT_FILE_SHORT = "-o";

	static constexpr const char* const PREFIX_WARNING_MODIFIER("-W");
	static constexpr const char* const WMOD_DISABLE("no-");
	static constexpr const char* const WMOD_MAKE_ERROR("error=");
	static constexpr const char* const WMOD_TREAT_WARNINGS_AS_ERRORS("error");
};
