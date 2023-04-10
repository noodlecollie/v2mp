#pragma once

#include <vector>
#include <string>
#include "Interface_Exception.h"
#include "LibV2MPLink/Linker.h"

namespace LibToolchainComponents
{
	class FilePool;
	class InputFile;
	class OutputFile;
}

namespace V2MPLink
{
	class Linker
	{
	public:
		std::string GetObjectFilePath() const;
		void SetObjectFilePath(const std::string& filePath);

		std::string GetOutputPath() const;
		void SetOutputPath(const std::string& outputPath);

		const ExceptionList& GetExceptions() const noexcept;

		V2MPLinker_LinkerResult Run() noexcept;

	private:
		using InputFile = LibToolchainComponents::InputFile;
		using OutputFile = LibToolchainComponents::OutputFile;
		using FilePool = LibToolchainComponents::FilePool;

		ExceptionList RunLinkProcess();
		std::shared_ptr<InputFile> TryOpenInputFile(FilePool& filePool, const std::string& path);
		std::shared_ptr<OutputFile> TryOpenOutputFile(const std::string& path);

		ExceptionList m_ExceptionList;

		// TODO: We've not yet defined how to link multiple object files together.
		// For now, we just treat a single object file as the code.
		std::string m_ObjectFilePath;
		std::string m_OutputPath;
	};
}
