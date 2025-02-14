#include "data_type.h"

using namespace Engine::Core::Reflection;

static const char BOOL_NAME[] = "BOOL";
static const char INT32_NAME[] = "INT32";
static const char UINT32_NAME[] = "UINT32";
static const char INT64_NAME[] = "INT64";
static const char UINT64_NAME[] = "UINT64";
static const char FLOAT_NAME[] = "FLOAT";
static const char DOUBLE_NAME[] = "DOUBLE";
static const char VEC2_NAME[] = "VEC2";
static const char VEC3_NAME[] = "VEC3";
static const char VEC4_NAME[] = "VEC4";
static const char MAT2_NAME[] = "MAT2";
static const char MAT3_NAME[] = "MAT3";
static const char MAT4_NAME[] = "MAT4";
static const char ERROR_NAME[] = "???";

const char* Engine::Core::Reflection::PrintType(DataType type)
{
	switch (type)
	{
	case DataType::BOOL:
		return BOOL_NAME;
	case DataType::INT32:
		return INT32_NAME;
	case DataType::UINT32:
		return UINT32_NAME;
	case DataType::INT64:
		return INT64_NAME;
	case DataType::UINT64:
		return UINT64_NAME;
	case DataType::FLOAT:
		return FLOAT_NAME;
	case DataType::DOUBLE:
		return DOUBLE_NAME;
	case DataType::VEC2:
		return VEC2_NAME;
	case DataType::VEC3:
		return VEC3_NAME;
	case DataType::VEC4:
		return VEC4_NAME;
	case DataType::MAT2:
		return MAT2_NAME;
	case DataType::MAT3:
		return MAT3_NAME;
	case DataType::MAT4:
		return MAT4_NAME;
	default:
		return ERROR_NAME;
	}
}
