#include "interface.h"

#include <thread>
#include <vector>
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
  // or simply process iterations inside the thread. However, the problem requires
  // the checksum to be calculated *after each iteration* of the full dataset.
  // Since the checksum depends on the state after each full pass, we must synchronize.
  
  // To optimize, we give each thread its own local buffer to avoid contention.
  std::vector<std::vector<uint64_t>> thread_local_counts(num_threads, std::vector<uint64_t>(k_space, 0));
  std::vector<uint64_t> global_counts(k_space, 0);
  
  uint64_t final_hash = 0;
  
  // Pre-calculate chunks
  std::vector<std::pair<size_t, size_t>> chunks(num_threads);
  size_t chunk_size = n_keys / num_threads;
  for (int t = 0; t < num_threads; ++t) {
    chunks[t].first = t * chunk_size;
    chunks[t].second = (t == num_threads - 1) ? n_keys : (t + 1) * chunk_size;
  }

  for (int iter = 0; iter < iters; ++iter) {
    // Reset global counts
    std::fill(global_counts.begin(), global_counts.end(), 0);
    
    std::vector<std::thread> workers;
    for (int t = 0; t < num_threads; ++t) {
      workers.emplace_back([&, t]() {
        auto& local_buf = thread_local_counts[t];
        std::fill(local_buf.begin(), local_buf.end(), 0);
        size_t start = chunks[t].first;
        size_t end = chunks[t].second;
        for (size_t i = start; i < end; ++i) {
          local_buf[keys[i]] += values[i];
        }
      });
    }

    for (auto& w : workers) w.join();

    // Reduction phase: Sum local buffers into global
    // This part can also be parallelized if key_space is large
    for (int t = 0; t < num_threads; ++t) {
      const auto& local_buf = thread_local_counts[t];
      for (size_t k = 0; k < k_space; ++k) {
        global_counts[k] += local_buf[k];
      }
    }
    
    final_hash = checksum_u64(global_counts);
  }

  return final_hash;
}