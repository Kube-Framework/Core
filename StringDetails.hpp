/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: StringDetails
 */

#pragma once

#include <string_view>
#include <cstring>

#include "Utils.hpp"

namespace kF::Core
{
    class IAllocator;

    namespace Internal
    {
        template<typename Base, typename Type, std::integral Range, bool IsAllocated>
            requires std::is_trivial_v<Type>
        class StringDetails;
    }
}

/** @brief String details bring facilities to manipulate a vector as a string */
template<typename Base, typename Type, std::integral Range, bool IsAllocated>
    requires std::is_trivial_v<Type>
class kF::Core::Internal::StringDetails : public Base
{
public:
    /** @brief View */
    using View = std::basic_string_view<Type>;

    using Base::Base;
    using Base::data;
    using Base::dataUnsafe;
    using Base::size;
    using Base::sizeUnsafe;
    using Base::capacity;
    using Base::capacityUnsafe;
    using Base::begin;
    using Base::end;
    using Base::resize;
    using Base::insert;
    using Base::isSafe;
    using Base::reserve;
    using Base::grow;
    using Base::operator bool;


    /** @brief Destructor */
    inline ~StringDetails(void) noexcept = default;


    /** @brief Default constructor */
    inline StringDetails(void) noexcept
            requires (!IsAllocated) = default;

    /** @brief Default constructor - Allocated version*/
    inline StringDetails(IAllocator &allocator) noexcept
            requires (IsAllocated)
        : Base(allocator) {}


    /** @brief Copy constructor */
    inline StringDetails(const StringDetails &other) noexcept = default;

    /** @brief Move constructor */
    inline StringDetails(StringDetails &&other) noexcept = default;


    /** @brief CString constructor */
    inline StringDetails(const char * const cstring) noexcept
            requires (!IsAllocated)
        { resize(cstring, cstring + SafeStrlen(cstring)); }

    /** @brief CString constructor - Allocated version */
    inline StringDetails(IAllocator &allocator, const char * const cstring) noexcept
            requires (IsAllocated)
        : Base(allocator) { resize(cstring, cstring + SafeStrlen(cstring)); }


    /** @brief CString length constructor */
    inline StringDetails(const char * const cstring, const std::size_t length) noexcept
            requires (!IsAllocated)
        { resize(cstring, cstring + length); }

    /** @brief CString length constructor - Allocated version */
    inline StringDetails(IAllocator &allocator, const char * const cstring, const std::size_t length) noexcept
            requires (IsAllocated)
        : Base(allocator) { resize(cstring, cstring + length); }


    /** @brief View constructor */
    inline StringDetails(const View &other) noexcept
            requires (!IsAllocated)
        { resize(other.begin(), other.end()); }

    /** @brief View constructor - Allocated version */
    inline StringDetails(IAllocator &allocator, const View &other) noexcept
            requires (IsAllocated)
        : Base(allocator) { resize(other.begin(), other.end()); }


    /** @brief Copy assignment */
    inline StringDetails &operator=(const StringDetails &other) noexcept = default;

    /** @brief Move assignment */
    inline StringDetails &operator=(StringDetails &&other) noexcept = default;

    /** @brief cstring assignment */
    inline StringDetails &operator=(const char * const cstring) noexcept { resize(cstring, cstring + SafeStrlen(cstring)); return *this; }

    /** @brief View assignment */
    inline StringDetails &operator=(const View &other) noexcept { resize(other.begin(), other.end()); return *this; }


    /** @brief Get string length (alias of 'size' function) */
    [[nodiscard]] Range length(void) const noexcept { return size(); }
    [[nodiscard]] Range lengthUnsafe(void) const noexcept { return sizeUnsafe(); }


    /** @brief Comparison operator */
    [[nodiscard]] inline bool operator==(const StringDetails &other) const noexcept { return std::equal(begin(), end(), other.begin(), other.end()); }
    [[nodiscard]] inline bool operator!=(const StringDetails &other) const noexcept { return !operator==(other); }
    [[nodiscard]] inline bool operator==(const char * const cstring) const noexcept { return std::equal(begin(), end(), cstring, cstring + SafeStrlen(cstring)); }
    [[nodiscard]] inline bool operator!=(const char * const cstring) const noexcept { return !operator==(cstring); }
    [[nodiscard]] inline bool operator==(const View &other) const noexcept { return std::equal(begin(), end(), other.begin(), other.end()); }
    [[nodiscard]] inline bool operator!=(const View &other) const noexcept { return !operator==(other); }


    /** @brief Append operator */
    inline StringDetails &operator+=(const StringDetails &other) noexcept { insert(end(), other.begin(), other.end()); return *this; }
    inline StringDetails &operator+=(const char * const cstring) noexcept { insert(end(), cstring, cstring + SafeStrlen(cstring)); return *this; }
    inline StringDetails &operator+=(const View &other) noexcept { insert(end(), other.begin(), other.end()); return *this; }


    /** @brief Addition operator */
    [[nodiscard]] inline StringDetails operator+(const StringDetails &other) noexcept;
    [[nodiscard]] inline StringDetails operator+(const char * const cstring) noexcept;
    [[nodiscard]] inline StringDetails operator+(const View &other) noexcept;


    /** @brief Get a std::string from the object */
    [[nodiscard]] inline View toView(void) const noexcept { return isSafe() ? View(data(), sizeUnsafe()) : View(); }

    /** @brief Get a null terminated char array pointer
     *  ! Be careful as the function is constant for convinience but it can still modify the internal pointer ! */
    [[nodiscard]] const char *c_str(void) const noexcept
    { // MSVC Throws an internal error when compiling this inside 'ipp'
        if (!size())
            return nullptr;
        else if (size() == capacity())
            const_cast<StringDetails *>(this)->grow(1);
        *const_cast<StringDetails *>(this)->end() = '\0';
        return dataUnsafe();
    }

private:
    /** @brief Strlen but with null cstring check */
    [[nodiscard]] static std::size_t SafeStrlen(const char * const cstring) noexcept;
};

#include "StringDetails.ipp"
