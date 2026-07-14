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
  
  const size_t n_keys = keys.size();
  const size_t k_space = static_cast<size_t>(key_space);
  
  // To avoid repeated thread creation overhead, we use a pattern where threads
  // process the iterations. However, the problem requires the checksum to be
  // calculated *after each iteration* of the full dataset update.
  // Therefore, we must synchronize after each iteration.
  
  uint64_t final_hash = 0;
  std::vector<uint64_t> global_counts(k_space);
  
  // Pre-calculate chunks
  std::vector<std::pair<size_t, size_t>> chunks(num_threads);
  size_t chunk_size = n_keys / num_threads;
  for (int t = 0; t < num_threads; ++t) {
    chunks[t].first = t * chunk_size;
    chunks[t].second = (t == num_threads - 1) ? n_keys : (t + 1) * chunk_size;
  }

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(global_counts.begin(), global_counts.end(), 0);
    
    // Use thread-local storage to avoid contention
    std::vector<std::vector<uint64_t>> local_counts(num_threads, std::vector<uint64_t>(k_space, 0));
    std::vector<std::thread> workers;
    
    for (int t = 0; t < num_threads; ++t) {
      workers.emplace_back([&, t]() {
        const size_t start = chunks[t].first;
        const size_t end = chunks[t].second;
        uint64_t* my_counts = local_counts[t].data();
        const uint32_t* k_ptr = keys.data();
        const uint32_t* v_ptr = values.data();
        
        for (size_t i = start; i < end; ++i) {
          my_counts[k_ptr[i]] += v_ptr[i];
        }
      });
    }
    
    for (auto& w : workers) w.join();

    // Reduction phase: Sum local counts into global counts
    // Parallelize reduction for large key_space
    std::vector<std::thread> reducers;
    size_t red_chunk = (k_space + num_threads - 1) / num_threads;
    for (int t = 0; t < num_threads; ++t) {
      reducers.emplace_back([&, t]() {
        size_t start = t * red_chunk;
        size_t end = std::min(start + red_chunk, k_space);
        for (size_t k = start; k < end; ++k) {
          uint64_t sum = 0;
          for (int th = 0; th < num_threads; ++th) {
            sum += local_counts[th][k];
          }
          global_counts[k] = sum;
        }
      });
    }
    for (auto& r : reducers) r.join();

    final_hash = checksum_u64(global_counts);
  }

  return final_hash;
}