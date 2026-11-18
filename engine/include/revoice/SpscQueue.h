#pragma once
#include <array>
#include <atomic>
#include <cstddef>
#include <type_traits>

namespace revoice {
// One producer and one consumer. The spare slot distinguishes full from empty.
// No mutex, allocation, or system call occurs in push/pop.
template<class T,std::size_t Capacity> class SpscQueue {
    static_assert(Capacity>1);
    static_assert(std::is_trivially_copyable_v<T>);
    static_assert(std::atomic<std::size_t>::is_always_lock_free);
    std::array<T,Capacity> data_{};
    alignas(64) std::atomic<std::size_t> write_{0};
    alignas(64) std::atomic<std::size_t> read_{0};
public:
    bool push(const T& value) noexcept {
        const auto write=write_.load(std::memory_order_relaxed);
        const auto next=(write+1)%Capacity;
        if(next==read_.load(std::memory_order_acquire)) return false;
        data_[write]=value;
        write_.store(next,std::memory_order_release);
        return true;
    }
    bool pop(T& value) noexcept {
        const auto read=read_.load(std::memory_order_relaxed);
        if(read==write_.load(std::memory_order_acquire)) return false;
        value=data_[read];
        read_.store((read+1)%Capacity,std::memory_order_release);
        return true;
    }
    // Call only while both producer and consumer are stopped.
    void reset() noexcept { write_=0; read_=0; }
};
} // namespace revoice
