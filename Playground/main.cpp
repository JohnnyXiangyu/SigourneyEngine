#include <iostream>

#include <nlohmann/json.hpp>

struct Person
{
	std::string Name;
	std::string Address;
	unsigned int Age;
};

void to_json(nlohmann::json& j, const Person& p)
{
    j = nlohmann::json{ {"Name", p.Name}, {"Address", p.Address}, {"Age", p.Age} };
}

void from_json(const nlohmann::json& j, Person& p)
{
    j.at("Name").get_to(p.Name);
    j.at("Address").get_to(p.Address);
    j.at("Age").get_to(p.Age);
}

int main()
{
	try
	{
		nlohmann::json j = nlohmann::json::parse("{\"Name\": 100, \"Address\": \"2\", \"Age\": 3, \"Misc\": 111}");
		Person p = j.template get<Person>();
		std::cout << p.Name << std::endl;
	}
	catch (std::exception& e)
	{
		std::cerr << "failed deserialziation: " << e.what() << std::endl;
	}

	return 0;
}