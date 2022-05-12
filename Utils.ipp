/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Utils
 */

#include "Utils.hpp"

template<std::integral Range>
constexpr Range kF::Core::Utils::AlignOffset(const Range offset, const Range alignment) noexcept
{
    Range skip = offset & (alignment - 1);
    if (skip)
        skip = alignment - skip;
    return offset + skip;
}

template<std::integral Unit>
constexpr Unit kF::Core::Utils::IsPowerOf2(Unit value) noexcept
{
    return value && !(value & (value - static_cast<Unit>(1)));
}

template<std::integral Unit>
constexpr Unit kF::Core::Utils::NextPowerOf2(Unit value) noexcept
{
    if (value && !(value & (value - static_cast<Unit>(1))))
        return value;
    auto count = static_cast<Unit>(0);
    while (value != 0) {
        value >>= 1;
        ++count;
    }
    return static_cast<Unit>(1) << count;
}

template<std::integral Unit, std::integral ResultUnit>
constexpr ResultUnit kF::Core::Utils::NextPowerOf2Bit(Unit value) noexcept
{
    auto count = static_cast<Unit>(0);
    value -= value && !(value & (value - static_cast<Unit>(1))); // Minus 1 when already a power of 2
    while (value != 0) {
        value >>= 1;
        ++count;
    }
    return count;
}