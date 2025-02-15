#pragma once

#include <stdexcept>
#include <string>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ ":" TOSTRING(__LINE__)

// C#'s NotImplementedException has grown on me
#define SE_THROW_NOT_IMPLEMENTED throw std::runtime_error(AT " Method not implemented!")

// now if you think this is not exactly a good way to report error, I agree, I just don't want to spend too much time up front, as long as it crashes the system it's fine
#define SE_THROW_GRAPHICS_EXCEPTION throw std::runtime_error(AT " Graphics error: see previous logs for details!")
