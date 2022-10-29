#pragma once

#include <cstdint>
#include <vector>
#include <variant>
#include "ProgramModel/InstructionMeta.h"
#include "ProgramModel/LabelReference.h"

namespace V2MPAsm
{
	class CodeWordArg : public std::variant<int32_t, LabelReference>
	{
	public:
		explicit CodeWordArg(int32_t value) :
			VariantType(value)
		{
		}

		CodeWordArg(LabelReference::ReferenceType refType, const std::string labelName) :
			VariantType(LabelReference(refType, labelName))
		{
		}

	private:
		using VariantType = std::variant<int32_t, LabelReference>;
	};

	class CodeWord
	{
	public:
		explicit CodeWord(InstructionType instructionType = InstructionType::NOP);
		CodeWord(const CodeWord& other);
		CodeWord(CodeWord&& other);

		CodeWord& operator =(const CodeWord& other);
		CodeWord& operator =(CodeWord&& other);

		InstructionType GetInstructionType() const;
		size_t GetArgumentCount() const;

		void AddArgument(int32_t value);
		void AddArgument(LabelReference::ReferenceType refType, const std::string& labelName);

	private:
		InstructionType m_InstructionType = InstructionType::NOP;
		std::vector<CodeWordArg> m_Arguments;
	};
}
