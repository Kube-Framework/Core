/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Functor
 */

#pragma once

#include <cstring>

#include "Utils.hpp"

namespace kF::Core
{
    template<typename Signature, StaticAllocatorRequirements Allocator = DefaultStaticAllocator, std::size_t CacheSize = CacheLineQuarterSize>
    class Functor;

    namespace Internal
    {
        /** @brief Ensure that a given functor met the trivial requirements of Functor */
        template<typename Functor, std::size_t CacheSize>
        concept FunctorCacheRequirements = std::is_trivially_copyable_v<Functor> && sizeof(Functor) <= CacheSize;

        /** @brief Ensure that a given functor DOES NOT met the trivial requirements of Functor */
        template<typename Functor, std::size_t CacheSize>
        concept FunctorNoCacheRequirements = (std::is_trivially_copyable_v<Functor> ? sizeof(Functor) > CacheSize : true);

        /** @brief Ensure that a given functor / function is callable */
        template<typename Functor, typename Return, typename ...Args>
        concept FunctorInvocable = std::is_invocable_r_v<Return, Functor, Args...>;

        /** @brief Ensure that a given member function is callable */
        template<auto Member, typename ClassType, typename Return, typename ...Args>
        concept FunctorMemberInvocable = std::is_invocable_r_v<Return, decltype(Member), ClassType &, Args...>;
    }
}

/** @brief Very fast opaque functor but only takes trivial types less or equal sized than cacheline eighth */
template<typename Return, typename ...Args, kF::Core::StaticAllocatorRequirements Allocator, std::size_t CacheSize>
class kF::Core::Functor<Return(Args...), Allocator, CacheSize>
{
public:
    static_assert(CacheSize >= CacheLineQuarterSize, "Functor's cache size must be at least of a quarter cacheline");

    /** @brief Byte cache */
    using Cache = std::byte[CacheSize];

    /** @brief Functor signature */
    using OpaqueInvoke = Return(*)(Cache &cache, Args...args);
    using OpaqueDestructor = void(*)(Cache &cache, const bool deallocate);

    /** @brief Structure describing a runtime allocation inside the functor */
    struct alignas_quarter_cacheline RuntimeAllocation
    {
        void *ptr { nullptr };
        std::uint32_t size { 0u };
        std::uint32_t capacity { 0u };
    };

    /** @brief Cast a cache into a given type */
    template<typename As>
    [[nodiscard]] static inline As &CacheAs(Cache &cache) noexcept
        { return reinterpret_cast<As &>(cache); }

    /** @brief Cast a cache into an allocation capacity */
    [[nodiscard]] static inline std::size_t &CacheCapacity(Cache &cache) noexcept
        { return *reinterpret_cast<std::size_t *>(&cache + sizeof(void *)); }


    /** @brief Destructor */
    inline ~Functor(void) noexcept { release<false>(); }

    /** @brief Default constructor */
    inline Functor(void) noexcept = default;

    /** @brief Move constructor */
    inline Functor(Functor &&other) noexcept
    {
        std::memcpy(this, &other, sizeof(Functor));
        other._invoke = nullptr;
        other._destruct = nullptr;
    }

    /** @brief Prepare constructor, limited to runtime functors due to template constructor restrictions */
    template<typename ClassFunctor>
        requires (!std::is_same_v<Functor, std::remove_cvref_t<ClassFunctor>>
            && Internal::FunctorInvocable<ClassFunctor, Return, Args...>)
    inline Functor(ClassFunctor &&functor) noexcept
        { prepare(std::forward<ClassFunctor>(functor)); }


    /** @brief Move assignment*/
    inline Functor &operator=(Functor &&other) noexcept
    {
        release<false>();
        std::memcpy(this, &other, sizeof(Functor));
        other._invoke = nullptr;
        other._destruct = nullptr;
        return *this;
    }

    /** @brief Destroy the holded instance if it isn't trivial, without freeing memory */
    template<bool IsDestructSafe = false>
    inline void destroy(void) noexcept
    {
        _invoke = nullptr;
        if constexpr (!IsDestructSafe) {
            if (!_destruct)
                return;
        }
        _destruct(_cache, false); // Deallocate set to false so we expect no deallocation
        if (!CacheAs<RuntimeAllocation>(_cache).ptr) [[unlikely]] // Check performed for custom deleters
            _destruct = nullptr;
    }

    /** @brief Destroy the functor and free its allocated memory
     *  Note that releasing without reseting members is not safe ! */
    template<bool ResetMembers = true>
    inline void release(void) noexcept
    {
        if (_destruct)
            _destruct(_cache, true);
        if constexpr (ResetMembers) {
            _invoke = nullptr;
            _destruct = nullptr;
        }
    }

    /** @brief Check if the functor is prepared */
    [[nodiscard]] inline operator bool(void) const noexcept { return _invoke; }


    /** @brief Prepare a trivial functor */
    template<typename ClassFunctor>
        requires (!std::is_same_v<Functor, std::remove_cvref_t<ClassFunctor>>
            && Internal::FunctorCacheRequirements<ClassFunctor, CacheSize>
            && Internal::FunctorInvocable<ClassFunctor, Return, Args...>)
    inline void prepare(ClassFunctor &&functor) noexcept
    {
        using FlatClassFunctor = std::remove_cvref_t<ClassFunctor>;

        release<false>();
        _invoke = [](Cache &cache, Args ...args) -> Return {
            if constexpr (std::is_same_v<Return, void>)
                CacheAs<ClassFunctor>(cache)(std::forward<Args>(args)...);
            else
                return CacheAs<ClassFunctor>(cache)(std::forward<Args>(args)...);
        };
        _destruct = nullptr;
        new (&_cache) FlatClassFunctor(std::forward<ClassFunctor>(functor));
    }

    /** @brief Prepare a non-trivial functor with an allocator */
    template<typename ClassFunctor>
        requires (!std::is_same_v<Functor, std::remove_cvref_t<ClassFunctor>>
            && Internal::FunctorNoCacheRequirements<ClassFunctor, CacheSize>
            && Internal::FunctorInvocable<ClassFunctor, Return, Args...>)
    inline void prepare(ClassFunctor &&functor) noexcept
    {
        using FlatClassFunctor = std::remove_cvref_t<ClassFunctor>;
        using ClassFunctorPtr = FlatClassFunctor *;

        auto &runtime = CacheAs<RuntimeAllocation>(_cache);

        if (_destruct) {
            if (runtime.capacity < sizeof(FlatClassFunctor)) [[unlikely]] {
                release<false>();
                runtime.ptr = Allocator::Allocate(sizeof(FlatClassFunctor), alignof(FlatClassFunctor));
            } else
                destroy<true>();
        } else {
            runtime.ptr = Allocator::Allocate(sizeof(FlatClassFunctor), alignof(FlatClassFunctor));
            runtime.capacity = sizeof(FlatClassFunctor);
        }
        runtime.size = sizeof(FlatClassFunctor);
        new (runtime.ptr) FlatClassFunctor(std::forward<FlatClassFunctor>(functor));
        _invoke = [](Cache &cache, Args ...args) -> Return {
            if constexpr (std::is_same_v<Return, void>)
                (*reinterpret_cast<ClassFunctorPtr &>(CacheAs<RuntimeAllocation>(cache).ptr))(std::forward<Args>(args)...);
            else
                return (*reinterpret_cast<ClassFunctorPtr &>(CacheAs<RuntimeAllocation>(cache).ptr))(std::forward<Args>(args)...);
        };
        _destruct = [](Cache &cache, const bool deallocate) {
            auto &runtime = CacheAs<RuntimeAllocation>(cache);
            if (runtime.size) {
                runtime.size = 0u;
                reinterpret_cast<ClassFunctorPtr &>(runtime.ptr)->~FlatClassFunctor();
            }
            if (deallocate)
                Allocator::Deallocate(runtime.ptr, sizeof(FlatClassFunctor), alignof(FlatClassFunctor));
        };
    }

    /** @brief Prepare a volatile member function */
    template<auto MemberFunction, typename ClassType>
        requires Internal::FunctorMemberInvocable<MemberFunction, ClassType, Return, Args...>
    inline void prepare(ClassType * const instance) noexcept
    {
        release<false>();
        _invoke = [](Cache &cache, Args ...args) {
            if constexpr (std::is_same_v<Return, void>)
                (CacheAs<ClassType *>(cache)->*MemberFunction)(std::forward<Args>(args)...);
            else
                return (CacheAs<ClassType *>(cache)->*MemberFunction)(std::forward<Args>(args)...);
        };
        _destruct = nullptr;
        new (&_cache) ClassType *(instance);
    }

    /** @brief Prepare a const member function */
    template<auto MemberFunction, typename ClassType>
        requires Internal::FunctorMemberInvocable<MemberFunction, const ClassType, Return, Args...>
    inline void prepare(const ClassType * const instance) noexcept
    {
        release<false>();
        _invoke = [](Cache &cache, Args ...args) {
            if constexpr (std::is_same_v<Return, void>)
                (CacheAs<const ClassType *>(cache)->*MemberFunction)(std::forward<Args>(args)...);
            else
                return (CacheAs<const ClassType *>(cache)->*MemberFunction)(std::forward<Args>(args)...);
        };
        _destruct = nullptr;
        new (&_cache) const ClassType *(instance);
    }

    /** @brief Prepare a free function */
    template<auto Function>
        requires Internal::FunctorInvocable<decltype(Function), Return, Args...>
    inline void prepare(void) noexcept
    {
        release<false>();
        _invoke = [](Cache &, Args ...args) -> Return {
            if constexpr (std::is_same_v<Return, void>)
                return (*Function)(std::forward<Args>(args)...);
            else
                return (*Function)(std::forward<Args>(args)...);
        };
        _destruct = nullptr;
    }

    /** @brief Prepare a pointer to functor using a custom deleter */
    template<auto Deleter, typename ClassFunctor>
        requires Internal::FunctorNoCacheRequirements<ClassFunctor, CacheSize>
            && Internal::FunctorInvocable<ClassFunctor, Return, Args...>
            && Internal::FunctorInvocable<decltype(Deleter), void, ClassFunctor *>
    inline void prepare(ClassFunctor * const functorPtr) noexcept
    {
        using ClassFunctorPtr = ClassFunctor *;

        auto &runtime = CacheAs<RuntimeAllocation>(_cache);

        release<false>();
        runtime.ptr = functorPtr;
        runtime.size = sizeof(ClassFunctor);
        runtime.capacity = sizeof(ClassFunctor);
        _invoke = [](Cache &cache, Args ...args) -> Return {
            if constexpr (std::is_same_v<Return, void>)
                (*reinterpret_cast<ClassFunctorPtr &>(CacheAs<RuntimeAllocation>(cache).ptr))(std::forward<Args>(args)...);
            else
                return (*reinterpret_cast<ClassFunctorPtr &>(CacheAs<RuntimeAllocation>(cache).ptr))(std::forward<Args>(args)...);
        };
        _destruct = [](Cache &cache, const bool) {
            auto &runtime = CacheAs<RuntimeAllocation>(cache);
            Deleter(reinterpret_cast<ClassFunctorPtr>(runtime.ptr));
            runtime.ptr = nullptr;
        };
    }

    /** @brief Invoke internal functor */
    inline Return operator()(Args ...args) const noexcept { return _invoke(const_cast<Cache &>(_cache), std::forward<Args>(args)...); }

private:
    OpaqueInvoke _invoke { nullptr };
    OpaqueDestructor _destruct { nullptr };
    Cache _cache {};
};