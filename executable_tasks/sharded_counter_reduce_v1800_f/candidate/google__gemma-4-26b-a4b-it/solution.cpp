#include "interface.h"

#include <atomic>
#include <thread>
#include <vector>
#include <algorithm>

namespace {

uint64_t checksum_u64(const std::vector<uint64_t>& counts) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint64_t v : counts) {
    hash ^= v;
    hash *= 1099511628211ULL;
  }
  return hash;
}

} // namespace

uint64_t sharded_counter_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int num_threads,
    int iters) {
  // Use std::atomic to allow concurrent updates to different buckets without a global mutex.
  // We use a vector of atomics. Note: std::atomic is not trivially copyable, 
  // so we manage the memory manually or use a vector of a wrapper.
  struct AtomicCounter {
    std::atomic<uint64_t> val{0};
  };
  
  // To avoid complex memory management with std::vector<std::atomic>, 
  // we use a raw buffer or a vector of uint64_t and use atomic_ref (C++20) 
  // or simply a vector of std::atomic if we handle initialization carefully.
  // Since we need to reset it every iteration, we'll use a vector of uint64_t 
  // and perform atomic operations on the elements.
  
  std::vector<uint64_t> counts(static_cast<std::size_t>(key_space), 0);
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset counts
    std::fill(counts.begin(), counts.end(), 0);

    std::vector<std::thread> threads;
    const std::size_t total_elements = keys.size();
    const std::size_t chunk = total_elements / static_cast<std::size_t>(num_threads);

    for (int t = 0; t < num_threads; ++t) {
      const std::size_t begin = static_cast<std::size_t>(t) * chunk;
      const std::size_t end = (t + 1 == num_threads) ? total_elements : begin + chunk;

      threads.emplace_back([&, begin, end]() {
        for (std::size_t i = begin; i < end; ++i) {
          // Use atomic fetch_add on the memory address of the specific bucket.
          // This allows multiple threads to work on different buckets simultaneously.
          auto* target = reinterpret_cast<std::atomic<uint64_t>*>(&counts[static_cast<std::size_t>(keys[i])]);
          target->fetch_add(static_cast<uint64_t>(values[i]), std::memory_order_relaxed);
        }
      });
    }

    for (std::thread& th : threads) {
      th.join();
    }

    // Convert the atomic-updated counts back to a standard vector for checksumming
    // (In this implementation, counts is already a vector of uint64_t)
    final_hash = checksum_u64(counts);
  }

  return final_hash;
}