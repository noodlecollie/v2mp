#include <memory>
#include "LibV2MPLink/Linker.h"
#include "Interface_Linker.h"
#include "Interface_Exception.h"

LIBV2MPLINK_PUBLIC(V2MPLink_Linker*) V2MPLink_Linker_Create(const char* objectFilePath, const char* outputFilePath)
{
	try
	{
		std::unique_ptr<struct V2MPLink_Linker> linker = std::make_unique<struct V2MPLink_Linker>();

		linker->inner.SetObjectFilePath(objectFilePath ? objectFilePath : "");
		linker->inner.SetOutputPath(outputFilePath ? outputFilePath : "");

		return linker.release();
	}
	catch (...)
	{
		return nullptr;
	}
}

LIBV2MPLINK_PUBLIC(void) V2MPLink_Linker_Destroy(V2MPLink_Linker* linker)
{
	if ( !linker )
	{
		return;
	}

	delete linker;
}

LIBV2MPLINK_PUBLIC(V2MPLink_LinkerResult) V2MPLink_Linker_Run(struct V2MPLink_Linker* linker)
{
	if ( !linker )
	{
		return V2MPLINK_FAILED;
	}

	return linker->inner.Run();
}

LIBV2MPLINK_PUBLIC(size_t) V2MPLink_Linker_GetExceptionCount(const struct V2MPLink_Linker* linker)
{
	if ( !linker )
	{
		return 0;
	}

	return linker->inner.GetExceptions().size();
}

LIBV2MPLINK_PUBLIC(const struct V2MPLink_Exception*) V2MPLink_Linker_GetException(const struct V2MPLink_Linker* linker, size_t index)
{
	if ( !linker )
	{
		return nullptr;
	}

	const V2MPLink::ExceptionList& list = linker->inner.GetExceptions();

	if ( index >= list.size() )
	{
		return nullptr;
	}

	return list[index].get();
}
