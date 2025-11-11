#pragma once

#include <array>
#include <atomic>
#include <optional>

template <typename T>
class LockFreeArray
{
  private:
    static constexpr size_t MAX_SIZE = 4;

    struct Node
    {
        std::atomic<T*> data;
        Node() : data(nullptr) {}
    };

    std::array<Node, MAX_SIZE> slots;
    std::atomic<size_t>        count;

  public:
    LockFreeArray() : count(0) {}

    ~LockFreeArray()
    {
        for (size_t i = 0; i < MAX_SIZE; ++i)
        {
            T* ptr = slots[i].data.load(std::memory_order_acquire);
            if (ptr != nullptr)
            {
                delete ptr;
            }
        }
    }

    bool push(const T& value)
    {

        if (const size_t current_count = count.load(std::memory_order_acquire); current_count >= MAX_SIZE)
        {
            return false;
        }

        for (size_t i = 0; i < MAX_SIZE; ++i)
        {
            T* expected  = nullptr;
            T* new_value = new T(value);

            if (slots[i].data.compare_exchange_strong(
                    expected, new_value, std::memory_order_release, std::memory_order_acquire))
            {
                count.fetch_add(1, std::memory_order_release);
                return true;
            }

            delete new_value;
        }

        return false;
    }

    std::optional<T> get(size_t index) const
    {
        if (index >= MAX_SIZE)
        {
            return std::nullopt;
        }

        T* ptr = slots[index].data.load(std::memory_order_acquire);
        if (ptr == nullptr)
        {
            return std::nullopt;
        }

        return *ptr;
    }

    // Try to remove an element at index, returns true if successful
    bool remove(size_t index)
    {
        if (index >= MAX_SIZE)
        {
            return false;
        }

        T* expected = slots[index].data.load(std::memory_order_acquire);
        if (expected == nullptr)
        {
            return false;
        }

        if (slots[index].data.compare_exchange_strong(
                expected, nullptr, std::memory_order_release, std::memory_order_acquire))
        {
            delete expected;
            count.fetch_sub(1, std::memory_order_release);
            return true;
        }

        return false;
    }

    size_t size() const { return count.load(std::memory_order_acquire); }

    bool is_full() const { return count.load(std::memory_order_acquire) >= MAX_SIZE; }

    bool is_empty() const { return count.load(std::memory_order_acquire) == 0; }
};
