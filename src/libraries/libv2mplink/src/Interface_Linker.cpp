#include <memory>
#include "LibV2MPLink/Linker.h"
#include "Interface_Linker.h"

extern "C"
{
	API_LIBV2MPLINK V2MPLink_Linker* V2MPLink_Linker_Create(const char* objectFilePath, const char* outputFilePath)
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

	API_LIBV2MPLINK void V2MPLink_Linker_Destroy(V2MPLink_Linker* linker)
	{
		if ( !linker )
		{
			return;
		}

		delete linker;
	}
} // extern "C"
