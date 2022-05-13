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
     * With default range (std::size_t), the vector takes 24 bytes
     * The vector may take a static allocator
     *
     * @tparam Type Internal type in container
     * @tparam Compare Compare operator
     * @tparam Allocator Static Allocator
     * @tparam Range Range of container
     */
    template<typename Type, typename Compare = std::less<Type>, StaticAllocatorRequirements Allocator = DefaultStaticAllocator, std::integral Range = std::size_t>
    using SortedVector = Internal::SortedVectorDetails<Internal::VectorBase<Type, Allocator, Range>, Type, Compare, Range, false, false>;

    /**
     * @brief 16 bytes sorted vector with a reduced range
     * The vector may take a static allocator
     *
     * @tparam Type Internal type in container
     * @tparam Compare Compare operator
     * @tparam Allocator Static Allocator
     */
    template<typename Type, typename Compare = std::less<Type>, StaticAllocatorRequirements Allocator = DefaultStaticAllocator>
    using SortedTinyVector = SortedVector<Type, Compare, Allocator, std::uint32_t>;
}