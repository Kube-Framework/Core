/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: String utils
 */

#pragma once

#include <ostream>

#include "StringDetails.hpp"

template<typename Base, typename Type, std::integral Range, bool IsAllocated>
inline std::wostream &operator<<(std::wostream &stream, const kF::Core::Internal::StringDetails<Base, Type, Range, IsAllocated> &str)
{
    return stream << str.toView();
}

template<typename Base, typename Type, std::integral Range, bool IsAllocated>
inline std::wostream &operator<<(std::wostream &stream, const kF::Core::Internal::StringDetails<Base, Type, Range, IsAllocated> &str)
{
    return stream << str.toView();
}