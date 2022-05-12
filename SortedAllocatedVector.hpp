/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SortedAllocatedVector
 */

#pragma once

#include "SortedVectorDetails.hpp"
#include "AllocatedVectorBase.hpp"

namespace kF::Core
{
    /**
     * @brief Vector that has its size and capacity close to the data pointer
     * With default range (std::size_t), the vector takes 32 bytes
     * The vector guarantee that it will be sorted at any given time
     *
     * @tparam Type Internal type in container
     * @tparam Compare Compare operator
     * @tparam Range Range of container
     */
    template<typename Type, typename Compare = std::less<Type>, std::integral Range = std::size_t>
    using SortedAllocatedVector = Internal::SortedVectorDetails<Internal::AllocatedVectorBase<Type, Range>, Type, Compare, Range, false, true>;

    /**
     * @brief 24 bytes vector that has its size and capacity close to the data pointer with reduced range
     * The vector guarantee that it will be sorted at any given time
     *
     * @tparam Type Internal type in container
     * @tparam Compare Compare operator
     */
    template<typename Type, typename Compare = std::less<Type>>
    using SortedAllocatedTinyVector = SortedAllocatedVector<Type, Compare, std::uint32_t>;
}