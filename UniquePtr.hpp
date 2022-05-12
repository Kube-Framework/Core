/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Unique pointer using allocator
 */

#pragma once

#include "Utils.hpp"

namespace kF::Core
{
    template<typename Type, kF::Core::Utils::StaticAllocator Allocator>
    class UniquePtr;
}

/** @brief Unique pointer class */
template<typename Type, kF::Core::Utils::StaticAllocator Allocator = kF::Core::Utils::DefaultStaticAllocator>
class kF::Core::UniquePtr
{
public:
    /** @brief Allocate an instance */
    template<typename ...Args>
    [[nodiscard]] static inline UniquePtr Make(Args &&...args) noexcept
        { return UniquePtr(new (Allocator::Allocate(sizeof(Type), alignof(Type))) Type(std::forward<Args>(args)...)); }

    /** @brief Allocate an instance */
    template<typename Derived, typename ...Args>
        requires std::derived_from<Derived, Type>
    [[nodiscard]] static inline UniquePtr Make(Args &&...args) noexcept
        { return UniquePtr(new (Allocator::Allocate(sizeof(Derived), alignof(Derived))) Derived(std::forward<Args>(args)...)); }


    /** @brief Destructor */
    inline ~UniquePtr(void) noexcept { release(); }

    /** @brief Construct the unique pointer */
    inline UniquePtr(void) noexcept = default;

    /** @brief Move constructor */
    inline UniquePtr(UniquePtr &&other) noexcept : _ptr(other._ptr) { other._ptr = nullptr; }

    /** @brief Move assignment */
    inline UniquePtr &operator=(UniquePtr &&other) noexcept
        { std::swap(_ptr, other._ptr); return *this; }


    /** @brief Move constructor from derived*/
    template<typename Other>
        requires std::derived_from<Other, Type>
    inline UniquePtr(UniquePtr<Other, Allocator> &&other) noexcept : _ptr(other.steal()) {}

    /** @brief Move assignment from derived */
    template<typename Other>
        requires std::derived_from<Other, Type>
    inline UniquePtr &operator=(UniquePtr<Other, Allocator> &&other) noexcept
        { release(); _ptr = other.steal(); return *this; }


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
    inline void swap(UniquePtr &other) noexcept { std::swap(_ptr, other._ptr); }

    /** @brief Destroy the managed object */
    inline void release(void) noexcept { if (_ptr) releaseUnsafe(); }


    /** @brief Steal internal pointer, the object must be freed manually */
    [[nodiscard]] inline Type *steal(void) noexcept { const auto ptr = _ptr; _ptr = nullptr; return ptr; }


    /** @brief Comparison operators */
    [[nodiscard]] auto operator<=>(const UniquePtr &other) const noexcept = default;

    /** @brief Raw pointer comparison */
    [[nodiscard]] bool operator==(const Type * const other) const noexcept { return _ptr == other; }
    [[nodiscard]] bool operator!=(const Type * const other) const noexcept { return _ptr != other; }

private:
    Type *_ptr { nullptr };


    /** @brief Instance constructor */
    inline UniquePtr(Type * const ptr) : _ptr(ptr) {}

    /** @brief Unsafe implementation of the release function */
    inline void releaseUnsafe(void) noexcept { _ptr->~Type(); Allocator::Deallocate(_ptr, sizeof(Type), alignof(Type)); _ptr = nullptr; }
};