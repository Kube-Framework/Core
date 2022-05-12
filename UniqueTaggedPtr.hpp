/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Unique pointer using allocator
 */

#pragma once

#include "TaggedPtr.hpp"

namespace kF::Core
{
    template<typename Type, std::size_t Alignment, kF::Core::Utils::StaticAllocator Allocator>
    class UniqueTaggedPtr;
}

/** @brief Unique pointer class */
template<typename Type, std::size_t Alignment = alignof(Type), kF::Core::Utils::StaticAllocator Allocator = kF::Core::Utils::DefaultStaticAllocator>
class kF::Core::UniqueTaggedPtr : public TaggedPtr<Type, Alignment>
{
public:
    /** @brief Allocate an instance */
    template<typename ...Args>
    [[nodiscard]] static inline UniqueTaggedPtr Make(Args &&...args) noexcept
        { return UniqueTaggedPtr(new (Allocator::Allocate(sizeof(Type), alignof(Type))) Type(std::forward<Args>(args)...)); }


    /** @brief Destructor */
    inline ~UniqueTaggedPtr(void) noexcept { release(); }

    /** @brief Construct the unique pointer */
    inline UniqueTaggedPtr(void) noexcept = default;

    /** @brief Move constructor */
    inline UniqueTaggedPtr(UniqueTaggedPtr &&other) noexcept : _ptr(other._ptr) { other._ptr = nullptr; }

    /** @brief Move assignment */
    inline UniqueTaggedPtr &operator=(UniqueTaggedPtr &&other) noexcept { swap(other); return *this; }


    /** @brief Boolean check operator */
    [[nodiscard]] inline operator bool(void) const noexcept { return _ptr != nullptr; }


    /** @brief Mutable managed object getter */
    [[nodiscard]] inline Type *get(void) noexcept { return _ptr; }

    /** @brief Constant managed object getter */
    [[nodiscard]] inline const Type *get(void) const noexcept { return _ptr; }


    /** @brief Mutable managed object pointer access */
    [[nodiscard]] inline Type *operator->(void) noexcept { return _ptr; }

    /** @brief Constant managed object pointer access */
    [[nodiscard]] inline const Type *operator->(void) const noexcept { return _ptr; }


    /** @brief Mutable managed object reference access */
    [[nodiscard]] inline Type &operator*(void) noexcept { return *_ptr; }

    /** @brief Constant managed object reference access */
    [[nodiscard]] inline const Type &operator*(void) const noexcept { return *_ptr; }


    /** @brief Swaps the managed objects */
    inline void swap(UniqueTaggedPtr &other) noexcept { std::swap(_ptr, other._ptr); }

    /** @brief Destroy the managed object */
    inline void release(void) noexcept { if (_ptr) releaseUnsafe(); }

private:
    /** @brief Instance constructor */
    inline UniqueTaggedPtr(Type * const ptr) : _ptr(ptr) {}

    /** @brief Unsafe implementation of the release function */
    inline void releaseUnsafe(void) noexcept { _ptr->~Type(); Allocator::Deallocate(_ptr, sizeof(Type), alignof(Type)); }
};