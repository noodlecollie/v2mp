#pragma once

#include <memory>
#include <vector>
#include "Files/InputFile.h"

namespace V2MPAsm
{
	class InputReader
	{
	public:
		explicit InputReader(const std::shared_ptr<InputFile>& file);

		size_t GetPosition() const;
		const char* GetDataAtPosition() const;
		bool IsEOF() const;

		char ReadChar();

	private:
		std::shared_ptr<InputFile> m_File;
		std::vector<char> m_Data;
		size_t m_Position = 0;
	};
}
