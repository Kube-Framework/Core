/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Sparse set
 */

#include "Assert.hpp"
#include "SparseSet.hpp"

template<typename Type, std::size_t PageSize, kF::Core::Utils::StaticAllocator Allocator, std::integral Range>
template<typename ...Args>
inline Type &kF::Core::SparseSet<Type, PageSize, Allocator, Range>::add(const Range index, Args &&...args) noexcept
{
    const auto pageIndex = GetPageIndex(index);
    const auto elementIndex = GetElementIndex(index);
    if (const auto size = _pages.size(); pageIndex >= size) [[unlikely]]
        _pages.insertDefault(_pages.end(), 1 + pageIndex - size);

    auto &page = _pages[pageIndex];
    if (!page) [[unlikely]]
        page = PagePtr::Make();

    return *new (&at(pageIndex, elementIndex)) Type(std::forward<Args>(args)...);

}

template<typename Type, std::size_t PageSize, kF::Core::Utils::StaticAllocator Allocator, std::integral Range>
inline void kF::Core::SparseSet<Type, PageSize, Allocator, Range>::remove(const Range index) noexcept
{
    if constexpr (!std::is_trivially_destructible_v<Type>) {
        remove(GetPageIndex(index), GetElementIndex(index));
    }
}

template<typename Type, std::size_t PageSize, kF::Core::Utils::StaticAllocator Allocator, std::integral Range>
inline void kF::Core::SparseSet<Type, PageSize, Allocator, Range>::remove(const Range pageIndex, const Range elementIndex) noexcept
{
    if constexpr (!std::is_trivially_destructible_v<Type>) {
        at(pageIndex, elementIndex).~Type();
    }
}

template<typename Type, std::size_t PageSize, kF::Core::Utils::StaticAllocator Allocator, std::integral Range>
inline Type kF::Core::SparseSet<Type, PageSize, Allocator, Range>::extract(const Range pageIndex, const Range elementIndex) noexcept
{
    auto &ref = at(pageIndex, elementIndex);
    Type value(std::move(ref));

    if constexpr (!std::is_trivially_destructible_v<Type>) {
        ref.~Type();
    }
    return value;
}