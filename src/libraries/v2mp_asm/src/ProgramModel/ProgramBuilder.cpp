#include <stdexcept>
#include "ProgramModel/ProgramBuilder.h"
#include "ProgramModel/ProgramModel.h"

namespace V2MPAsm
{
	ProgramBuilder::ProgramBuilder(const std::shared_ptr<ProgramModel>& model)
	{
		if ( !model )
		{
			throw std::runtime_error("No program model was provided");
		}
	}

	ProgramBuilder::~ProgramBuilder()
	{
	}
}
