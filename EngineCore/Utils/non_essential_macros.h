#pragma once

namespace Engine
{
namespace Core
{
namespace Utils
{

#define Property(Type, Name) private: \
	Type m_##Name;\
protected:\
	inline void Set##Name(Type value)\
	{\
		m_##Name = value; \
	}\
public:\
	inline Type Get##Name()\
	{\
		return m_##Name;\
	}

}
}
}
