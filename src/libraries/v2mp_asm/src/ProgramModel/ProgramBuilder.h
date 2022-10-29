#pragma once

#include <memory>

namespace V2MPAsm
{
	class ProgramModel;

	class ProgramBuilder
	{
	public:
		ProgramBuilder(const std::shared_ptr<ProgramModel>& model);
		~ProgramBuilder();

	private:
		std::shared_ptr<ProgramModel> m_ProgramModel;
	};
}
