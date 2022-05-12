/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Tests of the SafeAllocator
 */

#include <thread>
#include <cstdlib>
#include <cstring>

#include <gtest/gtest.h>

#include <Kube/Core/UnsafeAllocator.hpp>
#include <Kube/Core/SafeAllocator.hpp>

using namespace kF;

struct Allocation
{
    void *data { nullptr };
    std::size_t size {};
    std::size_t alignment {};
};


template<typename Allocator>
[[nodiscard]] static void *TestAllocationRetention(Allocator &allocator, const std::size_t size, const std::size_t align)
{
    void * const ptr = allocator.allocate(size, align);
    void *copy = ptr;
    std::size_t space { size };
    bool isValid = ptr != nullptr;
    void *alignPtr = std::align(align, size, copy, space);
    bool isAligned = alignPtr == ptr;

    EXPECT_TRUE(isValid);
    EXPECT_TRUE(isAligned);
    if (!isValid || !isAligned) {
        return nullptr;
    }
    std::size_t index = 0u;
    for (char value = 0; index < size; ++index, ++value) {
        reinterpret_cast<char *>(ptr)[index] = value;
    }
    return ptr;
}

template<typename Allocator, typename Vector>
static bool TestDeallocateRetention(Allocator &allocator, const Vector &retentions)
{
    for (const auto &alloc : retentions) {
        std::size_t index = 0u;
        for (char value = 0; index < alloc.size; ++index, ++value) {
            char at = reinterpret_cast<const char *>(alloc.data)[index];
            EXPECT_EQ(at, value);
            if (at != value)
                return false;
        }
        allocator.deallocate(alloc.data, alloc.size, alloc.alignment);
    }
    return true;
}

template<typename Allocator, std::size_t MinAlignment, std::size_t MaxAlignment, std::size_t MaxSize, std::size_t RetentionCount>
static bool TestAllocatorRetention(Allocator &allocator)
{
    std::vector<Allocation> retentions(RetentionCount);

    std::size_t index = 0ul;
    std::size_t cycles = 0ul;
    while (cycles < 4u) {
        for (std::size_t align = MinAlignment; align != MaxAlignment; align <<= 1) {
            for (std::size_t size = align; size != MaxSize ; size <<= 1) {
                retentions[index] = Allocation {
                    .data = TestAllocationRetention(allocator, size, align),
                    .size = size,
                    .alignment = align
                };
                if (++index == RetentionCount) {
                    if (!TestDeallocateRetention(allocator, retentions))
                        return false;
                    index = 0u;
                    ++cycles;
                }
            }
        }
    }
    retentions.erase(retentions.begin() + index, retentions.end());
    return TestDeallocateRetention(allocator, retentions);
}

template<typename Allocator>
static bool TestAllocationNoRetention(Allocator &allocator, const std::size_t size, const std::size_t align)
{
    auto ptr = TestAllocationRetention(allocator, size, align);
    if (!ptr)
        return false;
    allocator.deallocate(ptr, size, align);
    return true;
}

template<typename Allocator, std::size_t MinAlignment, std::size_t MaxAlignment, std::size_t MaxSize>
static bool TestAllocatorNoRetention(Allocator &allocator)
{
    std::size_t cycles = 0ul;
    while (cycles != 1000u) {
        for (std::size_t align = MinAlignment; align != MaxAlignment; align <<= 1) {
            for (std::size_t size = align; size != MaxSize ; size <<= 1) {
                if (!TestAllocationNoRetention(allocator, size, align))
                    return false;
            }
        }
        ++cycles;
    }
    return true;
}

TEST(UnsafeAllocator, NoRetention)
{
    using Allocator = Core::UnsafeAllocator<>;

    Allocator allocator;
    bool success = TestAllocatorNoRetention<Allocator, 8u, 256u, 1u << 18u>(allocator);
    ASSERT_TRUE(success);

}

TEST(UnsafeAllocator, Retention)
{
    using Allocator = Core::UnsafeAllocator<>;

    Allocator allocator;
    bool success = TestAllocatorRetention<Allocator, 8u, 256u, 1u << 18u, 10>(allocator)
        && TestAllocatorRetention<Allocator, 8u, 256u, 1u << 18u, 100>(allocator)
        && TestAllocatorRetention<Allocator, 8u, 256u, 1u << 18u, 1000>(allocator);
    ASSERT_TRUE(success);
}

TEST(SafeAllocator, NoRetention)
{
    using Allocator = Core::SafeAllocator<>;

    Allocator allocator;
    bool success = TestAllocatorNoRetention<Allocator, 8u, 256u, 1u << 18u>(allocator);
    ASSERT_TRUE(success);

}

TEST(SafeAllocator, Retention)
{
    using Allocator = Core::SafeAllocator<>;

    Allocator allocator;
    bool success = TestAllocatorRetention<Allocator, 8u, 256u, 1u << 18u, 10>(allocator)
        && TestAllocatorRetention<Allocator, 8u, 256u, 1u << 18u, 100>(allocator)
        && TestAllocatorRetention<Allocator, 8u, 256u, 1u << 18u, 1000>(allocator);
    ASSERT_TRUE(success);
}

// #include <Kube/Core/DebugAllocator.hpp>

TEST(SafeAllocator, ThreadingNoRetention)
{
    // using Allocator = Core::DebugAllocator<Core::SafeAllocator<>>;
    using Allocator = Core::SafeAllocator<>;

    // Allocator allocator("ThreadingNoRetention");
    Allocator allocator;
    auto testFunc = [&allocator] {
        TestAllocatorNoRetention<Allocator, 8u, 256u, 1u << 13u>(allocator);
    };

    std::vector<std::unique_ptr<std::thread>> thds(8);
    for (auto &thd : thds) {
        thd = std::make_unique<std::thread>(testFunc);
    }

    for (auto &thd : thds) {
        thd->join();
    }
}

TEST(SafeAllocator, ThreadingRetention)
{
    using Allocator = Core::SafeAllocator<>;

    Allocator allocator;
    auto testFunc = [&allocator] {
        TestAllocatorRetention<Allocator, 8u, 256u, 1u << 14u, 10>(allocator);
        TestAllocatorRetention<Allocator, 8u, 256u, 1u << 14u, 100>(allocator);
        TestAllocatorRetention<Allocator, 8u, 256u, 1u << 14u, 1000>(allocator);
    };

    std::vector<std::unique_ptr<std::thread>> thds(8);
    for (auto &thd : thds) {
        thd = std::make_unique<std::thread>(testFunc);
    }

    for (auto &thd : thds) {
        thd->join();
    }
}