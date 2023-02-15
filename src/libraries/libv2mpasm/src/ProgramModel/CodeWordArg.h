#pragma once

#include <cstdint>
#include <optional>
#include "ProgramModel/LabelReference.h"
#include "Utils/ParsingUtils.h"

namespace V2MPAsm
{
	class CodeWordArg
	{
	public:
		CodeWordArg(size_t column, int32_t value);
		CodeWordArg(size_t column, LabelReference::ReferenceType refType, const std::string labelName);

		size_t GetColumn() const;

		bool IsNumber() const;
		bool IsLabelReference() const;

		int32_t GetValue() const;
		void SetValue(int32_t value);
		LabelReference GetLabelReference() const;

	private:
		size_t m_Column = COLUMN_NUMBER_BASE;
		int32_t m_Value = 0;
		std::optional<LabelReference> m_LabelReference;
	};
}
