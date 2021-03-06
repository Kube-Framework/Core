/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SortedVectorDetails
 */

template<typename Base, typename Type, typename Compare, std::integral Range, bool IsSmallOptimized, bool IsAllocated>
template<typename ...Args> requires std::constructible_from<Type, Args...>
inline Type &kF::Core::Internal::SortedVectorDetails<Base, Type, Compare, Range, IsSmallOptimized, IsAllocated>::push(Args &&...args) noexcept
{
    if (!DetailsBase::data()) [[unlikely]]
        return DetailsBase::push(std::forward<Args>(args)...);
    Type value(std::forward<Args>(args)...);
    auto it = findSortedPlacement(value);
    return *DetailsBase::insert(it, { std::move(value) });
}

template<typename Base, typename Type, typename Compare, std::integral Range, bool IsSmallOptimized, bool IsAllocated>
inline void kF::Core::Internal::SortedVectorDetails<Base, Type, Compare, Range, IsSmallOptimized, IsAllocated>::insertDefault(const Range count) noexcept
{
    if (count) [[likely]]
        DetailsBase::insertDefault(findSortedPlacement(Type{}), count);
}

template<typename Base, typename Type, typename Compare, std::integral Range, bool IsSmallOptimized, bool IsAllocated>
inline void kF::Core::Internal::SortedVectorDetails<Base, Type, Compare, Range, IsSmallOptimized, IsAllocated>::insertCopy(
        const Range count, const Type &value) noexcept
{
    if (count) [[likely]]
        DetailsBase::insertCopy(findSortedPlacement(value), count, value);
}

template<typename Base, typename Type, typename Compare, std::integral Range, bool IsSmallOptimized, bool IsAllocated>
template<std::input_iterator InputIterator>
inline void kF::Core::Internal::SortedVectorDetails<Base, Type, Compare, Range, IsSmallOptimized, IsAllocated>::insert(
        InputIterator from, InputIterator to) noexcept
{
    if (from != to) [[likely]] {
        DetailsBase::insert(DetailsBase::end(), from, to);
        sort();
    }
}

template<typename Base, typename Type, typename Compare, std::integral Range, bool IsSmallOptimized, bool IsAllocated>
template<std::input_iterator InputIterator, typename Map>
inline void kF::Core::Internal::SortedVectorDetails<Base, Type, Compare, Range, IsSmallOptimized, IsAllocated>::insert(
        InputIterator from, InputIterator to, Map &&map) noexcept
{
    if (from != to) [[likely]] {
        DetailsBase::insert(DetailsBase::end(), from, to, std::forward<Map>(map));
        sort();
    }
}

template<typename Base, typename Type, typename Compare, std::integral Range, bool IsSmallOptimized, bool IsAllocated>
template<std::input_iterator InputIterator>
inline void kF::Core::Internal::SortedVectorDetails<Base, Type, Compare, Range, IsSmallOptimized, IsAllocated>::resize(
        InputIterator from, InputIterator to) noexcept
{
    if (from != to) [[likely]] {
        DetailsBase::resize(from, to);
        sort();
    }
}

template<typename Base, typename Type, typename Compare, std::integral Range, bool IsSmallOptimized, bool IsAllocated>
template<std::input_iterator InputIterator, typename Map>
inline void kF::Core::Internal::SortedVectorDetails<Base, Type, Compare, Range, IsSmallOptimized, IsAllocated>::resize(
        InputIterator from, InputIterator to, Map &&map) noexcept
{
    if (from != to) [[likely]] {
        DetailsBase::resize(from, to, std::forward<Map>(map));
        sort();
    }
}

template<typename Base, typename Type, typename Compare, std::integral Range, bool IsSmallOptimized, bool IsAllocated>
template<typename Initializer>
    requires std::is_invocable_r_v<Type, Initializer> || std::is_invocable_r_v<Type, Initializer, Range>
inline void kF::Core::Internal::SortedVectorDetails<Base, Type, Compare, Range, IsSmallOptimized, IsAllocated>::resize(const Range count, Initializer &&initializer) noexcept
{
    DetailsBase::resize(count, std::forward<Initializer>(initializer));
    if (count) [[likely]]
        sort();
}

template<typename Base, typename Type, typename Compare, std::integral Range, bool IsSmallOptimized, bool IsAllocated>
inline void kF::Core::Internal::SortedVectorDetails<Base, Type, Compare, Range, IsSmallOptimized, IsAllocated>::sort(void) noexcept
{
    std::sort(DetailsBase::beginUnsafe(), DetailsBase::endUnsafe(), Compare{});
}

template<typename Base, typename Type, typename Compare, std::integral Range, bool IsSmallOptimized, bool IsAllocated>
template<typename AssignType>
inline Range kF::Core::Internal::SortedVectorDetails<Base, Type, Compare, Range, IsSmallOptimized, IsAllocated>::assign(const Range index, AssignType &&value) noexcept
{
    const auto count = DetailsBase::sizeUnsafe();
    auto &elem = DetailsBase::at(index);
    Range finalPos = index;

    elem = std::forward<AssignType>(value);
    if (index > 0 && !Compare{}(DetailsBase::at(index - 1), elem)) {
        finalPos = index - 1;
        for (auto i = index - 2; i > 0; --i) {
            if (!Compare{}(DetailsBase::at(i), elem))
                finalPos = i;
            else
                break;
        }
        std::swap(DetailsBase::at(finalPos), elem);
    } else if (index + 1 < count && !Compare{}(elem, DetailsBase::at(index + 1))) {
        finalPos = index + 1;
        for (auto i = index + 2; i < count; ++i) {
            if (!Compare{}(elem, DetailsBase::at(i))) {
                finalPos = i;
            } else
                break;
        }
        std::swap(DetailsBase::at(finalPos), elem);
    }
    return finalPos;
}