#pragma once
#include <cstddef>

namespace mp
{

namespace utility_str
{

constexpr size_t npos = static_cast<size_t>(-1);
size_t KMP_Find(const char *findstr, const char *substr);
size_t KMP_Find_Count(const char *findstr, const char *substr);

}; // namespace utility_str

} // namespace mp
