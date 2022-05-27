/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Log helper
 */

#include <iostream>

#include "Log.hpp"

using namespace kF;

Core::Log Core::InfoLog(std::wcout);
Core::Log Core::ErrorLog(std::wcerr);

std::wostream &operator<<(std::wostream &lhs, const std::string_view &rhs) noexcept
{
    lhs << rhs.data();
    return lhs;
}
