/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SortedFlatVector
 */

#pragma once

#include "SortedVectorDetails.hpp"
#include "FlatVectorBase.hpp"

namespace kF::Core
{
    /**
     * @brief 8 bytes vector that allocates its size and capacity on the heap
     * The vector guarantee that it will be sorted at any given time
     *
     * @tparam Type Internal type in container
     * @tparam Compare Compare operator
     * @tparam CustomHeaderType Custom header of the flat vector
     * @tparam Range Range of container
     */
    template<typename Type, typename Compare = std::less<Type>, kF::Core::StaticAllocatorRequirements Allocator = DefaultStaticAllocator,
            typename CustomHeaderType = Internal::NoCustomHeaderType, std::integral Range = std::uint32_t>
    using SortedFlatVector = Internal::SortedVectorDetails<Internal::FlatVectorBase<Type, Allocator, CustomHeaderType, Range>, Type, Compare, Range, false, false>;


    /**
     * @brief 8 bytes vector using signed char with a long range
     * The vector guarantee that it will be sorted at any given time
     *
     * @tparam Type Internal type in container
     * @tparam Compare Compare operator
     * @tparam CustomHeaderType Custom header of the flat vector
     */
    template<typename Type, typename Compare = std::less<Type>, kF::Core::StaticAllocatorRequirements Allocator = DefaultStaticAllocator, typename CustomHeaderType = Internal::NoCustomHeaderType>
    using SortedLongFlatVector = SortedFlatVector<Type, Compare, Allocator, CustomHeaderType, std::size_t>;
}