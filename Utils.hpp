/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: A set of utility used globally in the framework
 */

#pragma once

#include <type_traits>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <utility>
#include <cstdlib>

#include "Platform.hpp"

/** @brief Helper used to pass template into macro */
#define TEMPLATE_TYPE(Class, ...) decltype(std::declval<Class<__VA_ARGS__>>())


/** @brief Align a variable / structure to cacheline size */
#define alignas_cacheline alignas(kF::Core::CacheLineSize)
#define alignas_double_cacheline alignas(kF::Core::CacheLineDoubleSize)
#define alignas_half_cacheline alignas(kF::Core::CacheLineHalfSize)
#define alignas_quarter_cacheline alignas(kF::Core::CacheLineQuarterSize)
#define alignas_eighth_cacheline alignas(kF::Core::CacheLineEighthSize)


/** @brief Helpers used to assert alignment of a structure */
#define static_assert_alignof(Type, Alignment) static_assert(alignof(Type) == Alignment, #Type " must be aligned to " #Alignment)
#define static_assert_alignof_cacheline(Type) static_assert_alignof(Type, kF::Core::CacheLineSize)
#define static_assert_alignof_double_cacheline(Type) static_assert_alignof(Type, kF::Core::CacheLineDoubleSize)
#define static_assert_alignof_half_cacheline(Type) static_assert_alignof(Type, kF::Core::CacheLineHalfSize)
#define static_assert_alignof_quarter_cacheline(Type) static_assert_alignof(Type, kF::Core::CacheLineQuarterSize)
#define static_assert_alignof_eighth_cacheline(Type) static_assert_alignof(Type, kF::Core::CacheLineEighthSize)

/** @brief Helpers used to assert size of a structure */
#define static_assert_sizeof(Type, Size) static_assert(sizeof(Type) == Size, #Type " must have a size of " #Size)
#define static_assert_sizeof_cacheline(Type) static_assert_sizeof(Type, kF::Core::CacheLineSize)
#define static_assert_sizeof_double_cacheline(Type) static_assert_sizeof(Type, kF::Core::CacheLineDoubleSize)
#define static_assert_sizeof_half_cacheline(Type) static_assert_sizeof(Type, kF::Core::CacheLineHalfSize)
#define static_assert_sizeof_quarter_cacheline(Type) static_assert_sizeof(Type, kF::Core::CacheLineQuarterSize)
#define static_assert_sizeof_eighth_cacheline(Type) static_assert_sizeof(Type, kF::Core::CacheLineEighthSize)

/** @brief Helpers used to assert that the size and alignment of a structure are equal to themselves and a given value */
#define static_assert_fit(Type, Size) static_assert(sizeof(Type) == alignof(Type) && alignof(Type) == Size, #Type " must have a size of " #Size " and be aligned to " #Size)
#define static_assert_fit_cacheline(Type) static_assert_fit(Type, kF::Core::CacheLineSize)
#define static_assert_fit_double_cacheline(Type) static_assert_fit(Type, kF::Core::CacheLineDoubleSize)
#define static_assert_fit_half_cacheline(Type) static_assert_fit(Type, kF::Core::CacheLineHalfSize)
#define static_assert_fit_quarter_cacheline(Type) static_assert_fit(Type, kF::Core::CacheLineQuarterSize)
#define static_assert_fit_eighth_cacheline(Type) static_assert_fit(Type, kF::Core::CacheLineEighthSize)


/** @brief Compile-time ternary expression */
#define ConstexprTernary(condition, body, elseBody) [] { if constexpr (condition) { return body; } else { return elseBody; } }()

/** @brief Compile-time ternary expression with runtime reference capture */
#define ConstexprTernaryRef(condition, body, elseBody) [&] { if constexpr (condition) { return body; } else { return elseBody; } }()

/** @brief Compile-time ternary expression with runtime copy capture */
#define ConstexprTernaryCopy(condition, body, elseBody) [=] { if constexpr (condition) { return body; } else { return elseBody; } }()


/** @brief Always inline hint (works only on GCC) */
#if KUBE_COMPILER_GCC
# define force_inline inline __attribute__((always_inline))
# define no_inline __attribute__((noinline))
#else
# define force_inline inline
# define no_inline __declspec(noinline)
#endif

namespace kF::Core
{
    /** @brief Theorical cacheline size */
    constexpr std::size_t CacheLineSize = sizeof(std::size_t) * 8;
    constexpr std::size_t CacheLineDoubleSize = CacheLineSize * 2;
    constexpr std::size_t CacheLineHalfSize = CacheLineSize / 2;
    constexpr std::size_t CacheLineQuarterSize = CacheLineSize / 4;
    constexpr std::size_t CacheLineEighthSize = CacheLineSize / 8;

    /** @brief Similar to std::aligned_alloc, but ensure arguments, you must use AlignedFree to free the memory */
    template<typename Cast = void>
    [[nodiscard]] inline Cast *AlignedAlloc(const std::size_t bytes, const std::size_t alignment) noexcept
        { return reinterpret_cast<Cast *>(::operator new(bytes, static_cast<std::align_val_t>(alignment), std::nothrow)); }

    /** @brief Free a pointer allocated with AlignedAlloc */
    inline void AlignedFree(void * const data, const std::size_t bytes, const std::size_t alignment) noexcept
        { ::operator delete(data, bytes, static_cast<std::align_val_t>(alignment)); }


    /** @brief Concept of a static allocator */
    template<typename Type>
    concept StaticAllocatorRequirements = requires(void *data, std::size_t bytes, std::size_t alignment)
    {
        { Type::Allocate(bytes, alignment) } -> std::same_as<void *>;
        { Type::Deallocate(data, bytes, alignment) } -> std::same_as<void>;
    };

    /** @brief Default static allocator */
    struct DefaultStaticAllocator
    {
        /** @brief Allocate function that forward to AlignedAlloc */
        [[nodiscard]] static inline void *Allocate(const std::size_t bytes, const std::size_t alignment) noexcept
            { return AlignedAlloc(bytes, alignment); }

        /** @brief Deallocate function that forward to AlignedFree */
        static inline void Deallocate(void * const data, const std::size_t bytes, const std::size_t alignment) noexcept
            { return AlignedFree(data, bytes, alignment); }
    };
    static_assert(StaticAllocatorRequirements<DefaultStaticAllocator>, "Default static allocator doesn't meet requirements");

    /** @brief Simple pair of random access begin / end iterators */
    template<std::random_access_iterator Iterator>
    struct IteratorRange
    {
        /** @brief Reverse iterator */
        using ReverseIterator = std::reverse_iterator<Iterator>;

        /** @brief Underlying type */
        using Type = std::remove_cvref_t<decltype(*std::declval<Iterator>())>;

        Iterator from {};
        Iterator to {};

        /** @brief Empty check */
        [[nodiscard]] inline bool empty(void) const noexcept { return from == to; }

        /** @brief Begin / End iterators */
        [[nodiscard]] inline Iterator begin(void) const noexcept { return from; }
        [[nodiscard]] inline Iterator end(void) const noexcept { return to; }

        /** @brief Reverse Begin / End iterators */
        [[nodiscard]] inline ReverseIterator rbegin(void) const noexcept { return std::make_reverse_iterator(to); }
        [[nodiscard]] inline ReverseIterator rend(void) const noexcept { return std::make_reverse_iterator(from); }

        /** @brief Range size */
        template<std::integral Range = std::size_t>
        [[nodiscard]] inline Range size(void) const noexcept { return static_cast<Range>(std::distance(from, to)); }

        /** @brief Dereference element at */
        template<std::integral Range = std::size_t>
        [[nodiscard]] inline auto &operator[](const Range index) const noexcept { return from[index]; }

        /** @brief Dereference element at */
        template<std::integral Range = std::size_t>
        [[nodiscard]] inline auto &at(const Range index) const noexcept { return from[index]; }

        /** @brief Get first element */
        [[nodiscard]] inline Type &front(void) noexcept { return at(0); }
        [[nodiscard]] inline const Type &front(void) const noexcept { return at(0); }

        /** @brief Get last element */
        [[nodiscard]] inline Type &back(void) noexcept { return at(size() - 1); }
        [[nodiscard]] inline const Type &back(void) const noexcept { return at(size() - 1); }
    };

    /** @brief Helper to know if a given type is a std::move_iterator */
    template<typename Type>
    struct IsMoveIterator;

    /** @brief Helper that match non-move iterators */
    template<typename Type>
    struct IsMoveIterator
    {
        static constexpr bool Value = false;
    };

    /** @brief Helper that match move iterators */
    template<typename Iterator>
    struct IsMoveIterator<std::move_iterator<Iterator>> : public IsMoveIterator<Iterator>
    {
        static constexpr bool Value = true;
    };

    /** @brief Helper that match reverse iterators */
    template<typename Iterator>
    struct IsMoveIterator<std::reverse_iterator<Iterator>> : public IsMoveIterator<Iterator>
    {};


    namespace Internal
    {
        /** @brief Helper to know if a given type is dereferencable - Failure case */
        template <typename>
        std::false_type Dereferencable(unsigned long) noexcept;

        /** @brief Helper to know if a given type is dereferencable - Success case */
        template <typename Type>
        auto Dereferencable(int) noexcept -> decltype(*std::declval<Type>(), std::true_type {});
    }

    /** @brief Helper to know if a given type is dereferencable */
    template<typename Type>
    constexpr bool IsDereferencable = decltype(Internal::Dereferencable<Type>(0))::value;


    /** @brief Align any offset to a specific alignment */
    template<std::integral Range>
    [[nodiscard]] constexpr Range AlignOffset(const Range offset, const Range alignment) noexcept;

    /** @brief Check if value is a power of 2 */
    template<std::integral Unit>
    [[nodiscard]] constexpr Unit IsPowerOf2(Unit value) noexcept;

    /** @brief Find the closest power of 2 of value */
    template<std::integral Unit>
    [[nodiscard]] constexpr Unit NextPowerOf2(Unit value) noexcept;

    /** @brief Get closest power of 2 of value as bit position */
    template<std::integral Unit, std::integral ResultUnit = std::size_t>
    [[nodiscard]] constexpr ResultUnit NextPowerOf2Bit(Unit value) noexcept;


    /** @brief Enum class flag maker */
    template<typename Type, typename ...Args>
        requires (std::conjunction_v<std::is_same<Type, Args>...>)
    [[nodiscard]] constexpr Type MakeFlags(const Type first, Args ...args) noexcept
    {
        using Underlying = std::underlying_type_t<Type>;
        return static_cast<Type>(((static_cast<Underlying>(first) | static_cast<Underlying>(args)) | ...));
    }

    /** @brief Enum class flag maker */
    template<typename Type>
    [[nodiscard]] constexpr Type MakeFlags(const Type flag) noexcept { return flag; }

    /** @brief Enum class flag remover */
    template<typename Type, typename ...Args>
        requires (sizeof...(Args) != 0 && std::conjunction_v<std::is_same<Type, Args>...>)
    [[nodiscard]] constexpr Type RemoveFlags(const Type flags, Args ...toRemove) noexcept
    {
        using Underlying = std::underlying_type_t<Type>;
        return static_cast<Type>((static_cast<Underlying>(flags) & ~static_cast<Underlying>(MakeFlags(toRemove...))));
    }

    /** @brief Enum class flag tester, all flags must be set */
    template<typename Type, typename ...Args>
        requires (sizeof...(Args) != 0 && std::conjunction_v<std::is_same<Type, Args>...>)
    [[nodiscard]] constexpr bool HasFlags(const Type value, Args ...args) noexcept
    {
        using Underlying = std::underlying_type_t<Type>;
        return ((static_cast<Underlying>(value) & static_cast<Underlying>(args)) && ...);
    }

    /** @brief Enum class flag tester, at least one of the variadic flags must be set */
    template<typename Type, typename ...Args>
        requires (sizeof...(Args) != 0 && std::conjunction_v<std::is_same<Type, Args>...>)
    [[nodiscard]] constexpr bool HasAnyFlags(const Type value, Args ...args) noexcept
    {
        using Underlying = std::underlying_type_t<Type>;
        return static_cast<Underlying>(value) & (static_cast<Underlying>(args) | ...);
    }

    /** @brief Get underlying value of an enumeration */
    template<typename Type>
    [[nodiscard]] constexpr auto ToUnderlying(const Type value) noexcept
        { return static_cast<std::underlying_type_t<Type>>(value); }


    /** @brief Guaranteed branchless compute (boolean condition)
     *  @todo Benchmark */
    template<typename Type>
        requires std::integral<Type> || std::floating_point<Type>
    [[nodiscard]] constexpr Type BranchlessIf(const bool condition, const Type lhs, const Type rhs) noexcept
        { return BranchlessIf(static_cast<Type>(condition), lhs, rhs); }

    /** @brief Guaranteed branchless compute (type condition) */
    template<typename Type>
        requires std::integral<Type> || std::floating_point<Type>
    [[nodiscard]] constexpr Type BranchlessIf(const Type condition, const Type lhs, const Type rhs) noexcept
        { return condition * lhs - rhs * (condition - static_cast<Type>(1)); }
}

#include "Utils.ipp"