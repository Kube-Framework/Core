/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: AllocatedFlatString
 */

#pragma once

#include "FlatString.hpp"
#include "AllocatedFlatVector.hpp"

namespace kF::Core
{
    /**
     * @brief 8 bytes string that allocates its size and capacity on the heap
     * The string is non-null terminated
     *
     * @tparam Type Type of character
     * @tparam Range Range of container
     */
    template<typename Type, std::integral Range = std::size_t>
    using AllocatedFlatStringBase = Internal::StringDetails<AllocatedFlatVector<Type, Range>, Type, Range, true>;

    /**
     * @brief 8 bytes string that allocates its size and capacity on the heap with a reduced range
     * The string is non-null terminated
     *
     * @tparam Type Type of character
     */
    template<typename Type>
    using AllocatedTinyFlatStringBase = AllocatedFlatStringBase<Type, std::uint32_t>;

    /** @brief 8 bytes string that allocates its size and capacity on the heap
     *  The string is non-null terminated */
    using AllocatedFlatString = AllocatedFlatStringBase<char>;

    /** @brief 8 bytes string that allocates its size and capacity on the heap with a reduced range
     * The string is non-null terminated */
    using AllocatedTinyFlatString = AllocatedTinyFlatStringBase<char>;
}