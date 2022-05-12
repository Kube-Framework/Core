/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Dispatcher
 */

#pragma once

#include "Functor.hpp"
#include "DispatcherDetails.hpp"

namespace kF::Core
{
    /** @brief Specialization of dispatcher with 'Functor' */
    template<typename Signature, Utils::StaticAllocator Allocator = Utils::DefaultStaticAllocator, std::size_t CacheSize = CacheLineQuarterSize>
    using Dispatcher = DispatcherDetails<Signature, Functor<Signature, Allocator, CacheSize>, Allocator, CacheSize>;
}