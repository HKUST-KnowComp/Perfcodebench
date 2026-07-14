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

}  // namespace

uint64_t sharded_counter_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int num_threads,
    int iters) {
  // Use atomics to allow concurrent updates to different buckets without a global mutex.
  // We use a vector of atomics. Note: std::atomic is not Copyable, so we manage memory carefully.
  // To avoid the overhead of atomic vector resizing or complex management, we use a raw buffer 
  // or a vector of a wrapper. Since we need to reset it every iteration, we'll use a vector of 
  // uint64_t and use atomic_ref (C++20) or just use a vector of std::atomic<uint64_t>.
  
  // Given the constraints and typical environment, std::atomic<uint64_t> in a vector is the cleanest.
  // However, std::vector<std::atomic<uint64_t>> is not possible because atomic is not CopyInsertable.
  // We will use a unique_ptr to an array of atomics.
  
  auto counts_ptr = std::make_unique<std::atomic<uint64_t>[]>(static_cast<size_t>(key_space));
  
  uint64_t final_hash = 0;
  const size_t n = keys.size();

  for (int iter = 0; iter < iters; ++iter) {
    // Reset counts
    for (size_t i = 0; i < static_cast<size_t>(key_space); ++i) {
      counts_ptr[i].store(0, std::memory_order_relaxed);
    }

    std::vector<std::thread> threads;
    threads.reserve(num_threads);
    
    const size_t chunk = n / static_cast<size_t>(num_threads);

    for (int t = 0; t < num_threads; ++t) {
      const size_t begin = static_cast<size_t>(t) * chunk;
      const size_t end = (t + 1 == num_threads) ? n : begin + chunk;

      threads.emplace_back([&, begin, end]() {
        for (size_t i = begin; i < end; ++i) {
          // Use relaxed memory order for the addition as we only care about the final sum
          // and there are no dependencies between iterations/threads other than the sum itself.
          counts_ptr[static_cast<size_t>(keys[i])].fetch_add(static_cast<uint64_t>(values[i]), std::memory_order_relaxed);
        }
      });
    }

    for (auto& th : threads) {
      th.join();
    }

    // Convert atomics to uint64_t for checksum calculation
    std::vector<uint64_t> counts_vec(static_cast<size_t>(key_space));
    for (size_t i = 0; i < static_cast<size_t>(key_space); ++i) {
      counts_vec[i] = counts_ptr[i].load(std::memory_order_relaxed);
    }
    final_hash = checksum_u64(counts_vec);
  }

  return final_hash;
}