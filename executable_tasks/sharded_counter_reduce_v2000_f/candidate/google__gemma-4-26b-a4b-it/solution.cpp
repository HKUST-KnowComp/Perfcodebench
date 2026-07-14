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
  // We use a vector of atomics. Note: std::atomic is not copyable, so we use a unique_ptr or a vector of a wrapper.
  // To avoid false sharing, one could pad, but for a large key_space, the density is low enough.
  struct AtomicCounter {
    std::atomic<uint64_t> val{0};
  };
  
  // Using a vector of std::atomic<uint64_t> directly is possible if we manage construction carefully.
  // However, std::vector<std::atomic<uint64_t>> is not allowed because atomic is not CopyInsertable.
  // We will use a raw pointer or a vector of a custom struct to manage the memory.
  std::vector<std::atomic<uint64_t>> counts(static_cast<std::size_t>(key_space));
  for(size_t i = 0; i < counts.size(); ++i) counts[i].store(0, std::memory_order_relaxed);

  uint64_t final_hash = 0;
  const std::size_t n = keys.size();
  const std::size_t chunk = n / static_cast<std::size_t>(num_threads);

  for (int iter = 0; iter < iters; ++iter) {
    // Reset counts for this iteration
    for (auto& c : counts) {
      c.store(0, std::memory_order_relaxed);
    }

    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    for (int t = 0; t < num_threads; ++t) {
      const std::size_t begin = static_cast<std::size_t>(t) * chunk;
      const std::size_t end = (t + 1 == num_threads) ? n : begin + chunk;

      threads.emplace_back([&, begin, end]() {
        for (std::size_t i = begin; i < end; ++i) {
          // Use relaxed memory order for the addition as we only care about the final sum
          // and there are no dependencies between iterations/threads other than the final join.
          counts[static_cast<std::size_t>(keys[i])].fetch_add(static_cast<uint64_t>(values[i]), std::memory_order_relaxed);
        }
      });
    }

    for (std::thread& th : threads) {
      th.join();
    }

    // Convert atomics to uint64_t for checksum calculation
    std::vector<uint64_t> snapshot(static_cast<std::size_t>(key_space));
    for (std::size_t i = 0; i < snapshot.size(); ++i) {
      snapshot[i] = counts[i].load(std::memory_order_relaxed);
    }
    final_hash = checksum_u64(snapshot);
  }

  return final_hash;
}