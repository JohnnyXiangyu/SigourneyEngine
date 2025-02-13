#include <iostream>

#include <nlohmann/json.hpp>

int main()
{
	auto jsonObj = nlohmann::json::parse("{ \"key\": 2000 }");

	auto pair = jsonObj.find("key");
	
	std::cout << pair->get<float>() << std::endl;

	return 0;
}