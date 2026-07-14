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
  // Use std::atomic to allow concurrent updates to different buckets without a global mutex.
  // We use a vector of atomics. Note: std::atomic is not CopyInsertable, so we manage memory carefully.
  // To avoid the overhead of atomic initialization in every loop, we use a single vector of atomics.
  // However, since we need to reset it every iteration, we'll use a vector of uint64_t and 
  // perform atomic operations via std::atomic_ref (C++20) or simply use a vector of std::atomic.
  
  // Since we need to reset the counts every iteration, and std::atomic is not easily resettable via fill,
  // we will use a vector of std::atomic<uint64_t> and use a loop to reset them.
  
  std::vector<std::atomic<uint64_t>> counts(static_cast<std::size_t>(key_space));
  for (size_t i = 0; i < counts.size(); ++i) {
    counts[i].store(0, std::memory_order_relaxed);
  }

  uint64_t final_hash = 0;
  const std::size_t num_keys = keys.size();

  for (int iter = 0; iter < iters; ++iter) {
    // Reset counts for the new iteration
    for (size_t i = 0; i < counts.size(); ++i) {
      counts[i].store(0, std::memory_order_relaxed);
    }

    std::vector<std::thread> threads;
    const std::size_t chunk = num_keys / static_cast<std::size_t>(num_threads);

    for (int t = 0; t < num_threads; ++t) {
      const std::size_t begin = static_cast<std::size_t>(t) * chunk;
      const std::size_t end = (t + 1 == num_threads) ? num_keys : begin + chunk;

      threads.emplace_back([&, begin, end]() {
        for (std::size_t i = begin; i < end; ++i) {
          // Use relaxed memory order as we only care about the final sum after all threads join
          counts[static_cast<std::size_t>(keys[i])].fetch_add(static_cast<uint64_t>(values[i]), std::memory_order_relaxed);
        }
      });
    }

    for (std::thread& th : threads) {
      th.join();
    }

    // Convert atomics to a standard vector for the checksum function
    std::vector<uint64_t> snapshot(static_cast<std::size_t>(key_space));
    for (size_t i = 0; i < snapshot.size(); ++i) {
      snapshot[i] = counts[i].load(std::memory_order_relaxed);
    }
    final_hash = checksum_u64(snapshot);
  }

  return final_hash;
}