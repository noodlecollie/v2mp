#pragma once

#include <vector>
#include "V2MPAsm/Assembler.h"
#include "TestUtil/Assembly.h"

void CheckProgramMatches(V2MPAsm_Assembler* assembler, const std::vector<V2MP_Word>& expected);
