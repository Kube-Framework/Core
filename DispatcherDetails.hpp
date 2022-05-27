/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Dispatcher
 */

#pragma once

#include "FunctorUtils.hpp"
#include "Vector.hpp"

namespace kF::Core
{
    template<typename Signature,
            FunctorRequirements InternalFunctor,
            StaticAllocatorRequirements Allocator, std::size_t CacheSize>
    class DispatcherDetails;
}

/** @brief Fast event dispatcher */
template<typename Return, typename... Args,
        kF::Core::FunctorRequirements InternalFunctor,
        kF::Core::StaticAllocatorRequirements Allocator, std::size_t CacheSize>
class alignas_quarter_cacheline kF::Core::DispatcherDetails<Return(Args...), InternalFunctor, Allocator, CacheSize>
{
public:
    /** @brief Destructor */
    inline ~DispatcherDetails(void) noexcept = default;

    /** @brief Default constructor */
    inline DispatcherDetails(void) noexcept = default;

    /** @brief Move constructor */
    inline DispatcherDetails(DispatcherDetails &&dispatcher) noexcept = default;


    /** @brief Move assignment*/
    inline DispatcherDetails &operator=(DispatcherDetails &&dispatcher) noexcept = default;


    /** @brief Internal functor count */
    [[nodiscard]] inline auto count(void) const noexcept { return _functors.size(); }


    /** @brief Add a functor to dispatch list */
    template<typename Functor>
    inline void add(Functor &&functor) noexcept { _functors.push().prepare(std::forward<Functor>(functor)); }

    /** @brief Add a member function to dispatch list */
    template<auto MemberFunction, typename ClassType>
    inline void add(ClassType * const instance) noexcept { _functors.push().template prepare<MemberFunction>(instance); }

    /** @brief Add a const member function to dispatch list */
    template<auto MemberFunction, typename ClassType>
    inline void add(const ClassType * const instance) noexcept { _functors.push().template prepare<MemberFunction>(instance); }

    /** @brief Add a free function to dispatch list */
    template<auto FreeFunction>
    inline void add(void) noexcept { _functors.push().template prepare<FreeFunction>(); }


    /** @brief Clear dispatch list */
    inline void clear(void) noexcept { _functors.clear(); }


    /** @brief Dispatch every internal functors */
    inline void dispatch(Args ...args)
    {
        for (auto &functor : _functors)
            functor(std::forward<Args>(args)...);
    }

    /** @brief Dispatch every internal functors with a given callback to receive the return value of each functor */
    template<typename Callback>
        requires (!std::is_same_v<Return, void> && std::invocable<Callback, Return>)
    inline void dispatch(Callback &&callback, Args ...args)
    {
        for (auto &functor : _functors)
            callback(functor(std::forward<Args>(args)...));
    }

private:
    Vector<InternalFunctor, Allocator> _functors {};
};