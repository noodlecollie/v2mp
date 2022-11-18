#pragma once

#include <cstdint>
#include <vector>
#include <variant>
#include "ProgramModel/InstructionMeta.h"
#include "ProgramModel/LabelReference.h"
#include "Utils/ParsingUtils.h"

namespace V2MPAsm
{
	class CodeWordArg
	{
	public:
		explicit CodeWordArg(size_t column, int32_t value) :
			m_Column(column),
			m_Value(value)
		{
		}

		CodeWordArg(size_t column, LabelReference::ReferenceType refType, const std::string labelName) :
			m_Column(column),
			m_Value(LabelReference(refType, labelName))
		{
		}

		size_t GetColumn() const
		{
			return m_Column;
		}

		bool IsNumber() const
		{
			return std::holds_alternative<int32_t>(m_Value);
		}

		bool IsLabelReference() const
		{
			return std::holds_alternative<LabelReference>(m_Value);
		}

		int32_t GetNumber() const
		{
			return IsNumber() ? std::get<int32_t>(m_Value) : 0;
		}

		LabelReference GetLabelReference() const
		{
			return IsLabelReference() ? std::get<LabelReference>(m_Value) : LabelReference();
		}

		void SetNumber(int32_t value)
		{
			m_Value = value;
		}

	private:
		using VariantType = std::variant<int32_t, LabelReference>;

		size_t m_Column = COLUMN_NUMBER_BASE;
		VariantType m_Value = 0;
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
		size_t GetLine() const;
		size_t GetColumn() const;

		size_t GetArgumentCount() const;

		uint16_t GetAddress() const;
		void SetAddress(uint16_t address);

		void AddArgument(size_t columnIndex, int32_t value);
		void AddArgument(size_t columnIndex, LabelReference::ReferenceType refType, const std::string& labelName);

		CodeWordArg* GetArgument(size_t argIndex);
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
