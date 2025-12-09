#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <vector>

constexpr size_t CACHE_LINE_SIZE  = 64;
constexpr size_t ARENA_BLOCK_SIZE = 64 * 1024;
constexpr size_t OCTREE_MAX_DEPTH = 16;

template <typename T>
struct alignas(CACHE_LINE_SIZE) CacheAligned
{
    T value;
};

class ArenaAllocator
{
  private:
    struct ArenaBlock
    {
        std::unique_ptr<uint8_t[]> memory;
        std::atomic<size_t>        offset;
        size_t                     size;

        ArenaBlock(size_t block_size) : memory(new uint8_t[block_size]), offset(0), size(block_size) {}
    };

    std::vector<std::unique_ptr<ArenaBlock>> blocks;
    std::atomic<size_t>                      current_block_idx;
    std::atomic<size_t>                      num_blocks;
    size_t                                   block_size;
    std::atomic<bool>                        expanding;

  public:
    explicit ArenaAllocator(const size_t initial_block_size = ARENA_BLOCK_SIZE) :
        current_block_idx(0), block_size(initial_block_size)
    {
        blocks.push_back(std::make_unique<ArenaBlock>(block_size));
    }

    // Allocate memory with proper alignment
    void* allocate(size_t size, size_t alignment = alignof(std::max_align_t))
    {
        size_t aligned_size = (size + alignment - 1) & ~(alignment - 1);

        while (true)
        {
            size_t block_idx        = current_block_idx.load(std::memory_order_acquire);
            size_t available_blocks = num_blocks.load(std::memory_order_acquire);

            if (block_idx >= available_blocks)
            {
                expand_arena();
                continue;
            }

            ArenaBlock* block = blocks[block_idx].get();
            if (!block)
            {
                std::this_thread::yield();
                continue;
            }

            size_t old_offset = block->offset.load(std::memory_order_relaxed);

            size_t aligned_offset = (old_offset + alignment - 1) & ~(alignment - 1);
            size_t new_offset     = aligned_offset + aligned_size;

            if (new_offset > block->size)
            {
                current_block_idx.compare_exchange_weak(
                    block_idx, block_idx + 1, std::memory_order_release, std::memory_order_relaxed);
                continue;
            }

            if (block->offset.compare_exchange_weak(
                    old_offset, new_offset, std::memory_order_release, std::memory_order_relaxed))
            {
                return block->memory.get() + aligned_offset;
            }
        }
    }

    template <typename T, typename... Args>
    T* construct(Args&&... args)
    {
        void* ptr = allocate(sizeof(T), alignof(T));
        return new (ptr) T(std::forward<Args>(args)...);
    }

    void reset()
    {
        const size_t num = num_blocks.load(std::memory_order_acquire);
        for (size_t i = 0; i < num && i < blocks.size(); ++i)
        {
            if (blocks[i])
            {
                blocks[i]->offset.store(0, std::memory_order_release);
            }
        }
        current_block_idx.store(0, std::memory_order_release);
    }

  private:
    void expand_arena()
    {
        bool expected = false;
        if (expanding.compare_exchange_strong(expected, true, std::memory_order_acquire, std::memory_order_relaxed))
        {
            try
            {
                auto new_block = std::make_unique<ArenaBlock>(block_size);

                blocks.push_back(std::move(new_block));

                num_blocks.fetch_add(1, std::memory_order_release);
            }
            catch (...)
            {
                expanding.store(false, std::memory_order_release);
                throw;
            }

            expanding.store(false, std::memory_order_release);
        }
        else
        {
            while (expanding.load(std::memory_order_acquire))
            {
                std::this_thread::yield();
            }
        }
    }
};