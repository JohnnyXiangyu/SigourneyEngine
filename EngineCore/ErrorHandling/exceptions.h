#pragma once

#include <stdexcept>
#include <string>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ ":" TOSTRING(__LINE__)

#define SE_THROW_NOT_IMPLEMENTED throw std::runtime_error(AT " Method not implemented!")
