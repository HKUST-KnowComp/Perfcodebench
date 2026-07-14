#include "interface.h"

#include <atomic>
#include <thread>
#include <vector>
#include <cstring>

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
  
  const std::size_t n = keys.size();
  const std::size_t ks = static_cast<std::size_t>(key_space);
  const int nt = num_threads;
  
  // Global counts
  std::vector<uint64_t> counts(ks, 0);
  
  // Thread-local counts for each thread
  std::vector<std::vector<uint64_t>> local_counts(nt);
  for (int t = 0; t < nt; ++t) {
    local_counts[t].resize(ks, 0);
  }
  
  // Synchronization
  std::atomic<int> phase{0};
  std::atomic<int> ready_count{0};
  std::atomic<int> done_count{0};
  std::atomic<bool> finished{false};
  
  uint64_t hash = 0;
  
  // Compute chunk boundaries
  const std::size_t chunk = n / static_cast<std::size_t>(nt);
  std::vector<std::size_t> begins(nt), ends(nt);
  for (int t = 0; t < nt; ++t) {
    begins[t] = static_cast<std::size_t>(t) * chunk;
    ends[t] = (t + 1 == nt) ? n : begins[t] + chunk;
  }
  
  // Merge chunk boundaries for parallel merge
  const std::size_t merge_chunk = ks / static_cast<std::size_t>(nt);
  std::vector<std::size_t> merge_begins(nt), merge_ends(nt);
  for (int t = 0; t < nt; ++t) {
    merge_begins[t] = static_cast<std::size_t>(t) * merge_chunk;
    merge_ends[t] = (t + 1 == nt) ? ks : merge_begins[t] + merge_chunk;
  }
  
  std::vector<std::thread> threads;
  threads.reserve(nt);
  
  for (int t = 0; t < nt; ++t) {
    threads.emplace_back([&, t]() {
      const std::size_t b = begins[t];
      const std::size_t e = ends[t];
      const std::size_t mb = merge_begins[t];
      const std::size_t me = merge_ends[t];
      std::vector<uint64_t>& lc = local_counts[t];
      const uint32_t* __restrict kptr = keys.data();
      const uint32_t* __restrict vptr = values.data();
      
      int current_phase = 0;
      
      while (!finished.load(std::memory_order_relaxed)) {
        // Wait for work phase
        while (phase.load(std::memory_order_acquire) <= current_phase && 
               !finished.load(std::memory_order_relaxed)) {
          // spin
        }
        if (finished.load(std::memory_order_relaxed)) break;
        
        // Clear local counts
        std::memset(lc.data(), 0, ks * sizeof(uint64_t));
        
        // Accumulate into thread-local counts
        for (std::size_t i = b; i < e; ++i) {
          lc[kptr[i]] += vptr[i];
        }
        
        // Signal ready for merge
        ready_count.fetch_add(1, std::memory_order_release);
        
        // Wait for all threads to be ready
        while (ready_count.load(std::memory_order_acquire) < nt && 
               !finished.load(std::memory_order_relaxed)) {
          // spin
        }
        if (finished.load(std::memory_order_relaxed)) break;
        
        // Parallel merge: each thread merges a portion of key_space
        for (std::size_t k = mb; k < me; ++k) {
          uint64_t sum = 0;
          for (int tt = 0; tt < nt; ++tt) {
            sum += local_counts[tt][k];
          }
          counts[k] = sum;
        }
        
        // Signal done
        done_count.fetch_add(1, std::memory_order_release);
        
        current_phase++;
      }
    });
  }
  
  for (int iter = 0; iter < iters; ++iter) {
    ready_count.store(0, std::memory_order_relaxed);
    done_count.store(0, std::memory_order_relaxed);
    
    // Signal threads to start
    phase.store(iter + 1, std::memory_order_release);
    
    // Wait for all threads to complete merge
    while (done_count.load(std::memory_order_acquire) < nt) {
      // spin
    }
    
    hash = checksum_u64(counts);
  }
  
  finished.store(true, std::memory_order_release);
  phase.store(iters + 2, std::memory_order_release);  // Wake up any waiting threads
  
  for (std::thread& th : threads) {
    th.join();
  }
  
  return hash;
}