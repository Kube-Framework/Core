/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Static allocator wrapper
 */

#pragma once

#include "UniquePtr.hpp"
#include "FixedString.hpp"

namespace kF::Core
{
    template<typename Allocator, FixedString Name>
    class StaticAllocator;
}


/** @brief Wrapper used to create static allocators */
template<typename Allocator, kF::Core::FixedString Name>
class kF::Core::StaticAllocator
{
public:
    /** @brief Allocate function that forward to AlignedAlloc */
    [[nodiscard]] static void *Allocate(const std::size_t bytes, const std::size_t alignment) noexcept;

    /** @brief Deallocate function that forward to AlignedFree */
    static void Deallocate(void * const data, const std::size_t bytes, const std::size_t alignment) noexcept;

private:
    static inline UniquePtr<Allocator> _Allocator {};


    /** @brief Ensure that the allocator instance is ready to use */
    static inline void CreateInstance(void) noexcept
        { _Allocator = UniquePtr<Allocator>::Make(); }
};

#include "StaticAllocator.ipp"