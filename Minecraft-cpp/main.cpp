#include <iostream>

#include "mymc.h"

int main()
{
	auto& core = mymc::Core::GetInstance();
	if (!core.init(800, 600, "mc-cpp"))
	{
		std::cerr << "核心初始化失败" << std::endl;
		return -1;
	}
	core.run();
}