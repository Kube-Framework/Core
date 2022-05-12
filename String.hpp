/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: String
 */

#pragma once

#include "StringDetails.hpp"
#include "Vector.hpp"

namespace kF::Core
{
    /**
     * @brief 24 bytes string that has its size and capacity close to the data pointer
     * The string is non-null terminated
     *
     * @tparam Type Internal type in container
     * @tparam Allocator Static Allocator
     * @tparam Range Range of container
     */
    template<typename Type, Utils::StaticAllocator Allocator = Utils::DefaultStaticAllocator, std::integral Range = std::size_t>
    using StringBase = Internal::StringDetails<Vector<Type, Allocator, Range>, Type, Range, false>;

    /**
     * @brief 16 bytes string that has its size and capacity close to the data pointer with a reduced range
     * The string is non-null terminated
     *
     * @tparam Type Internal type in container
     * @tparam Allocator Static Allocator
     */
    template<typename Type, Utils::StaticAllocator Allocator = Utils::DefaultStaticAllocator>
    using TinyStringBase = StringBase<Type, Allocator, std::uint32_t>;

    /**
     * @brief 24 bytes string using signed char
     * The string is non-null terminated
     */
    using String = StringBase<char>;

    /**
     * @brief 16 bytes string using signed char with a reduced range
     * The string is non-null terminated
     */
    using TinyString = TinyStringBase<char>;
}

static_assert_sizeof(kF::Core::String, 3 * kF::Core::CacheLineEighthSize);
static_assert_sizeof_quarter_cacheline(kF::Core::TinyString);