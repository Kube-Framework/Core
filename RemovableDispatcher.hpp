/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Dispatcher
 */

#pragma once

#include "Functor.hpp"
#include "RemovableDispatcherDetails.hpp"

namespace kF::Core
{
    /** @brief Specialization of removable dispatcher with 'Functor' */
    template<typename Signature, Utils::StaticAllocator Allocator = Utils::DefaultStaticAllocator, std::size_t CacheSize = CacheLineQuarterSize>
    using RemovableDispatcher = RemovableDispatcherDetails<Signature, Functor<Signature, Allocator, CacheSize>, Allocator, CacheSize>;
}