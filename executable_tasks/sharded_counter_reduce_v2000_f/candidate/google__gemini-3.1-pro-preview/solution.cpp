#include "interface.h"

#include <thread>
#include <vector>
#include <algorithm>
#include <cstdint>

namespace {

// Standard FNV-1a hash for checksumming the final state
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
  
  const std::size_t n = keys.size();
  const std::size_t ks = static_cast<std::size_t>(key_space);
  std::vector<uint64_t> global_counts(ks, 0);
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(global_counts.begin(), global_counts.end(), 0);
    
    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    // To avoid contention and false sharing, each thread works on its own local counter array.
    // This is feasible if key_space is reasonably sized. 
    // If key_space is extremely large, a different sharding strategy (atomic or mutex-per-bucket) would be needed.
    auto worker = [&](std::size_t begin, std::size_t end, std::vector<uint64_t>* local_out) {
      std::vector<uint64_t> local_counts(ks, 0);
      for (std::size_t i = begin; i < end; ++i) {
        local_counts[keys[i]] += values[i];
      }
      *local_out = std::move(local_counts);
    };

    std::vector<std::vector<uint64_t>> thread_locals(num_threads);
    const std::size_t chunk = n / static_cast<std::size_t>(num_threads);

    for (int t = 0; t < num_threads; ++t) {
      const std::size_t begin = static_cast<std::size_t>(t) * chunk;
      const std::size_t end = (t + 1 == num_threads) ? n : begin + chunk;
      threads.emplace_back(worker, begin, end, &thread_locals[t]);
    }

    for (auto& th : threads) {
      th.join();
    }

    // Reduce local counts into global counts
    for (int t = 0; t < num_threads; ++t) {
      const auto& local = thread_locals[t];
      for (std::size_t i = 0; i < ks; ++i) {
        global_counts[i] += local[i];
      }
    }

    final_hash = checksum_u64(global_counts);
  }

  return final_hash;
}