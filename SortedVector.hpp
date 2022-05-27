/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SortedVector
 */

#pragma once

#include "SortedVectorDetails.hpp"
#include "VectorBase.hpp"

namespace kF::Core
{
    /**
     * @brief Sorted Vector that has its size and capacity close to the data pointer
     * With default range (std::uint32_t), the vector takes 16 bytes by default
     * The vector may take a static allocator
     *
     * @tparam Type Internal type in container
     * @tparam Compare Compare operator
     * @tparam Allocator Static Allocator
     * @tparam Range Range of container
     */
    template<typename Type, typename Compare = std::less<Type>, StaticAllocatorRequirements Allocator = DefaultStaticAllocator, std::integral Range = std::uint32_t>
    using SortedVector = Internal::SortedVectorDetails<Internal::VectorBase<Type, Allocator, Range>, Type, Compare, Range, false, false>;

    /**
     * @brief 24 bytes sorted vector with a long range
     * The vector may take a static allocator
     *
     * @tparam Type Internal type in container
     * @tparam Compare Compare operator
     * @tparam Allocator Static Allocator
     */
    template<typename Type, typename Compare = std::less<Type>, StaticAllocatorRequirements Allocator = DefaultStaticAllocator>
    using SortedLongVector = SortedVector<Type, Compare, Allocator, std::size_t>;
}