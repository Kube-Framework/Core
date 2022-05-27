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
    template<typename Type, StaticAllocatorRequirements Allocator = DefaultStaticAllocator, std::integral Range = std::size_t>
    using StringBase = Internal::StringDetails<Vector<Type, Allocator, Range>, Type, Range, false>;

    /**
     * @brief 16 bytes string that has its size and capacity close to the data pointer with a reduced range
     * The string is non-null terminated
     *
     * @tparam Type Internal type in container
     * @tparam Allocator Static Allocator
     */
    template<typename Type, StaticAllocatorRequirements Allocator = DefaultStaticAllocator>
    using TinyStringBase = StringBase<Type, Allocator, std::uint32_t>;

    /**
     * @brief 24 bytes string using signed char
     * The string is non-null terminated */
    template<StaticAllocatorRequirements Allocator = DefaultStaticAllocator>
    using String = StringBase<char, Allocator>;

    /**
     * @brief 16 bytes string using signed char with a reduced range
     * The string is non-null terminated */
    template<StaticAllocatorRequirements Allocator = DefaultStaticAllocator>
    using TinyString = TinyStringBase<char, Allocator>;

    /**
     * @brief 24 bytes string using signed wchar
     * The string is non-null terminated */
    template<StaticAllocatorRequirements Allocator = DefaultStaticAllocator>
    using WString = StringBase<wchar_t, Allocator>;

    /**
     * @brief 16 bytes string using signed wchar with a reduced range
     * The string is non-null terminated */
    template<StaticAllocatorRequirements Allocator = DefaultStaticAllocator>
    using TinyWString = TinyStringBase<wchar_t, Allocator>;
}

static_assert_sizeof(kF::Core::String<>, 3 * kF::Core::CacheLineEighthSize);
static_assert_sizeof_quarter_cacheline(kF::Core::TinyString<>);
static_assert_sizeof(kF::Core::WString<>, 3 * kF::Core::CacheLineEighthSize);
static_assert_sizeof_quarter_cacheline(kF::Core::TinyWString<>);