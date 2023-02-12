#pragma once

#include <string>

namespace V2MPAsm
{
	class LabelReference
	{
	public:
		enum class ReferenceType
		{
			UPPER_BYTE_OF_ADDRESS,
			LOWER_BYTE_OF_ADDRESS,
			SIGNED_NUM_WORDS_DIST_TO_LABEL,
			UNSIGNED_NUM_WORDS_DIST_TO_LABEL
		};

		LabelReference() = default;
		LabelReference(ReferenceType refType, const std::string labelName);

		ReferenceType GetReferenceType() const;
		void SetReferenceType(ReferenceType type);

		std::string GetLabelName() const;
		void SetLabelName(const std::string& name);

	private:
		ReferenceType m_Type = ReferenceType::UPPER_BYTE_OF_ADDRESS;
		std::string m_LabelName;
	};
}
