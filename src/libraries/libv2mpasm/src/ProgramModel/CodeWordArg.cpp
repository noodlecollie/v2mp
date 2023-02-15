#include "ProgramModel/CodeWordArg.h"

namespace V2MPAsm
{
	CodeWordArg::CodeWordArg(size_t column, int32_t value) :
		m_Column(column),
		m_Value(value)
	{
	}

	CodeWordArg::CodeWordArg(size_t column, LabelReference::ReferenceType refType, const std::string labelName) :
		m_Column(column),
		m_LabelReference(LabelReference(refType, labelName))
	{
	}

	size_t CodeWordArg::GetColumn() const
	{
		return m_Column;
	}

	bool CodeWordArg::IsNumber() const
	{
		return !IsLabelReference();
	}

	bool CodeWordArg::IsLabelReference() const
	{
		return m_LabelReference.has_value();
	}

	int32_t CodeWordArg::GetValue() const
	{
		return m_Value;
	}

	void CodeWordArg::SetValue(int32_t value)
	{
		m_Value = value;
	}

	LabelReference CodeWordArg::GetLabelReference() const
	{
		return IsLabelReference() ? m_LabelReference.value() : LabelReference();
	}
}
