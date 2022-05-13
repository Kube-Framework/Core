/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: FunctorUtils
 */

#pragma once

#include <cstddef>

namespace kF::Core
{
    /** @brief Types of functor implementations */
    enum class FunctorType
    {
        Error,
        Trivial,
        Allocated
    };

    /** @brief Functor type detector, Error case */
    template<typename ObjectType>
    struct FunctorTypeDetector
    {
        static constexpr FunctorType Type = FunctorType::Error;
    };

    /** @brief Functor type detector, Trivial case */
    template<template<typename, std::size_t> typename ObjectType, typename Arg, std::size_t CacheSize>
    struct FunctorTypeDetector<ObjectType<Arg, CacheSize>>
    {
        static constexpr FunctorType Type = FunctorType::Trivial;
    };

    /** @brief Functor type detector, Allocated case */
    template<template<typename, typename, std::size_t> typename ObjectType, typename Arg, typename Allocator, std::size_t CacheSize>
    struct FunctorTypeDetector<ObjectType<Arg, Allocator, CacheSize>>
    {
        static constexpr FunctorType Type = FunctorType::Allocated;
    };

    /** @brief Concept of a dispatcher functor */
    template<typename ObjectType>
    concept FunctorRequirements = FunctorTypeDetector<ObjectType>::Type != FunctorType::Error;
}