/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: VectorDetails
 */

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsAllocated>
template<typename ...Args> requires std::constructible_from<Type, Args...>
inline Type &kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsAllocated>::push(Args &&...args) noexcept
{
    if (!data())
        reserveUnsafe<false>(static_cast<Range>(2));
    else if (sizeUnsafe() == capacityUnsafe())
        grow();
    const Range currentSize = sizeUnsafe();
    Type * const elem = dataUnsafe() + currentSize;
    setSize(static_cast<Range>(currentSize + static_cast<Range>(1)));
    new (elem) Type(std::forward<Args>(args)...);
    return *elem;
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsAllocated>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsAllocated>::pop(void) noexcept
{
    const auto desiredSize = sizeUnsafe() - 1;

    dataUnsafe()[desiredSize].~Type();
    setSize(desiredSize);
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsAllocated>
inline typename kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsAllocated>::Iterator
    kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsAllocated>::insertDefault(Iterator pos, const Range count) noexcept
{
    if (!count) [[unlikely]]
        return end();
    else if (pos == nullptr) [[unlikely]] {
        resize(count);
        return beginUnsafe();
    }
    const auto currentData = dataUnsafe();
    const auto currentSize = sizeUnsafe();
    auto currentBegin = beginUnsafe();
    auto currentEnd = endUnsafe();
    auto position = static_cast<Range>(std::distance(currentBegin, pos));
    if (const Range currentCapacity = capacityUnsafe(), total = currentSize + count; total > currentCapacity) [[unlikely]] {
        const auto desiredCapacity = currentCapacity + std::max(currentCapacity, count);
        const auto tmpData = allocate(desiredCapacity);
        setData(tmpData);
        setSize(total);
        setCapacity(desiredCapacity);
        if constexpr (IsSmallOptimized) {
            if (tmpData == currentData) {
                if (const auto after = currentSize - position; after > count) {
                    std::uninitialized_move(currentEnd - count, currentEnd, currentEnd);
                    std::move_backward(currentBegin + position, currentEnd - count, currentEnd);
                } else
                    std::uninitialized_move(currentBegin + position, currentEnd, currentEnd + count - after);
                std::uninitialized_default_construct_n(tmpData + position, count);
                return tmpData + position;
            }
        }
        std::uninitialized_move_n(currentBegin, position, tmpData);
        std::uninitialized_move(currentBegin + position, currentEnd, tmpData + position + count);
        std::uninitialized_default_construct_n(tmpData + position, count);
        deallocate(currentData, currentCapacity);
        return tmpData + position;
    } else if (const auto after = currentSize - position; after > count) {
        std::uninitialized_move(currentEnd - count, currentEnd, currentEnd);
        std::move_backward(currentBegin + position, currentEnd - count, currentEnd);
    } else
        std::uninitialized_move(currentBegin + position, currentEnd, currentEnd + count - after);
    std::uninitialized_default_construct_n(currentBegin + position, count);
    setSize(currentSize + count);
    return currentBegin + position;
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsAllocated>
inline typename kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsAllocated>::Iterator
    kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsAllocated>::insertCopy(
        Iterator pos, const Range count, const Type &value) noexcept
{
    if (!count) [[unlikely]]
        return end();
    else if (pos == nullptr) [[unlikely]] {
        resize(count, value);
        return beginUnsafe();
    }
    const auto currentData = dataUnsafe();
    const auto currentSize = sizeUnsafe();
    auto currentBegin = beginUnsafe();
    auto currentEnd = endUnsafe();
    auto position = static_cast<Range>(std::distance(currentBegin, pos));
    if (const Range currentCapacity = capacityUnsafe(), total = currentSize + count; total > currentCapacity) [[unlikely]] {
        const auto desiredCapacity = currentCapacity + std::max(currentCapacity, count);
        const auto tmpData = allocate(desiredCapacity);
        setData(tmpData);
        setSize(total);
        setCapacity(desiredCapacity);
        if constexpr (IsSmallOptimized) {
            if (tmpData == currentData) {
                if (const auto after = currentSize - position; after > count) {
                    std::uninitialized_move(currentEnd - count, currentEnd, currentEnd);
                    std::move_backward(currentBegin + position, currentEnd - count, currentEnd);
                } else
                    std::uninitialized_move(currentBegin + position, currentEnd, currentEnd + count - after);
                std::fill_n(tmpData + position, count, value);
                return tmpData + position;
            }
        }
        std::uninitialized_move_n(currentBegin, position, tmpData);
        std::uninitialized_move(currentBegin + position, currentEnd, tmpData + position + count);
        std::fill_n(tmpData + position, count, value);
        deallocate(currentData, currentCapacity);
        return tmpData + position;
    } else if (const auto after = currentSize - position; after > count) {
        std::uninitialized_move(currentEnd - count, currentEnd, currentEnd);
        std::move_backward(currentBegin + position, currentEnd - count, currentEnd);
    } else
        std::uninitialized_move(currentBegin + position, currentEnd, currentEnd + count - after);
    std::fill_n(currentBegin + position, count, value);
    setSize(currentSize + count);
    return currentBegin + position;
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsAllocated>
template<std::input_iterator InputIterator>
inline typename kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsAllocated>::Iterator
    kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsAllocated>::insert(
        Iterator pos, InputIterator from, InputIterator to) noexcept
{
    const auto count = static_cast<Range>(std::distance(from, to));
    Range position;

    if (!count) [[unlikely]]
        return end();
    else if (pos == Iterator()) [[unlikely]] {
        reserve(count);
        position = 0;
    } else [[likely]]
        position = static_cast<Range>(std::distance(beginUnsafe(), pos));
    const auto currentData = dataUnsafe();
    const auto currentSize = sizeUnsafe();
    auto currentBegin = beginUnsafe();
    auto currentEnd = endUnsafe();
    if (const Range currentCapacity = capacityUnsafe(), total = currentSize + count; total > currentCapacity) [[unlikely]] {
        const auto desiredCapacity = currentCapacity + std::max(currentCapacity, count);
        const auto tmpData = allocate(desiredCapacity);
        setData(tmpData);
        setSize(total);
        setCapacity(desiredCapacity);
        if constexpr (IsSmallOptimized) {
            if (tmpData == currentData) {
                if (const auto after = currentSize - position; after > count) {
                    std::uninitialized_move(currentEnd - count, currentEnd, currentEnd);
                    std::move_backward(currentBegin + position, currentEnd - count, currentEnd);
                } else
                    std::uninitialized_move(currentBegin + position, currentEnd, currentEnd + count - after);
                std::copy(from, to, tmpData + position);
                return tmpData + position;
            }
        }
        std::uninitialized_move_n(currentBegin, position, tmpData);
        std::uninitialized_move(currentBegin + position, currentEnd, tmpData + position + count);
        std::copy(from, to, tmpData + position);
        deallocate(currentData, currentCapacity);
        return tmpData + position;
    } else if (const auto after = currentSize - position; after > count) {
        std::uninitialized_move(currentEnd - count, currentEnd, currentEnd);
        std::move_backward(currentBegin + position, currentEnd - count, currentEnd);
    } else
        std::uninitialized_move(currentBegin + position, currentEnd, currentEnd + count - after);
    std::copy(from, to, currentBegin + position);
    setSize(currentSize + count);
    return currentBegin + position;
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsAllocated>
template<std::input_iterator InputIterator, typename Map>
inline typename kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsAllocated>::Iterator
    kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsAllocated>::insert(
        Iterator pos, InputIterator from, InputIterator to, Map &&map) noexcept
{
    constexpr auto MapCopy = [](InputIterator from, InputIterator to, auto &map, Iterator begin) {
        while (from != to) {
            if constexpr (Utils::IsMoveIterator<InputIterator>::Value)
                new (begin) Type(std::invoke(map, std::move(*from)));
            else
                new (begin) Type(std::invoke(map, *from));
            ++from;
            ++begin;
        }
    };

    const auto count = static_cast<Range>(std::distance(from, to));
    Range position;

    if (!count) [[unlikely]]
        return end();
    else if (pos == Iterator()) [[unlikely]] {
        reserve(count);
        position = 0;
    } else [[likely]]
        position = static_cast<Range>(std::distance(beginUnsafe(), pos));
    const auto currentData = dataUnsafe();
    const auto currentSize = sizeUnsafe();
    auto currentBegin = beginUnsafe();
    auto currentEnd = endUnsafe();
    if (const Range currentCapacity = capacityUnsafe(), total = currentSize + count; total > currentCapacity) [[unlikely]] {
        const auto desiredCapacity = currentCapacity + std::max(currentCapacity, count);
        const auto tmpData = allocate(desiredCapacity);
        setData(tmpData);
        setSize(total);
        setCapacity(desiredCapacity);
        if constexpr (IsSmallOptimized) {
            if (tmpData == currentData) {
                if (const auto after = currentSize - position; after > count) {
                    std::uninitialized_move(currentEnd - count, currentEnd, currentEnd);
                    std::move_backward(currentBegin + position, currentEnd - count, currentEnd);
                } else
                    std::uninitialized_move(currentBegin + position, currentEnd, currentEnd + count - after);
                MapCopy(from, to, map, tmpData + position);
                return tmpData + position;
            }
        }
        std::uninitialized_move_n(currentBegin, position, tmpData);
        std::uninitialized_move(currentBegin + position, currentEnd, tmpData + position + count);
        MapCopy(from, to, map, tmpData + position);
        deallocate(currentData, currentCapacity);
        return tmpData + position;
    } else if (const auto after = currentSize - position; after > count) {
        std::uninitialized_move(currentEnd - count, currentEnd, currentEnd);
        std::move_backward(currentBegin + position, currentEnd - count, currentEnd);
    } else
        std::uninitialized_move(currentBegin + position, currentEnd, currentEnd + count - after);
    MapCopy(from, to, map, currentBegin + position);
    setSize(currentSize + count);
    return currentBegin + position;
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsAllocated>
inline kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsAllocated>::Iterator
    kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsAllocated>::erase(Iterator from, Iterator to) noexcept
{
    if (from == to) [[unlikely]]
        return to;
    const auto end = endUnsafe();
    setSize(sizeUnsafe() - static_cast<Range>(std::distance(from, to)));
    std::destroy(from, to);
    std::uninitialized_move(to, end, from);
    return from;
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsAllocated>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsAllocated>::resizeUninitialized(const Range count) noexcept
{
    if (!data()) {
        reserveUnsafe<false>(count);
    } else {
        clearUnsafe();
        reserveUnsafe<true>(count);
    }
    setSize(count);
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsAllocated>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsAllocated>::resize(const Range count) noexcept
    requires std::constructible_from<Type>
{
    resizeUninitialized(count);
    std::uninitialized_value_construct_n(dataUnsafe(), count);
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsAllocated>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsAllocated>::resize(const Range count, const Type &value) noexcept
    requires std::copy_constructible<Type>
{
    resizeUninitialized(count);
    std::uninitialized_fill_n(dataUnsafe(), count, value);
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsAllocated>
template<typename Initializer>
    requires std::is_invocable_r_v<Type, Initializer> || std::is_invocable_r_v<Type, Initializer, Range>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsAllocated>::resize(const Range count, Initializer &&initializer) noexcept
{
    resizeUninitialized(count);
    auto ptr = beginUnsafe();
    for (Range i = 0; i != count; ++i) {
        if constexpr (std::is_invocable_r_v<Type, Initializer, Range>)
            new (ptr + i) Type(std::invoke(initializer, i));
        else
            new (ptr + i) Type(std::invoke(initializer));
    }
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsAllocated>
template<std::input_iterator InputIterator>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsAllocated>::resize(InputIterator from, InputIterator to) noexcept
{
    const auto count = static_cast<Range>(std::distance(from, to));

    resizeUninitialized(count);
    std::uninitialized_copy(from, to, beginUnsafe());
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsAllocated>
template<std::input_iterator InputIterator, typename Map>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsAllocated>::resize(InputIterator from, InputIterator to, Map &&map) noexcept
{
    const auto count = static_cast<Range>(std::distance(from, to));

    resizeUninitialized(count);
    auto begin = beginUnsafe();
    while (from != to) {
        if constexpr (Utils::IsMoveIterator<InputIterator>::Value)
            new (begin) Type(std::invoke(map, std::move(*from)));
        else
            new (begin) Type(std::invoke(map, *from));
        ++from;
        ++begin;
    }
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsAllocated>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsAllocated>::clear(void) noexcept
{
    if (data()) [[likely]]
        clearUnsafe();
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsAllocated>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsAllocated>::clearUnsafe(void) noexcept
{
    std::destroy_n(dataUnsafe(), sizeUnsafe());
    setSize(0);
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsAllocated>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsAllocated>::release(void) noexcept
{
    if (data()) [[likely]]
        releaseUnsafe();
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsAllocated>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsAllocated>::releaseUnsafe(void) noexcept
{
    const auto currentData = dataUnsafe();
    const Range currentCapacity = capacityUnsafe();

    clearUnsafe();
    setCapacity(0);
    setData(nullptr);
    deallocate(currentData, currentCapacity);
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsAllocated>
inline bool kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsAllocated>::reserve(const Range capacity) noexcept
{
    if (data())
        return reserveUnsafe<true>(capacity);
    else
        return reserveUnsafe<false>(capacity);
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsAllocated>
template<bool IsSafe>
inline bool kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsAllocated>::reserveUnsafe(const Range capacity) noexcept
{
    if constexpr (IsSafe) {
        const Range currentCapacity = capacityUnsafe();
        if (currentCapacity >= capacity) [[unlikely]]
            return false;
        const auto currentSize = sizeUnsafe();
        const auto currentData = dataUnsafe();
        const auto tmpData = allocate(capacity);
        setData(tmpData);
        setSize(currentSize);
        setCapacity(capacity);
        if constexpr (IsSmallOptimized) {
            if (tmpData == currentData)
                return false;
        }
        std::uninitialized_move_n(currentData, currentSize, tmpData);
        std::destroy_n(currentData, currentSize);
        deallocate(currentData, currentCapacity);
        return true;
    } else {
        if (capacity == 0)
            return false;
        setData(allocate(capacity));
        setSize(0);
        setCapacity(capacity);
        return true;
    }
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsAllocated>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsAllocated>::grow(const Range minimum) noexcept
{
    const auto currentData = dataUnsafe();
    const Range currentSize = sizeUnsafe();
    const Range currentCapacity = capacityUnsafe();
    const Range desiredCapacity = static_cast<Range>(currentCapacity + static_cast<Range>(std::max(currentCapacity, minimum)));
    const auto tmpData = allocate(desiredCapacity);



    setData(tmpData);
    setSize(currentSize);
    setCapacity(desiredCapacity);
    if constexpr (IsSmallOptimized) {
        if (tmpData == currentData)
            return;
    }
    std::uninitialized_move_n(currentData, currentSize, tmpData);
    std::destroy_n(currentData, currentSize);
    deallocate(currentData, currentCapacity);
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsAllocated>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsAllocated>::move(Range from, Range to, Range output) noexcept
{
    kFAssert(output < from || output > to,
        "VectorDetails::move: Invalid move range [", from, ", ", to, "] -> ", output);
    ++to;
    if (output < from) {
        const auto tmp = from;
        from = output;
        output = to;
        to = tmp;
    } else if (output)
        ++output;
    const auto it = beginUnsafe();
    std::rotate(it + from, it + to, it + output);
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized, bool IsAllocated>
inline bool kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized, IsAllocated>::operator==(const VectorDetails &other) const noexcept
    requires std::equality_comparable<Type>
{
    const auto count = size();
    const auto otherCount = other.size();

    if (count == otherCount) [[unlikely]] {
        if (count) [[likely]]
            return std::equal(beginUnsafe(), endUnsafe(), other.beginUnsafe());
        else [[unlikely]]
            return true;
    } else [[likely]]
        return false;
}