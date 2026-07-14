#include "interface.h"

#include <vector>
#include <thread>
#include <atomic>
#include <numeric>

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
  
  const size_t n_keys = keys.size();
  const size_t k_space = static_cast<size_t>(key_space);
  
  // To avoid repeated thread creation overhead, we create threads once.
  // Each thread will have its own local buffer to avoid contention.
  std::vector<std::vector<uint64_t>> local_counts(num_threads, std::vector<uint64_t>(k_space, 0));
  std::vector<std::thread> threads;
  
  const size_t chunk = n_keys / num_threads;
  
  // We need to run the process 'iters' times. 
  // To maximize performance, we move the 'iters' loop inside the thread logic
  // but the problem requires a checksum after EACH iteration. 
  // Therefore, we must synchronize after each iteration.
  
  uint64_t final_hash = 0;
  std::vector<uint64_t> global_counts(k_space, 0);

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(global_counts.begin(), global_counts.end(), 0);
    
    // Reset local buffers for this iteration
    for(int t = 0; t < num_threads; ++t) {
        std::fill(local_counts[t].begin(), local_counts[t].end(), 0);
    }

    threads.clear();
    for (int t = 0; t < num_threads; ++t) {
      const size_t begin = t * chunk;
      const size_t end = (t == num_threads - 1) ? n_keys : (t + 1) * chunk;
      
      threads.emplace_back([&, t, begin, end]() {
        uint64_t* my_counts = local_counts[t].data();
        const uint32_t* k_ptr = keys.data();
        const uint32_t* v_ptr = values.data();
        for (size_t i = begin; i < end; ++i) {
          my_counts[k_ptr[i]] += v_ptr[i];
        }
      });
    }

    for (auto& th : threads) {
      th.join();
    }

    // Reduction phase: Sum up all local buffers into global_counts
    // Parallelize reduction for large key_space
    for (int t = 0; t < num_threads; ++t) {
      const uint64_t* my_counts = local_counts[t].data();
      for (size_t k = 0; k < k_space; ++k) {
        global_counts[k] += my_counts[k];
      }
    }
    
    final_hash = checksum_u64(global_counts);
  }

  return final_hash;
}