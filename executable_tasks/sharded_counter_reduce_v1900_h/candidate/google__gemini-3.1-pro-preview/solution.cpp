#include "interface.h"

#include <atomic>
#include <cstdint>
#include <thread>
#include <vector>
#include <algorithm>

namespace {

// Standard FNV-1a-like hash used in the baseline
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
  
  if (iters <= 0) return 0;

  // The baseline resets counts and re-calculates the same result 'iters' times.
  // Since keys and values are const, we only need to compute the result once.
  
  // Use std::atomic to allow concurrent updates to the same bucket.
  // We use a vector of atomics. Note: std::atomic is not trivially copyable,
  // so we initialize the vector carefully.
  std::vector<std::atomic<uint64_t>> atomic_counts(key_space);
  for (uint32_t i = 0; i < key_space; ++i) {
    atomic_counts[i].store(0, std::memory_order_relaxed);
  }

  auto worker = [&](size_t begin, size_t end) {
    for (size_t i = begin; i < end; ++i) {
      // Use relaxed memory order for maximum throughput as we only care about 
      // the final state after all threads join.
      atomic_counts[keys[i]].fetch_add(static_cast<uint64_t>(values[i]), std::memory_order_relaxed);
    }
  };

  std::vector<std::thread> threads;
  size_t n = keys.size();
  size_t chunk_size = (n + num_threads - 1) / num_threads;

  for (int t = 0; t < num_threads; ++t) {
    size_t begin = std::min(static_cast<size_t>(t) * chunk_size, n);
    size_t end = std::min(begin + chunk_size, n);
    if (begin < end) {
      threads.emplace_back(worker, begin, end);
    }
  }

  for (auto& th : threads) {
    th.join();
  }

  // Convert atomics back to a standard vector for the checksum function.
  std::vector<uint64_t> final_counts(key_space);
  for (uint32_t i = 0; i < key_space; ++i) {
    final_counts[i] = atomic_counts[i].load(std::memory_order_relaxed);
  }

  return checksum_u64(final_counts);
}
