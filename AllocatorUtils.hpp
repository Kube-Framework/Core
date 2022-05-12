/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Allocator utilities
 */

#pragma once

#include "Utils.hpp"

namespace kF::Core
{
    namespace AllocatorUtils
    {
        /** @brief Header of an allocation */
        struct AllocationHeader
        {
            AllocationHeader *next { nullptr };
        };


        /** @brief Fallback allocate function */
        [[nodiscard]] inline void *FallbackAllocate(const std::size_t size, const std::size_t alignment) noexcept
            { return Utils::AlignedAlloc(size, alignment); }

        /** @brief Fallback deallocate function */
        inline void FallbackDeallocate(void * const data, const std::size_t size, const std::size_t alignment) noexcept
            { return Utils::AlignedFree(data, size, alignment); }


        /** @brief Get the bucket index of a runtime size considering the minimal power size of the allocator (constant time) */
        template<std::size_t MinSizePower>
        [[nodiscard]] std::size_t GetBucketIndex(const std::size_t size) noexcept;

        /** @brief Find the number of bucket subdivision between two bucket iterators (linear time)
         *  @param begin The begin iterator is updated inside the function, pointing to the bucket to be subdivided */
        template<std::size_t MinSizePower, std::size_t MaxSizePower, std::size_t BucketFragmentationRange, typename BucketIterator>
        [[nodiscard]] std::size_t FindBucketSubdivisionCount(BucketIterator &begin, const BucketIterator end) noexcept;

        /** @brief Find the largest bucket size within availableSize
         *  @param head The head allow to track alignment of availableSize
         *  @note The bucket is guaranted to be aligned over its size */
        template<std::size_t MaxSizePower>
        [[nodiscard]] std::size_t FindBucketFit(const std::size_t availableSize, const std::size_t head) noexcept;


        /** @brief Get the ideal stack size of allocator using a target bucket size and a page size */
        template<std::size_t StackMetaDataSize, std::size_t MaxStackSize>
        [[nodiscard]] std::size_t GetStackSize(const std::size_t bucketSize, const std::size_t pageSize, const std::size_t lastStackSize) noexcept;
    }
}

#include "AllocatorUtils.ipp"