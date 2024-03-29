#pragma once

#include <vector>
#include "LibV2MPAsm/Assembler.h"
#include "TestUtil/Assembly.h"

void CheckProgramMatches(V2MPAsm_Assembler* assembler, const std::vector<Asm::Word>& expected);
