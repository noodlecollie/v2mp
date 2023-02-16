#include <iostream>
#include "V2MPAsmArgumentParser.h"

int main(int argc, char** argv)
{
	V2MPAsmArgumentParser parser;

	try
	{
		parser.Parse(argc, argv);
	}
	catch ( const std::runtime_error& ex )
	{
		std::cerr << ex.what() << std::endl;
		parser.PrintHelp(std::cerr);
		std::cerr << std::endl;

		return 1;
	}
	catch ( ... )
	{
		std::cerr << "Encountered an unhandled exception." << std::endl;
		return 0;
	}

	std::cout << "Input file: " << parser.GetInputFile() << std::endl;
	std::cout << "Output file: " << parser.GetOutputFile() << std::endl;

	return 0;
}
