#include <iostream>

#include <nlohmann/json.hpp>


int Bar()
{
	static int i = 0;
	static int v = i + 10;
	return v;
}

int main()
{
	std::cout << Bar() << std::endl;
	std::cout << Bar() << std::endl;
	std::cout << Bar() << std::endl;
	std::cout << Bar() << std::endl;
	std::cout << Bar() << std::endl;
	std::cout << Bar() << std::endl;
	std::cout << Bar() << std::endl;
	return 0;
}
