#include "interface.h"

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
  constexpr std::size_t NUM_SHARDS = 256;
  std::vector<uint64_t> counts(static_cast<std::size_t>(key_space), 0);
  std::vector<std::mutex> shard_mutexes(NUM_SHARDS);
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(counts.begin(), counts.end(), 0);
    std::vector<std::thread> threads;
    const std::size_t chunk = keys.size() / static_cast<std::size_t>(num_threads);
    
    for (int t = 0; t < num_threads; ++t) {
      const std::size_t begin = static_cast<std::size_t>(t) * chunk;
      const std::size_t end = (t + 1 == num_threads) ? keys.size() : begin + chunk;
      
      threads.emplace_back([&, begin, end]() {
        std::vector<uint64_t> local_counts(static_cast<std::size_t>(key_space), 0);
        
        for (std::size_t i = begin; i < end; ++i) {
          local_counts[static_cast<std::size_t>(keys[i])] += static_cast<uint64_t>(values[i]);
        }
        
        for (std::size_t k = 0; k < static_cast<std::size_t>(key_space); ++k) {
          if (local_counts[k] != 0) {
            std::size_t shard_idx = k % NUM_SHARDS;
            std::lock_guard<std::mutex> lock(shard_mutexes[shard_idx]);
            counts[k] += local_counts[k];
          }
        }
      });
    }
    
    for (std::thread& th : threads) {
      th.join();
    }
    hash = checksum_u64(counts);
  }
  return hash;
}
