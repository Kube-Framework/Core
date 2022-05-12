/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Static allocator wrapper
 */

#include "StaticAllocator.hpp"

template<typename Allocator, kF::Core::FixedString Name>
inline void *kF::Core::StaticAllocator<Allocator, Name>::Allocate(const std::size_t bytes, const std::size_t alignment) noexcept
{
    if (_Allocator) [[likely]]
AllocateLabel:
        return _Allocator->allocate(bytes, alignment);
    else [[unlikely]] {
        CreateInstance();
        goto AllocateLabel;
    }
}

template<typename Allocator, kF::Core::FixedString Name>
inline void kF::Core::StaticAllocator<Allocator, Name>::Deallocate(void * const data, const std::size_t bytes, const std::size_t alignment) noexcept
{
    if (_Allocator) [[likely]]
DeallocateLabel:
        return _Allocator->deallocate(data, bytes, alignment);
    else [[unlikely]] {
        CreateInstance();
        goto DeallocateLabel;
    }
}
