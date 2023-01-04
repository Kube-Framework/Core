/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: String details
 */

#include "StringDetails.hpp"

template<typename Base, typename Type, std::integral Range, bool IsRuntimeAllocated>
    requires std::is_trivial_v<Type>
inline kF::Core::Internal::StringDetails<Base, Type, Range, IsRuntimeAllocated> kF::Core::Internal::StringDetails<Base, Type, Range, IsRuntimeAllocated>::operator+(const StringDetails &other) const noexcept
{
    StringDetails res;
    res.reserve(size() + other.size());
    res.insert(res.end(), begin(), end());
    res.insert(res.end(), other.begin(), other.end());
    return res;
}

template<typename Base, typename Type, std::integral Range, bool IsRuntimeAllocated>
    requires std::is_trivial_v<Type>
inline kF::Core::Internal::StringDetails<Base, Type, Range, IsRuntimeAllocated> kF::Core::Internal::StringDetails<Base, Type, Range, IsRuntimeAllocated>::operator+(const Type character) const noexcept
{
    StringDetails res;
    res.reserve(size() + 1);
    res.insert(res.end(), begin(), end());
    res.push(character);
    return res;
}

template<typename Base, typename Type, std::integral Range, bool IsRuntimeAllocated>
    requires std::is_trivial_v<Type>
inline kF::Core::Internal::StringDetails<Base, Type, Range, IsRuntimeAllocated> kF::Core::Internal::StringDetails<Base, Type, Range, IsRuntimeAllocated>::operator+(const char * const cstring) const noexcept
{
    StringDetails res;
    const auto length = SafeStrlen(cstring);
    res.reserve(size() + length);
    res.insert(res.end(), begin(), end());
    res.insert(res.end(), cstring, cstring + length);
    return res;
}

template<typename Base, typename Type, std::integral Range, bool IsRuntimeAllocated>
    requires std::is_trivial_v<Type>
inline kF::Core::Internal::StringDetails<Base, Type, Range, IsRuntimeAllocated> kF::Core::Internal::StringDetails<Base, Type, Range, IsRuntimeAllocated>::operator+(const std::basic_string_view<Type> &other) const noexcept
{
    StringDetails res;
    res.reserve(size() + static_cast<Range>(other.size()));
    res.insert(res.end(), begin(), end());
    res.insert(res.end(), other.begin(), other.end());
    return res;
}

template<typename Base, typename Type, std::integral Range, bool IsRuntimeAllocated>
    requires std::is_trivial_v<Type>
inline Range kF::Core::Internal::StringDetails<Base, Type, Range, IsRuntimeAllocated>::SafeStrlen(const char * const cstring) noexcept
{
    if (!cstring)
        return Range {};
    else
        return static_cast<Range>(std::strlen(cstring));
}