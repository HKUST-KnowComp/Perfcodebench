#include "interface.h"

#include <atomic>
#include <mutex>
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
  // Use sharded counters - one per thread to avoid contention
  std::vector<std::vector<uint64_t>> shards(num_threads,
    std::vector<uint64_t>(key_space, 0));
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Reset all shards
    for (int t = 0; t < num_threads; ++t) {
      std::fill(shards[t].begin(), shards[t].end(), 0);
    }
    
    std::vector<std::thread> threads;
    const std::size_t chunk = keys.size() / static_cast<std::size_t>(num_threads);
    
    for (int t = 0; t < num_threads; ++t) {
      const std::size_t begin = static_cast<std::size_t>(t) * chunk;
      const std::size_t end = (t + 1 == num_threads) ? keys.size() : begin + chunk;
      
      threads.emplace_back([&shards, t, begin, end, &keys, &values]() {
        for (std::size_t i = begin; i < end; ++i) {
          shards[t][static_cast<std::size_t>(keys[i])] +=
            static_cast<uint64_t>(values[i]);
        }
      });
    }
    
    for (std::thread& th : threads) {
      th.join();
    }
    
    // Aggregate shards and compute checksum
    std::vector<uint64_t> counts(key_space, 0);
    for (int t = 0; t < num_threads; ++t) {
      for (std::size_t i = 0; i < key_space; ++i) {
        counts[i] += shards[t][i];
      }
    }
    
    hash = checksum_u64(counts);
  }
  
  return hash;
}