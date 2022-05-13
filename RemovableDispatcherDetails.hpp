/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Removable Dispatcher
 */

#pragma once

#include "FunctorUtils.hpp"
#include "Vector.hpp"

namespace kF::Core
{
    template<typename Signature,
            FunctorRequirements InternalFunctor,
            StaticAllocatorRequirements Allocator, std::size_t CacheSize>
    class RemovableDispatcherDetails;
}

/** @brief Fast event dispatcher */
template<typename Return, typename... Args,
        kF::Core::FunctorRequirements InternalFunctor,
        kF::Core::StaticAllocatorRequirements Allocator, std::size_t CacheSize>
class alignas_quarter_cacheline kF::Core::RemovableDispatcherDetails<Return(Args...), InternalFunctor, Allocator, CacheSize>
{
public:
    /** @brief Functor handle */
    using Handle = std::uint32_t;


    /** @brief Destructor */
    inline ~RemovableDispatcherDetails(void) noexcept = default;

    /** @brief Default constructor */
    inline RemovableDispatcherDetails(void) noexcept = default;

    /** @brief Move constructor */
    inline RemovableDispatcherDetails(RemovableDispatcherDetails &&dispatcher) noexcept = default;


    /** @brief Move assignment*/
    inline RemovableDispatcherDetails &operator=(RemovableDispatcherDetails &&dispatcher) noexcept = default;


    /** @brief Internal functor count */
    [[nodiscard]] inline auto count(void) const noexcept { return _functors.size() - _freeList.size(); }


    /** @brief Add a functor to dispatch list */
    template<typename Functor>
    inline Handle add(Functor &&functor) noexcept
    {
        InternalFunctor *internal;
        Handle index;
        if (!_freeList.empty()) {
            index = _freeList.back();
            _freeList.pop();
            internal = &_functors[index];
        } else {
            index = _functors.size();
            internal = &_functors.push();
        }
        internal->prepare(std::forward<Functor>(functor));
        return index + 1;
    }

    /** @brief Add a member function to dispatch list */
    template<auto MemberFunction, typename ClassType>
    inline Handle add(ClassType * const instance) noexcept
    {
        InternalFunctor *functor;
        Handle index;
        if (!_freeList.empty()) {
            index = _freeList.back();
            _freeList.pop();
            functor = &_functors[index];
        } else {
            index = _functors.size();
            functor = &_functors.push();
        }
        functor->template prepare<MemberFunction>(instance);
        return index + 1;
    }

    /** @brief Add a const member function to dispatch list */
    template<auto MemberFunction, typename ClassType>
    inline Handle add(const ClassType * const instance) noexcept
    {
        InternalFunctor *functor;
        Handle index;
        if (!_freeList.empty()) {
            index = _freeList.back();
            _freeList.pop();
            functor = &_functors[index];
        } else {
            index = _functors.size();
            functor = &_functors.push();
        }
        functor->template prepare<MemberFunction>(instance);
        return index + 1;
    }

    /** @brief Add a free function to dispatch list */
    template<auto FreeFunction>
    inline Handle add(void) noexcept
    {
        InternalFunctor *functor;
        Handle index;
        if (!_freeList.empty()) {
            index = _freeList.back() - 1;
            _freeList.pop();
            functor = &_functors[index];
        } else {
            index = _functors.size();
            functor = &_functors.push();
        }
        functor->template prepare<FreeFunction>();
        return index + 1;
    }


    /** @brief Remove a slot from its handle */
    inline void remove(const Handle handle) noexcept
    {
        kFAssert(handle, "RemovableDispatcherDetails::remove: Can't remove null handle");
        const auto index = handle - 1u;
        _functors[index].destroy();
        _freeList.push(index);
    }

    /** @brief Clear dispatch list */
    inline void clear(void) noexcept { _functors.clear(); _freeList.clear(); }


    /** @brief Dispatch every internal functors */
    inline void dispatch(Args ...args)
    {
        for (auto &functor : _functors) {
            if (functor)
                functor(std::forward<Args>(args)...);
        }
    }

    /** @brief Dispatch every internal functors with a given callback to receive the return value of each functor */
    template<typename Callback>
        requires (!std::is_same_v<Return, void> && std::invocable<Callback, Return>)
    inline void dispatch(Callback &&callback, Args ...args)
    {
        for (auto &functor : _functors) {
            if (functor)
                callback(functor(std::forward<Args>(args)...));
        }
    }

private:
    TinyVector<InternalFunctor, Allocator> _functors {};
    TinyVector<Handle, Allocator> _freeList {};
};