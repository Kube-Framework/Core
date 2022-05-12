/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: AllocatedSmallString
 */

#pragma once

#include "AllocatedSmallVector.hpp"
#include "StringDetails.hpp"

namespace kF::Core
{
    /**
     * @brief String that has its size, capacity and a small cache close to the data pointer
     * The string is non-null terminated
     *
     * @tparam Type Type of character
     * @tparam OptimizedCapacity Count of element in the optimized cache
     * @tparam Range Range of container
     */
    template<typename Type, std::size_t OptimizedCapacity, std::integral Range = std::size_t>
    using AllocatedSmallStringBase = Internal::StringDetails<AllocatedSmallVector<Type, OptimizedCapacity, Range>, Type, Range, true>;

    /**
     * @brief Small optimized string with a reduced range
     * The string is non-null terminated
     *
     * @tparam Type Type of character
     * @tparam OptimizedCapacity Count of element in the optimized cache
     */
    template<typename Type, std::size_t OptimizedCapacity>
    using AllocatedTinySmallStringBase = AllocatedSmallStringBase<Type, OptimizedCapacity, std::uint32_t>;

    /** @brief 64 bytes small optimized string (default cache of 32 bytes)
     * The string is non-null terminated
     *
     * @tparam OptimizedCapacity Count of element in the optimized cache
     */
    using AllocatedSmallString = AllocatedSmallStringBase<char, CacheLineHalfSize>;

    /** @brief 32 bytes small optimized string (default cache of 8 bytes) with a reduced range
     * The string is non-null terminated
     *
     * @tparam OptimizedCapacity Count of element in the optimized cache
     */
    using AllocatedTinySmallString = AllocatedTinySmallStringBase<char, CacheLineEighthSize>;
}
