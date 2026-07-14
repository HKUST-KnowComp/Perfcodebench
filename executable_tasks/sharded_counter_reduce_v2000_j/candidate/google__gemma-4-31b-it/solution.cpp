#include "interface.h"

#include <vector>
#include <thread>
#include <atomic>
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
  
  if (iters == 0) return 0;

  const size_t n_keys = keys.size();
  const size_t k_space = static_cast<size_t>(key_space);
  
  // To avoid repeated thread creation overhead, we use a structure where
  // each thread has its own private buffer to avoid contention.
  // We allocate a 2D array: [num_threads][key_space]
  std::vector<std::vector<uint64_t>> thread_local_counts(num_threads, std::vector<uint64_t>(k_space, 0));
  std::vector<uint64_t> global_counts(k_space, 0);
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset buffers for this iteration
    for (int t = 0; t < num_threads; ++t) {
      std::fill(thread_local_counts[t].begin(), thread_local_counts[t].end(), 0);
    }

    std::vector<std::thread> workers;
    const size_t chunk = n_keys / num_threads;

    for (int t = 0; t < num_threads; ++t) {
      const size_t begin = t * chunk;
      const size_t end = (t == num_threads - 1) ? n_keys : (t + 1) * chunk;
      
      workers.emplace_back([&, t, begin, end]() {
        uint64_t* local_buf = thread_local_counts[t].data();
        const uint32_t* k_ptr = &keys[begin];
        const uint32_t* v_ptr = &values[begin];
        const size_t len = end - begin;
        for (size_t i = 0; i < len; ++i) {
          local_buf[k_ptr[i]] += v_ptr[i];
        }
      });
    }

    for (auto& w : workers) w.join();

    // Reduction phase
    std::fill(global_counts.begin(), global_counts.end(), 0);
    for (int t = 0; t < num_threads; ++t) {
      const uint64_t* local_buf = thread_local_counts[t].data();
      for (size_t k = 0; k < k_space; ++k) {
        global_counts[k] += local_buf[k];
      }
    }
    
    final_hash = checksum_u64(global_counts);
  }

  return final_hash;
}