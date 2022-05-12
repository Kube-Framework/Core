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
    };

    constexpr Version KubeVersion {
        .major = 0,
        .minor = 1,
        .patch = 0
    };
}