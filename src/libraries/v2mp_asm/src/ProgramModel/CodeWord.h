#pragma once

#include <cstdint>
#include <vector>
#include <variant>
#include "ProgramModel/InstructionMeta.h"
#include "ProgramModel/LabelReference.h"
#include "Utils/ParsingUtils.h"

namespace V2MPAsm
{
	class CodeWordArg : public std::variant<int32_t, LabelReference>
	{
	public:
		explicit CodeWordArg(size_t column, int32_t value) :
			VariantType(value),
			m_Column(column)
		{
		}

		CodeWordArg(size_t column, LabelReference::ReferenceType refType, const std::string labelName) :
			VariantType(LabelReference(refType, labelName)),
			m_Column(column)
		{
		}

		size_t GetColumn() const
		{
			return m_Column;
		}

	private:
		using VariantType = std::variant<int32_t, LabelReference>;

		size_t m_Column = COLUMN_NUMBER_BASE;
	};

	class CodeWord
	{
	public:
		CodeWord(
			InstructionType instructionType = InstructionType::NOP,
			size_t line = LINE_NUMBER_BASE,
			size_t column = COLUMN_NUMBER_BASE
		);

		CodeWord(const CodeWord& other);
		CodeWord(CodeWord&& other);

		CodeWord& operator =(const CodeWord& other);
		CodeWord& operator =(CodeWord&& other);

		InstructionType GetInstructionType() const;
		size_t GetArgumentCount() const;

		uint16_t GetAddress() const;
		void SetAddress(uint16_t address);

		void AddArgument(size_t columnIndex, int32_t value);
		void AddArgument(size_t columnIndex, LabelReference::ReferenceType refType, const std::string& labelName);

		const CodeWordArg* GetArgument(size_t argIndex) const;
		size_t GetArgumentColumn(size_t argIndex) const;

	private:
		uint16_t m_Address = 0;
		size_t m_Line = LINE_NUMBER_BASE;
		size_t m_Column = COLUMN_NUMBER_BASE;
		InstructionType m_InstructionType = InstructionType::NOP;
		std::vector<CodeWordArg> m_Arguments;
	};
}
