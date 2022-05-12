/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Small Vector
 */

#pragma once

#include "VectorDetails.hpp"
#include "SmallVectorBase.hpp"

namespace kF::Core
{
    /**
     * @brief Vector that has its size, capacity and a small cache close to the data pointer
     * The vector may take a static allocator
     *
     * @tparam Type Internal type in container
     * @tparam OptimizedCapacity Count of element in the optimized cache
     * @tparam Allocator Static Allocator
     * @tparam Range Range of container
     */
    template<typename Type, std::size_t OptimizedCapacity, Utils::StaticAllocator Allocator = Utils::DefaultStaticAllocator,
            std::integral Range = std::size_t>
    using SmallVector = Internal::VectorDetails<Internal::SmallVectorBase<Type, OptimizedCapacity, Allocator, Range>, Type, Range, true, false>;

    /** @brief Small optimized vector with a reduced range
     * The vector may take a static allocator
     *
     * @tparam Type Internal type in container
     * @tparam OptimizedCapacity Count of element in the optimized cache
     * @tparam Allocator Static Allocator
    */
    template<typename Type, std::size_t OptimizedCapacity, Utils::StaticAllocator Allocator = Utils::DefaultStaticAllocator>
    using TinySmallVector = SmallVector<Type, OptimizedCapacity, Allocator, std::uint32_t>;
}
