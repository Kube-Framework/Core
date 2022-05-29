/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: String utils
 */

#pragma once

#include <ostream>

#include "StringDetails.hpp"

template<typename Base, typename Type, std::integral Range, bool IsAllocated>
inline std::ostream &operator<<(std::ostream &stream, const kF::Core::Internal::StringDetails<Base, Type, Range, IsAllocated> &str)
{
    return stream << str.toView();
}
