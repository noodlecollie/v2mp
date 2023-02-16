#include <iostream>
#include "LibV2MPLink/Dummy.h"

int main(int /* argc */, char** /* argv */)
{
	std::cout << "Return value: " << V2MPLink_Dummy() << std::endl;
	return 0;
}
