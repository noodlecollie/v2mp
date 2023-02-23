#include <memory>
#include "Assembler/Assembler.h"
#include "LibV2MPAsm/Assembler.h"
#include "Interface_Assembler.h"

static V2MPAsm_AssemblerResult ToExternalResult(V2MPAsm::Assembler::Result inResult)
{
	switch ( inResult )
	{
		case V2MPAsm::Assembler::Result::COMPLETED:
		{
			return V2MPASM_COMPLETED_OK;
		}

		case V2MPAsm::Assembler::Result::COMPLETED_WITH_WARNINGS:
		{
			return V2MPASM_COMPLETED_WITH_WARNINGS;
		}

		default:
		{
			return V2MPASM_FAILED;
		}
	}
}

extern "C"
{
	API_LIBV2MPASM struct V2MPAsm_Assembler* V2MPAsm_Assembler_CreateFromFiles(const char* inputFilePath, const char* outputFilePath)
	{
		try
		{
			std::unique_ptr<struct V2MPAsm_Assembler> assembler = std::make_unique<struct V2MPAsm_Assembler>();

			assembler->inner.SetInputFileName(inputFilePath ? inputFilePath : "");
			assembler->inner.SetOutputToFile(outputFilePath ? outputFilePath : "");

			return assembler.release();
		}
		catch (...)
		{
			return nullptr;
		}
	}

	API_LIBV2MPASM struct V2MPAsm_Assembler* V2MPAsm_Assembler_CreateFromMemory(const char* inputFileName, const char* inputBuffer)
	{
		try
		{
			std::unique_ptr<struct V2MPAsm_Assembler> assembler = std::make_unique<struct V2MPAsm_Assembler>();

			assembler->inner.SetInputRawData(inputFileName ? inputFileName : "", inputBuffer);
			assembler->inner.SetOutputToRawData();

			return assembler.release();
		}
		catch (...)
		{
			return nullptr;
		}
	}

	API_LIBV2MPASM void V2MPAsm_Assembler_Destroy(struct V2MPAsm_Assembler* assembler)
	{
		if ( !assembler )
		{
			return;
		}

		delete assembler;
	}

	API_LIBV2MPASM V2MPAsm_AssemblerResult V2MPAsm_Assembler_Run(struct V2MPAsm_Assembler* assembler)
	{
		if ( !assembler )
		{
			return V2MPASM_FAILED;
		}

		try
		{
			return ToExternalResult(assembler->inner.Run());
		}
		catch (...)
		{
			return V2MPASM_FAILED;
		}
	}

	API_LIBV2MPASM size_t V2MPAsm_Assembler_GetExceptionCount(const struct V2MPAsm_Assembler* assembler)
	{
		if ( !assembler )
		{
			return 0;
		}

		return assembler->inner.GetExceptions().size();
	}

	API_LIBV2MPASM const struct V2MPAsm_Exception* V2MPAsm_Assembler_GetException(const struct V2MPAsm_Assembler* assembler, size_t index)
	{
		if ( !assembler )
		{
			return nullptr;
		}

		const V2MPAsm::ExceptionList& list = assembler->inner.GetExceptions();

		if ( index >= list.size() )
		{
			return nullptr;
		}

		return list[index].get();
	}

	API_LIBV2MPASM bool V2MPAsm_Assembler_HasInMemoryOutputBuffer(const struct V2MPAsm_Assembler* assembler)
	{
		return assembler ? assembler->inner.OutputIsRawData() : false;
	}

	API_LIBV2MPASM size_t V2MPAsm_Assembler_InMemoryOutputBufferSizeInBytes(const struct V2MPAsm_Assembler* assembler)
	{
		return (assembler && assembler->inner.OutputIsRawData()) ? (assembler->inner.RawOutputSizeInWords() * sizeof(uint16_t)) : 0;
	}

	API_LIBV2MPASM size_t V2MPAsm_Assembler_TakeInMemoryOutputBuffer(struct V2MPAsm_Assembler* assembler, void* outBuffer, size_t outBufferSizeInBytes)
	{
		if ( !assembler || !assembler->inner.OutputIsRawData() )
		{
			return 0;
		}

		const std::vector<uint16_t> rawOutputFromAssembler = assembler->inner.TakeRawOutput();
		size_t bytesWritten = 0;

		if ( outBuffer && outBufferSizeInBytes >= sizeof(uint16_t) )
		{
			const size_t outBufferSizeInWords = std::min<size_t>(rawOutputFromAssembler.size(), outBufferSizeInBytes / sizeof(uint16_t));
			uint16_t* outPtr = static_cast<uint16_t*>(outBuffer);

			for ( size_t index = 0; index < outBufferSizeInWords; ++index )
			{
				outPtr[index] = rawOutputFromAssembler[index];
			}

			bytesWritten = outBufferSizeInWords * sizeof(uint16_t);
		}

		return bytesWritten;
	}
} // extern "C"
