#pragma once

#include "Utils.hpp"

namespace kF::Core::Unicode
{
    /** @brief Get the length on an unicode string from an utf8 one */
    template<typename Range = std::size_t>
    [[nodiscard]] constexpr Range Length(const std::string_view &utf8) noexcept;

    /** @brief Get the byte count of the next unicode character */
    [[nodiscard]] constexpr std::uint32_t GetNextCharByteCount(const char * const it, const char * const end) noexcept;

    /** @brief Get the byte count of the previous unicode character */
    [[nodiscard]] constexpr std::uint32_t GetPreviousCharByteCount(const char * const it, const char * const begin) noexcept;

    /** @brief Get the next unicode character in utf8 string */
    [[nodiscard]] constexpr std::uint32_t GetNextChar(const char *&from, const char * const end) noexcept;

    /** @brief Get the previous unicode character in utf8 string */
    [[nodiscard]] constexpr std::uint32_t GetPreviousChar(const char *&from, const char * const begin) noexcept;

    /** @brief Decode an utf8 character */
    [[nodiscard]] constexpr std::uint32_t Decode(const char * const data, const std::uint32_t byteCount) noexcept;
}

#include "Unicode.ipp"