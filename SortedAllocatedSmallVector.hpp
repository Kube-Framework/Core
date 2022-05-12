/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SortedAllocatedSmallVector
 */

#pragma once

#include "SortedVectorDetails.hpp"
#include "AllocatedSmallVectorBase.hpp"

namespace kF::Core
{
    /**
     * @brief Vector that has its size, capacity and a small cache close to the data pointer
     * The vector guarantee that it will be sorted at any given time
     *
     * @tparam Type Internal type in container
     * @tparam OptimizedCapacity Count of element in the optimized cache
     * @tparam Compare Compare operator
     * @tparam Range Range of container
     */
    template<typename Type, std::size_t OptimizedCapacity, typename Compare = std::less<Type>, std::integral Range = std::size_t>
    using SortedAllocatedSmallVector = Internal::SortedVectorDetails<Internal::AllocatedSmallVectorBase<Type, OptimizedCapacity, Range>, Type, Compare, Range, true, true>;

    /** @brief Small optimized vector with a reduced range
     * The vector guarantee that it will be sorted at any given time
     *
     * @tparam Type Internal type in container
     * @tparam OptimizedCapacity Count of element in the optimized cache
     * @tparam Compare Compare operator
     */
    template<typename Type, std::size_t OptimizedCapacity, typename Compare = std::less<Type>>
    using SortedAllocatedTinySmallVector = SortedAllocatedSmallVector<Type, OptimizedCapacity, Compare, std::uint32_t>;
}