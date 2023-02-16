#include <iostream>
#include "V2MPAsmArgumentParser.h"
#include "LibV2MPAsm/Assembler.h"
#include "LibV2MPAsm/Exception.h"

enum ReturnCode
{
	RETURN_OK = 0,
	RETURN_ENCOUNTERED_WARNINGS,
	RETURN_ENCOUNTERED_ERRORS,
	RETURN_UNEXPECTED_ERROR = -1,
};

static ReturnCode GenerateObjectFile(const std::string& inputFile, const std::string& outputFile)
{
	V2MPAsm_Assembler* assembler = V2MPAsm_Assembler_CreateFromFiles(inputFile.c_str(), outputFile.c_str());

	if ( !assembler )
	{
		throw std::runtime_error("Failed to create assembler instance.");
	}

	ReturnCode returnValue = RETURN_UNEXPECTED_ERROR;

	do
	{
		const V2MPAsm_AssemblerResult result = V2MPAsm_Assembler_Run(assembler);

		if ( result == V2MPASM_COMPLETED_OK )
		{
			// All good.
			returnValue = RETURN_OK;
			break;
		}

		returnValue = RETURN_ENCOUNTERED_WARNINGS;
		const size_t numExceptions = V2MPAsm_Assembler_GetExceptionCount(assembler);

		for ( size_t index = 0; index < numExceptions; ++index )
		{
			const V2MPAsm_Exception* exception = V2MPAsm_Assembler_GetException(assembler, index);

			if ( V2MPAsm_Exception_GetType(exception) == V2MPASM_EXCEPTION_ERROR )
			{
				returnValue = RETURN_ENCOUNTERED_ERRORS;
			}

			char errorString[512];
			errorString[0] = '\0';

			V2MPAsm_Exception_ToString(exception, errorString, sizeof(errorString));
			std::cerr << errorString << std::endl;
		}
	}
	while ( false );

	V2MPAsm_Assembler_Destroy(assembler);
	return returnValue;
}

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

		return RETURN_UNEXPECTED_ERROR;
	}
	catch ( ... )
	{
		std::cerr << "Encountered an unhandled exception." << std::endl;
		return RETURN_UNEXPECTED_ERROR;
	}

	try
	{
		return GenerateObjectFile(parser.GetInputFile(), parser.GetOutputFile());
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
