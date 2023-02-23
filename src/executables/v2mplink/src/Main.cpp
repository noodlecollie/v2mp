#include <iostream>
#include "LibV2MPLink/Version.h"

int main(int /* argc */, char** /* argv */)
{
	std::cout << V2MPLink_Version_GetVersionString() << std::endl;
	return 0;
}
