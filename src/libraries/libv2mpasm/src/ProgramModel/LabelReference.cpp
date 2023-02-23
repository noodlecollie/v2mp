#include "ProgramModel/LabelReference.h"

namespace V2MPAsm
{
	LabelReference::LabelReference(ReferenceType refType, const std::string labelName) :
		m_Type(refType),
		m_LabelName(labelName)
	{
	}

	LabelReference::ReferenceType LabelReference::GetReferenceType() const
	{
		return m_Type;
	}

	void LabelReference::SetReferenceType(ReferenceType type)
	{
		m_Type = type;
	}

	std::string LabelReference::GetLabelName() const
	{
		return m_LabelName;
	}

	void LabelReference::SetLabelName(const std::string& name)
	{
		m_LabelName = name;
	}
}
