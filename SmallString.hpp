/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SmallString
 */

#pragma once

#include "SmallVector.hpp"
#include "StringDetails.hpp"

namespace kF::Core
{
    /**
     * @brief String that has its size, capacity and a small cache close to the data pointer
     * The string is non-null terminated
     *
     * @tparam Type Internal type in container
     * @tparam OptimizedCapacity Count of element in the optimized cache
     * @tparam Allocator Static Allocator
     * @tparam Range Range of container
     */
    template<typename Type, std::size_t OptimizedCapacity, StaticAllocatorRequirements Allocator = DefaultStaticAllocator, std::integral Range = std::size_t>
    using SmallStringBase = Internal::StringDetails<SmallVector<Type, OptimizedCapacity, Allocator, Range>, Type, Range, false>;

    /**
     * @brief Small optimized string with a reduced range
     *  The string is non-null terminated
     *
     * @tparam Type Internal type in container
     * @tparam OptimizedCapacity Count of element in the optimized cache
     * @tparam Allocator Static Allocator
     */
    template<typename Type, std::size_t OptimizedCapacity, StaticAllocatorRequirements Allocator = DefaultStaticAllocator>
    using TinySmallStringBase = SmallStringBase<Type, OptimizedCapacity, Allocator, std::uint32_t>;

    /**
     * @brief 32 bytes small optimized string (cache of 8 bytes)
     * The string is non-null terminated */
    template<StaticAllocatorRequirements Allocator = DefaultStaticAllocator>
    using SmallString = SmallStringBase<char, CacheLineEighthSize, Allocator>;

    /**
     * @brief 32 bytes small optimized string (cache of 16 bytes) with a reduced range
     * The string is non-null terminated */
    template<StaticAllocatorRequirements Allocator = DefaultStaticAllocator>
    using TinySmallString = TinySmallStringBase<char, CacheLineQuarterSize, Allocator>;

    /**
     * @brief 64 bytes small optimized string (cache of 40 bytes)
     * The string is non-null terminated */
    template<StaticAllocatorRequirements Allocator = DefaultStaticAllocator>
    using SmallWString = SmallStringBase<wchar_t, (CacheLineHalfSize + CacheLineEighthSize) / sizeof(wchar_t), Allocator>;

    /**
     * @brief 64 bytes small optimized string (cache of 48 bytes) with a reduced range
     * The string is non-null terminated */
    template<StaticAllocatorRequirements Allocator = DefaultStaticAllocator>
    using TinySmallWString = TinySmallStringBase<wchar_t, (CacheLineHalfSize + CacheLineQuarterSize) / sizeof(wchar_t), Allocator>;
}

static_assert_sizeof_half_cacheline(kF::Core::SmallString<>);
static_assert_sizeof_half_cacheline(kF::Core::TinySmallString<>);
static_assert_sizeof_cacheline(kF::Core::SmallWString<>);
static_assert_sizeof_cacheline(kF::Core::TinySmallWString<>);
