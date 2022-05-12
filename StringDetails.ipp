/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: String details
 */

template<typename Base, typename Type, std::integral Range, bool IsAllocated>
    requires std::is_trivial_v<Type>
inline kF::Core::Internal::StringDetails<Base, Type, Range, IsAllocated> kF::Core::Internal::StringDetails<Base, Type, Range, IsAllocated>::operator+(const StringDetails &other) noexcept
{
    StringDetails res;
    res.reserve(size() + other.size());
    res.insert(res.end(), begin(), end());
    res.insert(res.end(), other.begin(), other.end());
    return res;
}

template<typename Base, typename Type, std::integral Range, bool IsAllocated>
    requires std::is_trivial_v<Type>
inline kF::Core::Internal::StringDetails<Base, Type, Range, IsAllocated> kF::Core::Internal::StringDetails<Base, Type, Range, IsAllocated>::operator+(const char * const cstring) noexcept
{
    StringDetails res;
    const auto length = SafeStrlen(cstring);
    res.reserve(size() + length);
    res.insert(res.end(), begin(), end());
    res.insert(res.end(), cstring, cstring + length);
    return res;
}

template<typename Base, typename Type, std::integral Range, bool IsAllocated>
    requires std::is_trivial_v<Type>
inline kF::Core::Internal::StringDetails<Base, Type, Range, IsAllocated> kF::Core::Internal::StringDetails<Base, Type, Range, IsAllocated>::operator+(const std::basic_string_view<Type> &other) noexcept
{
    StringDetails res;
    res.reserve(size() + other.size());
    res.insert(res.end(), begin(), end());
    res.insert(res.end(), other.begin(), other.end());
    return res;
}

template<typename Base, typename Type, std::integral Range, bool IsAllocated>
    requires std::is_trivial_v<Type>
inline const char *kF::Core::Internal::StringDetails<Base, Type, Range, IsAllocated>::c_str(void) const noexcept
{
    if (!size())
        return nullptr;
    else if (size() == capacity())
        const_cast<StringDetails *>(this)->grow(1);
    *const_cast<StringDetails *>(this)->end() = '\0';
    return dataUnsafe();
}

template<typename Base, typename Type, std::integral Range, bool IsAllocated>
    requires std::is_trivial_v<Type>
inline std::size_t kF::Core::Internal::StringDetails<Base, Type, Range, IsAllocated>::SafeStrlen(const char * const cstring) noexcept
{
    if (!cstring)
        return 0;
    else
        return std::strlen(cstring);
}