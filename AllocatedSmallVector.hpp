/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: AllocatedSmallVector
 */

#pragma once

#include "VectorDetails.hpp"
#include "AllocatedSmallVectorBase.hpp"

namespace kF::Core
{
    /**
     * @brief Vector that has its size, capacity and a small cache close to the data pointer
     *
     * @tparam Type Internal type in container
     * @tparam OptimizedCapacity Count of element in the optimized cache
     * @tparam Range Range of container
     */
    template<typename Type, std::size_t OptimizedCapacity, std::integral Range = std::size_t>
    using AllocatedSmallVector = Internal::VectorDetails<Internal::AllocatedSmallVectorBase<Type, OptimizedCapacity, Range>, Type, Range, true, true>;

    /**
     * @brief Small optimized vector with a reduced range
     *
     * @tparam Type Internal type in container
     * @tparam OptimizedCapacity Count of element in the optimized cache
     */
    template<typename Type, std::size_t OptimizedCapacity>
    using AllocatedTinySmallVector = AllocatedSmallVector<Type, OptimizedCapacity, std::uint32_t>;
}
