/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: TrivialDispatcher
 */

#pragma once

#include "TrivialFunctor.hpp"
#include "DispatcherDetails.hpp"

namespace kF::Core
{
    /** @brief Specialization of dispatcher with 'TrivialFunctor' */
    template<typename Signature, StaticAllocatorRequirements Allocator = DefaultStaticAllocator, std::size_t CacheSize = CacheLineEighthSize>
    using TrivialDispatcher = DispatcherDetails<Signature, TrivialFunctor<Signature, CacheSize>, Allocator, CacheSize>;
}