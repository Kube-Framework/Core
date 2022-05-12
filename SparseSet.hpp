/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Sparse set
 */

#pragma once

#include "UniquePtr.hpp"
#include "Vector.hpp"

namespace kF::Core
{
    template<typename Type, std::size_t PageSize, kF::Core::Utils::StaticAllocator Allocator, std::integral Range>
    class SparseSet;
}

/** @brief The sparse index set is a container which provide O(1) look-up time at the cost of non-efficient memory consumption
 *  @note This implementation is not aware of which index is initialized the user must manage lifecycle **carefully** */
template<typename Type, std::size_t PageSize, kF::Core::Utils::StaticAllocator Allocator = kF::Core::Utils::DefaultStaticAllocator, std::integral Range = std::uint32_t>
class kF::Core::SparseSet
{
public:
    /** @brief Page containing values */
    struct alignas(alignof(Type)) Page
    {
        std::uint8_t data[PageSize * sizeof(Type)];
    };

    /** @brief Pointer over page */
    using PagePtr = UniquePtr<Page, Allocator>;

    /** @brief If true, the set can be cleared and released safely */
    static constexpr bool IsSafeToClear = std::is_trivially_destructible_v<Type>;


    /** @brief Get page index of element */
    [[nodiscard]] static inline Range GetPageIndex(const Range index) noexcept { return index / PageSize; }

    /** @brief Get element index inside page */
    [[nodiscard]] static inline Range GetElementIndex(const Range index) noexcept { return index % PageSize; }


    /** @brief Default destructor */
    ~SparseSet(void) noexcept = default;

    /** @brief Default constructor */
    SparseSet(void) noexcept = default;

    /** @brief Default move constructor */
    SparseSet(SparseSet &&other) noexcept = default;

    /** @brief Move assignment */
    SparseSet &operator=(SparseSet &&other) noexcept = default;

    /** @brief Swap two instances */
    inline void swap(SparseSet &other) noexcept { _pages.swap(other._pages); }


    /** @brief Add a new value to the set */
    template<typename ...Args>
    Type &add(const Range index, Args &&...args) noexcept;


    /** @brief Remove a value from the set and return it */
    void remove(const Range index) noexcept;

    /** @brief Remove a value by page & elemnt index */
    void remove(const Range pageIndex, const Range elementIndex) noexcept;


    /** @brief Extract and remove a value from the set and return it */
    [[nodiscard]] inline Type extract(const Range index) noexcept
        { return extract(GetPageIndex(index), GetElementIndex(index)); }

    /** @brief Extract and remove a value by page & elemnt index */
    [[nodiscard]] Type extract(const Range pageIndex, const Range elementIndex) noexcept;


    /** @brief Get value reference by index */
    [[nodiscard]] inline Type &at(const Range index) noexcept
        { return at(GetPageIndex(index), GetElementIndex(index)); }
    [[nodiscard]] inline const Type &at(const Range index) const noexcept
        { return at(GetPageIndex(index), GetElementIndex(index)); }

    /** @brief Get value reference by page & element index */
    [[nodiscard]] inline Type &at(const Range pageIndex, const Range elementIndex) noexcept
        { return reinterpret_cast<Type *>(_pages[pageIndex].get())[elementIndex]; }
    [[nodiscard]] inline const Type &at(const Range pageIndex, const Range elementIndex) const noexcept
        { return reinterpret_cast<const Type *>(_pages[pageIndex].get())[elementIndex]; }


    /** @brief Release all memory without calling any Type destructors */
    inline void clearUnsafe(void) noexcept { _pages.clear(); }

    /** @brief Release all memory without calling any Type destructors */
    inline void releaseUnsafe(void) noexcept { _pages.release(); }

private:
    Core::Vector<PagePtr, Allocator, Range> _pages {};
};

#include "SparseSet.ipp"