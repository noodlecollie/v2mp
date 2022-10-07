#include <memory>
#include "Assembler/Assembler.h"
#include "V2MPAsm/Assembler.h"
#include "Interface_Assembler.h"

#ifdef __cplusplus
extern "C" {
#endif

API_V2MPASM struct V2MPAsm_Assembler* V2MPAsm_Assembler_Create(const char* /* inputFile */, const char* /* outputFile */)
{
	try
	{
		std::unique_ptr<struct V2MPAsm_Assembler> assembler = std::make_unique<struct V2MPAsm_Assembler>();

		// TODO: Provide inputs and outputs

		return assembler.release();
	}
	catch (...)
	{
		return nullptr;
	}
}

API_V2MPASM void V2MPAsm_Assembler_Destroy(struct V2MPAsm_Assembler* assembler)
{
	if ( !assembler )
	{
		return;
	}

	delete assembler;
}

API_V2MPASM bool V2MPAsm_Assembler_Run(struct V2MPAsm_Assembler* assembler)
{
	if ( !assembler )
	{
		return false;
	}

	return assembler->inner.Run();
}

API_V2MPASM size_t V2MPAsm_Assembler_GetExceptionCount(const struct V2MPAsm_Assembler* assembler)
{
	if ( !assembler )
	{
		return 0;
	}

	return assembler->inner.GetExceptions().size();
}

API_V2MPASM const struct V2MPAsm_Exception* V2MPAsm_Assembler_GetException(const struct V2MPAsm_Assembler* assembler, size_t index)
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

#ifdef __cplusplus
} // extern "C"
#endif
