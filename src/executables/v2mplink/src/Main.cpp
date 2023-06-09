#include <iostream>
#include "LibV2MPLink/Linker.h"
#include "LibV2MPLink/Exception.h"
#include "V2MPLinkArgumentParser.h"

enum ReturnCode
{
	RETURN_OK = 0,
	RETURN_ENCOUNTERED_WARNINGS,
	RETURN_ENCOUNTERED_ERRORS,
	RETURN_UNEXPECTED_ERROR = -1,
};

static ReturnCode GenerateOutput(const std::string& inputFile, const std::string& outputFile)
{
	V2MPLink_Linker* linker = V2MPLink_Linker_Create(inputFile.c_str(), outputFile.c_str());

	if ( !linker )
	{
		throw std::runtime_error("Failed to create assembler instance.");
	}

	ReturnCode returnValue = RETURN_UNEXPECTED_ERROR;

	do
	{
		const V2MPLink_LinkerResult result = V2MPLink_Linker_Run(linker);

		if ( result == V2MPLINK_COMPLETED_OK )
		{
			// All good.
			returnValue = RETURN_OK;
			break;
		}

		returnValue = RETURN_ENCOUNTERED_WARNINGS;
		const size_t numExceptions = V2MPLink_Linker_GetExceptionCount(linker);

		for ( size_t index = 0; index < numExceptions; ++index )
		{
			const V2MPLink_Exception* exception = V2MPLink_Linker_GetException(linker, index);

			if ( V2MPLink_Exception_GetType(exception) == V2MPLINK_EXCEPTION_ERROR )
			{
				returnValue = RETURN_ENCOUNTERED_ERRORS;
			}

			char errorString[512];
			errorString[0] = '\0';

			V2MPLink_Exception_ToString(exception, errorString, sizeof(errorString));
			std::cerr << errorString << std::endl;
		}
	}
	while ( false );

	V2MPLink_Linker_Destroy(linker);
	return returnValue;
}

int main(int argc, char** argv)
{
	V2MPLinkArgumentParser parser;

	try
	{
		parser.Parse(argc, argv);
	}
	catch ( const std::runtime_error& ex )
	{
		std::cerr << ex.what() << std::endl;
		parser.PrintHelp(std::cerr);
		std::cerr << std::endl;

		return RETURN_UNEXPECTED_ERROR;
	}
	catch ( ... )
	{
		std::cerr << "Encountered an unhandled exception." << std::endl;
		return RETURN_UNEXPECTED_ERROR;
	}

	try
	{
		return GenerateOutput(parser.GetInputFile(), parser.GetOutputFile());
	}
	catch ( const std::runtime_error& ex )
	{
		std::cerr << "ERROR: " << ex.what() << std::endl;
		return RETURN_UNEXPECTED_ERROR;
	}
	catch ( ... )
	{
		std::cerr << "Encountered an unhandled exception." << std::endl;
		return RETURN_UNEXPECTED_ERROR;
	}
}
