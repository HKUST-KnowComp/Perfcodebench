#include "interface.h"

#include <vector>
#include <thread>
#include <numeric>
#include <atomic>

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
  
  // To avoid repeated thread creation overhead, we use a barrier-like synchronization
  // or simply process the iterations inside the threads if the logic allows.
  // However, the checksum is calculated per iteration, so we need a way to sync.
  
  // Pre-allocate local buffers for each thread to avoid contention
  std::vector<std::vector<uint64_t>> local_counts(num_threads, std::vector<uint64_t>(k_space, 0));
  std::vector<std::thread> threads;
  std::atomic<int> iteration_counter{0};
  std::atomic<bool> stop_flag{false};

  // We use a simple spin-wait/barrier mechanism to handle the 'iters' loop
  // but since the checksum is required per iteration, we must synchronize.
  // To keep it simple and fast, we'll use a more direct approach: 
  // The baseline creates threads inside the loop. We can optimize by
  // processing the iterations in a way that minimizes overhead.

  uint64_t final_hash = 0;
  std::vector<uint64_t> global_counts(k_space);

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(global_counts.begin(), global_counts.end(), 0);
    
    std::vector<std::thread> workers;
    const size_t chunk = n_keys / num_threads;
    
    for (int t = 0; t < num_threads; ++t) {
      const size_t begin = t * chunk;
      const size_t end = (t == num_threads - 1) ? n_keys : begin + chunk;
      
      workers.emplace_back([&, t, begin, end]() {
        uint64_t* my_counts = local_counts[t].data();
        std::fill(my_counts, my_counts + k_space, 0);
        for (size_t i = begin; i < end; ++i) {
          my_counts[keys[i]] += values[i];
        }
      });
    }

    for (auto& w : workers) w.join();

    // Reduction phase
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