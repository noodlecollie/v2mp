#pragma once

#include <memory>
#include <vector>
#include <functional>
#include "Files/InputFile.h"
#include "Utils/ParsingUtils.h"

namespace V2MPAsm
{
	class InputReader
	{
	public:
		explicit InputReader(const std::shared_ptr<InputFile>& file);
		InputReader(const std::string& path, std::vector<char>&& rawData);

		std::string GetPath() const;
		size_t GetCurrentPosition() const;
		size_t GetCurrentLine() const;
		size_t GetCurrentColumn();
		size_t GetDataSize() const;
		bool IsEOF() const;

		// Advances line/column.
		char ReadChar();
		void SkipChars(size_t maxCharsToSkip);
		void SkipForAnyOf(const std::string& charsToSkip);
		void SkipForAnyOf(const std::function<bool(char)>& charIsSkippableFunc);
		void SkipUntilAnyOf(const std::string& charsToSkip, bool advanceOnceReached = false);
		void SkipUntilAnyOf(const std::function<bool(char)>& charIsNotSkippableFunc, bool advanceOnceReached = false);

		// Does not advance line/column.
		const char* GetDataAtCurrentPosition() const;
		const char* GetDataAtAbsolutePosition(size_t position) const;
		char PeekChar(size_t offset = 0) const;

		void ResetPosition();

	private:
		char AdvanceLineAndColumn();

		std::string m_Path;
		std::vector<char> m_Data;
		size_t m_Position = 0;
		size_t m_Line = LINE_NUMBER_BASE;
		size_t m_Column = COLUMN_NUMBER_BASE;
	};
}
