/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Vector
 */

#pragma once

#include "VectorDetails.hpp"
#include "VectorBase.hpp"

namespace kF::Core
{
    /**
     * @brief Vector that has its size and capacity close to the data pointer
     * With default range (std::size_t), the vector takes 24 bytes
     * The vector may take a static allocator
     *
     * @tparam Type Internal type in container
     * @tparam Allocator Static Allocator
     * @tparam Range Range of container
     */
    template<typename Type, Utils::StaticAllocator Allocator = Utils::DefaultStaticAllocator, std::integral Range = std::size_t>
    using Vector = Internal::VectorDetails<Internal::VectorBase<Type, Allocator, Range>, Type, Range, false, false>;

    /**
     * @brief 16 bytes vector with a reduced range
     * The vector may take a static allocator
     *
     * @tparam Type Internal type in container
     * @tparam Allocator Static Allocator
     */
    template<typename Type, Utils::StaticAllocator Allocator = Utils::DefaultStaticAllocator>
    using TinyVector = Vector<Type, Allocator, std::uint32_t>;
}