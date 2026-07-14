#include "interface.h"

#include <atomic>
#include <thread>
#include <vector>

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
  const std::size_t kspace = static_cast<std::size_t>(key_space);
  std::vector<uint64_t> counts(kspace, 0);

  // one atomic counter per bucket per thread
  std::vector<std::vector<std::atomic<uint64_t>>> thread_counts(
      static_cast<std::size_t>(num_threads),
      std::vector<std::atomic<uint64_t>>(kspace));

  const std::size_t n = keys.size();
  const std::size_t chunk = (n + static_cast<std::size_t>(num_threads) - 1) /
                            static_cast<std::size_t>(num_threads);

  for (int iter = 0; iter < iters; ++iter) {
    // reset thread-local atomics
    for (int t = 0; t < num_threads; ++t) {
      for (std::size_t k = 0; k < kspace; ++k) {
        thread_counts[static_cast<std::size_t>(t)][k].store(0,
                                                            std::memory_order_relaxed);
      }
    }

    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads; ++t) {
      const std::size_t begin = static_cast<std::size_t>(t) * chunk;
      const std::size_t end = std::min(begin + chunk, n);
      threads.emplace_back([&, t, begin, end]() {
        auto& local = thread_counts[static_cast<std::size_t>(t)];
        for (std::size_t i = begin; i < end; ++i) {
          uint32_t k = keys[i];
          uint32_t v = values[i];
          local[k].fetch_add(static_cast<uint64_t>(v), std::memory_order_relaxed);
        }
      });
    }
    for (auto& th : threads) th.join();

    // merge into single counts vector
    for (std::size_t k = 0; k < kspace; ++k) {
      uint64_t sum = 0;
      for (int t = 0; t < num_threads; ++t) {
        sum += thread_counts[static_cast<std::size_t>(t)][k].load(
            std::memory_order_relaxed);
      }
      counts[k] = sum;
    }
  }

  return checksum_u64(counts);
}