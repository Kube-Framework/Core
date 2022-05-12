/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: MPSC Queue
 */

#pragma once

#include <atomic>
#include <cstdlib>
#include <memory>
#include <algorithm>

#include "Utils.hpp"

namespace kF::Core
{
    template<typename Type, kF::Core::Utils::StaticAllocator Allocator>
    class MPSCQueue;
}

/**
 * @brief The MPSC queue is a lock-free queue that supports a Multiple Producers and Single Consumer
 * The queue supports ranged push / pop to insert multiple elements without performance impact
 *
 * @tparam Type to be inserted
 * @tparam Allocator Static allocator
 */
template<typename Type, kF::Core::Utils::StaticAllocator Allocator = kF::Core::Utils::DefaultStaticAllocator>
class alignas_double_cacheline kF::Core::MPSCQueue
{
public:
    /** @brief Buffer structure containing all cells */
    struct Buffer
    {
        Type *data { nullptr };
        std::size_t capacity { 0 };
    };

    /** @brief Local thread cache */
    struct ConsumerCache
    {
        Buffer buffer {};
        std::size_t value { 0 };
    };

    /** @brief Shared thread cache */
    struct ProducerCache
    {
        Buffer buffer {};
        std::atomic<std::size_t> value { 0 };
    };


    /** @brief Destruct and release all memory (unsafe) */
    ~MPSCQueue(void) noexcept;

    /** @brief Default constructor initialize the queue
     * If 'usedAsBuffer' is true, capacity will be increased by 1 because the queue implementation needs one unused value when the queue is full */
    MPSCQueue(const std::size_t capacity, const bool usedAsBuffer = true) noexcept;


    /** @brief Push a single element into the queue
     *  @return true if the element has been inserted */
    template<typename ...Args>
    [[nodiscard]] bool push(Args &&...args) noexcept
        requires std::constructible_from<Type, Args...>;


    /** @brief Pop a single element from the queue
     *  @return true if an element has been extracted */
    [[nodiscard]] bool pop(Type &value) noexcept;


    /** @brief Push exactly 'count' elements into the queue
     *  @tparam ForceCopy If true, will prevent to move construct elements
     *  @return Success on true */
    template<std::input_iterator InputIterator>
    [[nodiscard]] bool tryPushRange(const InputIterator from, const InputIterator to) noexcept
        { return pushRangeImpl<false>(from, to); }

    /** @brief Push up to 'count' elements into the queue
     *  @tparam ForceCopy If true, will prevent to move construct elements
     *  @return The number of extracted elements */
    template<std::input_iterator InputIterator>
    [[nodiscard]] std::size_t pushRange(const InputIterator from, const InputIterator to) noexcept
        { return pushRangeImpl<true>(from, to); }


    /** @brief Pop exactly 'count' elements from the queue
     *  @return Success on true */
    template<typename OutputIterator> requires std::output_iterator<OutputIterator, Type>
    [[nodiscard]] bool tryPopRange(const OutputIterator from, const OutputIterator to) noexcept
        { return popRangeImpl<false>(from, to); }

    /** @brief Pop up to 'count' elements from the queue
     *  @return The number of extracted elements */
    template<typename OutputIterator> requires std::output_iterator<OutputIterator, Type>
    [[nodiscard]] std::size_t popRange(const OutputIterator from, const OutputIterator to) noexcept
        { return popRangeImpl<true>(from, to); }


    /** @brief Clear all elements of the queue (unsafe) */
    void clear(void) noexcept;


    /** @brief Get the size of the queue */
    [[nodiscard]] std::size_t size(void) const noexcept;


private:
    alignas_cacheline std::atomic<size_t> _tail { 0 }; // Tail accessed by both producer and consumer threads
    alignas_cacheline ProducerCache _tailCache {}; // Cache accessed by producer thread

    alignas_cacheline std::atomic<size_t> _head { 0 }; // Head accessed by both producer and consumers threads
    alignas_cacheline ConsumerCache _headCache {}; // Cache accessed by consumer threads


    /** @brief Copy and move constructors disabled */
    MPSCQueue(const MPSCQueue &other) = delete;
    MPSCQueue(MPSCQueue &&other) = delete;


    /** @brief Implementation of push range */
    template<bool AllowLess, std::input_iterator InputIterator>
    [[nodiscard]] std::size_t pushRangeImpl(const InputIterator from, const InputIterator to) noexcept;

    /** @brief Implementation of pop range */
    template<bool AllowLess, typename OutputIterator> requires std::output_iterator<OutputIterator, Type>
    [[nodiscard]] std::size_t popRangeImpl(const OutputIterator from, const OutputIterator to) noexcept;
};

#include "MPSCQueue.ipp"