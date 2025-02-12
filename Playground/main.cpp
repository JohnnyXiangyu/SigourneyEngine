#include <iostream>

#include <nlohmann/json.hpp>

#define SCRIPTABLE_PROPERTY(propertyType, propertyName) using __Typeof_##propertyName = propertyType;\
static void __Set_##propertyName(__Typeof_This* target, const propertyType& value) \
{ \
	target->propertyName = value; \
} \
static std::string __Get_PropName_##propertyName() { return #propertyName; }

enum class ValueType
{
	INT,
	BOOL
};


struct PropertyDescriptor
{
	std::string Name;
	ValueType Type;
	unsigned int Offset;
};


struct Person
{
	bool Alive;
	int Age;
};


struct BuilderContext
{
	std::vector<std::string> Messages;

	BuilderContext& Add(const std::string& next)
	{
		Messages.push_back(next);
		return *this;
	}

	void Finalize() const
	{
		std::cout << Messages.size() << std::endl;
	}
};


int main()
{

	return 0;
}