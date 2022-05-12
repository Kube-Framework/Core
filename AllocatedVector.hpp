/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: AllocatedVector
 */

#pragma once

#include "VectorDetails.hpp"
#include "AllocatedVectorBase.hpp"

namespace kF::Core
{
    /**
     * @brief Vector that has its size and capacity close to the data pointer
     * With default range (std::size_t), the vector takes 32 bytes
     *
     * @tparam Type Internal type in container
     * @tparam Range Range of container
     */
    template<typename Type, std::integral Range = std::size_t>
    using AllocatedVector = Internal::VectorDetails<Internal::AllocatedVectorBase<Type, Range>, Type, Range, false, true>;

    /** @brief 24 bytes vector with a reduced range
     *
     * @tparam Type Internal type in container
     */
    template<typename Type>
    using AllocatedTinyVector = AllocatedVector<Type, std::uint32_t>;
}