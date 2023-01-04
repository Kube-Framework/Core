/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Version structure
 */

#pragma once

#include "Utils.hpp"

namespace kF::Core
{
    /** @brief Version structure */
    struct Version
    {
        std::uint8_t major {};
        std::uint8_t minor {};
        std::uint8_t patch {};

        /** @brief Comparison operators */
        [[nodiscard]] constexpr bool operator==(const Version &other) const noexcept = default;
        [[nodiscard]] constexpr bool operator!=(const Version &other) const noexcept = default;
    };

    constexpr Version KubeVersion {
        .major = 0,
        .minor = 1,
        .patch = 0
    };
}