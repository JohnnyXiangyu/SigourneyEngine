#pragma once

namespace Utils {

#define Property(Type, Name) private: Type m_##Name; protected: void Set##Name(Type value) { m_##Name = value; } public: Type Get##Name() { return m_##Name; }

}