/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: FunctorUtils
 */

#pragma once

#include <cstddef>

namespace kF::Core::Utils
{
    /** @brief Types of functor implementations */
    enum class FunctorType
    {
        Error,
        Trivial,
        Allocated
    };

    /** @brief Functor type detector, Error case */
    template<typename Type>
    struct FunctorTypeDetector
    {
        static constexpr FunctorType Type = FunctorType::Error;
    };

    /** @brief Functor type detector, Trivial case */
    template<template<typename, std::size_t> typename Type, typename Arg, std::size_t CacheSize>
    struct FunctorTypeDetector<Type<Arg, CacheSize>>
    {
        static constexpr FunctorType Type = FunctorType::Trivial;
    };

    /** @brief Functor type detector, Allocated case */
    template<template<typename, typename, std::size_t> typename Type, typename Arg, typename Allocator, std::size_t CacheSize>
    struct FunctorTypeDetector<Type<Arg, Allocator, CacheSize>>
    {
        static constexpr FunctorType Type = FunctorType::Allocated;
    };

    /** @brief Concept of a dispatcher functor */
    template<typename Type>
    concept FunctorRequirements = FunctorTypeDetector<Type>::Type != FunctorType::Error;
}