/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: A set of platform compile-time data
 */

#pragma once

#include <cstddef>

// Detect the platform
#if defined(__APPLE__)
# define KUBE_PLATFORM_APPLE    1
# define KUBE_PLATFORM_LINUX    0
# define KUBE_PLATFORM_WINDOWS  0
#elif defined(__linux__)
# define KUBE_PLATFORM_APPLE    0
# define KUBE_PLATFORM_LINUX    1
# define KUBE_PLATFORM_WINDOWS  0
#elif defined(_WIN32)
# define KUBE_PLATFORM_APPLE    0
# define KUBE_PLATFORM_LINUX    0
# define KUBE_PLATFORM_WINDOWS  1
#else
# define KUBE_PLATFORM_APPLE    0
# define KUBE_PLATFORM_LINUX    0
# define KUBE_PLATFORM_WINDOWS  0
#endif

static_assert(KUBE_PLATFORM_LINUX || KUBE_PLATFORM_WINDOWS || KUBE_PLATFORM_APPLE, "Platform not detected");


// Detect the compiler
#if defined(__clang__)
# define KUBE_COMPILER_CLANG    1
# define KUBE_COMPILER_GCC      0
# define KUBE_COMPILER_MSVC     0
#elif defined(__GNUC__) || defined(__GNUG__)
# define KUBE_COMPILER_CLANG    0
# define KUBE_COMPILER_GCC      1
# define KUBE_COMPILER_MSVC     0
#elif defined(_MSC_VER)
# define KUBE_COMPILER_CLANG    0
# define KUBE_COMPILER_GCC      0
# define KUBE_COMPILER_MSVC     1
#else
# define KUBE_COMPILER_INTEL    0
# define KUBE_COMPILER_CLANG    0
# define KUBE_COMPILER_GNU      0
# define KUBE_COMPILER_MSVC     0
#endif

static_assert(KUBE_COMPILER_CLANG || KUBE_COMPILER_GCC || KUBE_COMPILER_MSVC, "Compiler not detected");

namespace kF::Core::Platform
{
    /** @brief Get the system page size */
    [[nodiscard]] std::size_t GetPageSize(void) noexcept;
}