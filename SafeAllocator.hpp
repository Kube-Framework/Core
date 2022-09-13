/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Thread safe allocator
 */

#pragma once

#include <atomic>
#include <array>

#include "TaggedPtr.hpp"
#include "IAllocator.hpp"
#include "AllocatorUtils.hpp"

namespace kF::Core
{
    template<std::size_t MinSizePower, std::size_t MaxSizePower, std::size_t MaxStackSize>
    class SafeAllocator;

    namespace AllocatorUtils
    {
        /** @brief Meta data of a stack */
        struct alignas_half_cacheline SafeStackMetaData
        {
            std::size_t size { 0u };
            std::size_t head { 0u };
            SafeStackMetaData *next { nullptr };

            /** @brief Get the stack data pointer at given byte index */
            [[nodiscard]] inline void *dataHead(void) noexcept
                { return reinterpret_cast<void *>(reinterpret_cast<std::uint8_t *>(this) + head); }

            /** @brief Get the stack data pointer at given byte index */
            [[nodiscard]] inline void *dataAt(const std::size_t at) noexcept
                { return reinterpret_cast<void *>(reinterpret_cast<std::uint8_t *>(this) + at); }

            /** @brief Get the stack data pointer at given byte index casted to allocation header */
            [[nodiscard]] inline AllocatorUtils::AllocationHeader *allocationAt(const std::size_t at) noexcept
                { return reinterpret_cast<AllocatorUtils::AllocationHeader *>(dataAt(at)); }
        };


        /** @brief Atomic stack pointer type */
        using AtomicStack = std::atomic<TaggedPtr<SafeStackMetaData>>;


        /** @brief Atomic bucket type */
        template<std::size_t Alignment>
            requires (Alignment >= 16) // 16 values at minimum (< ~16 concurrent threads)
        using AtomicBucket = std::atomic<TaggedPtr<AllocationHeader, Alignment>>;

        /** @brief Atomic bucket type but aligned to the size of a cacheline */
        template<std::size_t Alignment>
        struct alignas_cacheline AlignedAtomicBucket
        {
            AtomicBucket<Alignment> value;

            /** @brief Implicit reference converter */
            [[nodiscard]] operator AtomicBucket<Alignment> &(void) noexcept { return value; }

            /** @brief Implicit constant reference converter */
            [[nodiscard]] operator const AtomicBucket<Alignment> &(void) const noexcept { return value; }
        };


        /** @brief Try to steal an atomic stack */
        [[nodiscard]] SafeStackMetaData *TryStealAtomicStack(AtomicStack &target) noexcept;

        /** @brief Try to steal an atomic stack */
        void InsertAtomicStack(AtomicStack &target, SafeStackMetaData *stack) noexcept;


        /** @brief Try to steal an atomic bucket */
        template<std::size_t Alignment>
        [[nodiscard]] void *TryStealAtomicBucket(AtomicBucket<Alignment> &bucket) noexcept;

        /** @brief Try to insert a data into a bucket list */
        template<std::size_t Alignment>
        void InsertAtomicBucket(AtomicBucket<Alignment> &bucket, void * const data) noexcept;

        /** @brief Non-template utility that destroys safe allocator stacks */
        void DestroySafeAllocator(const std::size_t pageSize, SafeStackMetaData * const stack) noexcept;
    }

}

/** @brief Synchronized memory allocator, used for general purposes allocations with threading support.
 *  Strength:   + Memory is split into aligned chunks that are reusable in many different size / alignment scenarios
 *  Weakness:   - No defragmentation: you may need to tune the template parameters to fit the scenario you are dealing with.
 *
 *  NB: By default, bucket fragmentation is disabled to fit the most general cases.
 *
 *  The implementation aims to bring efficiency of a stack based allocator with buckets to retain memory.
 *  This implementation ignores defragmentation and thus, you may need to tune the MinSizePower template parameter
 *  in order to not split too much the memory at the cost of small unused memory holes.
 *  Each bucket hold a memory allocation which is the size of a power of 2 and aligned over this size:
 *  32 bytes aligned over 32, 64 bytes aligned over 64, etc ...
 *  This allow fast fragmentation of the memory at the cost of defragmentation that becomes too heavy to be supported.
 *
 *
 *  @tparam MinSizePower The minimal allocation size a bucket can store
 *  @tparam MaxSizePower The maximal allocation size a bucket can store
 *  @tparam MaxStackSizePower The maximal allocation size a stack can have
 *
 *  @todo Benchmark an allocate implementation that prioritize stack allocation rather than fragmentation in case of non perfect fit
 *
 *  @note 1 << 16 == MMAP_THRESHOLD
*/
template<std::size_t MinSizePower = 5, std::size_t MaxSizePower = 12, std::size_t MaxStackSizePower = 16>
class alignas_double_cacheline kF::Core::SafeAllocator : public IAllocator
{
public:
    /** @brief Minimum retained allocation size in byte */
    static constexpr std::size_t MinSize = 1ul << MinSizePower;

    /** @brief Maximum retained allocation size in byte */
    static constexpr std::size_t MaxSize = 1ul << MaxSizePower;

    /** @brief Number of bucket to retain */
    static constexpr std::size_t BucketCount = (MaxSizePower - MinSizePower) + 1;

    /** @brief Maximum stack allocation size in byte */
    static constexpr std::size_t MaxStackSize = 1ul << MaxStackSizePower;


    static_assert(MaxStackSize > MaxSize);
    static_assert(BucketCount > 0, "BucketCount must be superior to 0");
    static_assert(MinSize >= sizeof(void *), "MinSize must be superior or equal to sizeof(void *)");


    /** @brief Virtual destructor ! NOT THREAD SAFE ! */
    ~SafeAllocator(void) noexcept override;

    /** @brief Constructor */
    SafeAllocator(void) noexcept;


    /** @brief Allocate function implementation */
    [[nodiscard]] void *allocate(const std::size_t size, const std::size_t alignment) noexcept override;

    /** @brief Deallocate function implementation */
    void deallocate(void * const data, const std::size_t size, const std::size_t alignment) noexcept override;


private:
    /** @brief Allocate data from a specific bucket */
    [[nodiscard]] void *allocateFromBucket(const std::size_t bucketIndex) noexcept;

    /** @brief Deallocate data from a specific bucket */
    void deallocateFromBucket(void * const data, const std::size_t bucketIndex) noexcept;


    /** @brief Allocate a chunk from stack */
    [[nodiscard]] void *allocateFromStack(const std::size_t bucketSize) noexcept;


    /** @brief Build a new stack for internal allocation, considering the size of queried bucket */
    [[nodiscard]] AllocatorUtils::SafeStackMetaData *buildStack(const std::size_t bucketSize) noexcept;


    /** @brief Fragment stack until end */
    void fragmentStack(AllocatorUtils::SafeStackMetaData * const stack) noexcept;

    /** @brief Fragment a single block of the stack */
    void fragmentStackBlock(AllocatorUtils::SafeStackMetaData * const stack, const std::size_t size) noexcept;


    // Cacheline 0
    alignas_cacheline const std::size_t _pageSize;
    // Cacheline 1
    alignas_cacheline std::atomic<std::size_t> _maxStackSize { 0u };
    // Cacheline 2
    alignas_cacheline AllocatorUtils::AtomicStack _stack {}; // N stacks can be linked at a time
    // Cacheline 3
    alignas_cacheline AllocatorUtils::AtomicStack _busyStack {};
    // Cacheline 4 + (1 per bucket)
    std::array<AllocatorUtils::AlignedAtomicBucket<MinSize>, BucketCount> _buckets {};
};

#include "SafeAllocator.ipp"