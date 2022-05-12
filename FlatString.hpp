/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: FlatString
 */

#pragma once

#include "StringDetails.hpp"
#include "FlatVector.hpp"

namespace kF::Core
{
    /**
     * @brief 8 bytes string
     * The string is non-null terminated
     *
     * @tparam Type Internal type in container
     * @tparam Allocator Static Allocator
     * @tparam Range Range of container
     */
    template<typename Type, Utils::StaticAllocator Allocator = Utils::DefaultStaticAllocator, std::integral Range = std::size_t>
    using FlatStringBase = Internal::StringDetails<FlatVector<Type, Allocator, Range>, Type, Range, false>;

    /**
     * @brief 8 bytes string with a reduced range
     * The string is non-null terminated
     *
     * @tparam Type Internal type in container
     * @tparam Allocator Static Allocator
     */
    template<typename Type, Utils::StaticAllocator Allocator = Utils::DefaultStaticAllocator>
    using TinyFlatStringBase = FlatStringBase<Type, Allocator, std::uint32_t>;

    /**
     * @brief 8 bytes string using signed char and size_t range
     * The string is non-null terminated
     */
    using FlatString = FlatStringBase<char>;

    /**
     * @brief 8 bytes string using signed char with a reduced range
     * The string is non-null terminated
     */
    using TinyFlatString = TinyFlatStringBase<char>;
}

static_assert_sizeof(kF::Core::FlatString, kF::Core::CacheLineEighthSize);
static_assert_sizeof(kF::Core::TinyFlatString, kF::Core::CacheLineEighthSize);
