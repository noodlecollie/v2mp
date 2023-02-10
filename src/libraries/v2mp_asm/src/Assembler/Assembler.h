#pragma once

#include <memory>
#include <vector>
#include <fstream>
#include <variant>
#include "Interface_Exception.h"

namespace V2MPAsm
{
	class OutputFile;
	class ProgramModel;

	class Assembler
	{
	public:
		enum class Result
		{
			COMPLETED = 0,
			COMPLETED_WITH_WARNINGS,
			FAILED
		};

		void SetInputFileName(const std::string& inFile) noexcept;
		void SetInputRawData(const std::string& path, const char* data) noexcept;

		void SetOutputToFile(const std::string& outFile) noexcept;
		void SetOutputToRawData() noexcept;

		Result Run() noexcept;

		const ExceptionList& GetExceptions() const noexcept;

		bool InputIsRawData() const noexcept;
		bool OutputIsRawData() const noexcept;
		std::vector<uint16_t> TakeRawOutput() noexcept;
		size_t RawOutputSizeInWords() const noexcept;

	private:
		struct InputRawData
		{
			std::string path;
			std::vector<char> data;
		};

		using InputVariant = std::variant<std::string, InputRawData>;
		using OutputVariant = std::variant<std::string, std::vector<uint16_t>>;

		static std::vector<char> CopyInput(const char* input);

		std::string GetInputPath() const;
		ExceptionList ParseInputAndCompileExceptionList(std::unique_ptr<ProgramModel>& model) noexcept;
		ExceptionList ParseInput(std::unique_ptr<ProgramModel>& model);

		ExceptionList TryWriteOutputFile(std::unique_ptr<ProgramModel> model);
		ExceptionList TryWriteRawOutput(std::unique_ptr<ProgramModel> model);
		void WriteOutput(const std::unique_ptr<ProgramModel>& model, const std::string& outPath, std::ostream& outStream);
		void WriteOutput(const std::unique_ptr<ProgramModel>& model, std::vector<uint16_t>& outVec);

		ExceptionList m_ExceptionList;

		InputVariant m_Input = std::string();
		OutputVariant m_Output = std::string();
	};
}
