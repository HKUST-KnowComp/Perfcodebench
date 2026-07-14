#include "interface.h"
#include <thread>
#include <vector>

namespace {

// Exactly the same FNV-1a hash
uint64_t checksum_u64(const std::vector<uint64_t>& counts) {
  uint64_t hash = 14695981039346656037ULL;
  for (uint64_t v : counts) {
    hash ^= v;
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t sharded_counter_checksum(const std::vector<uint32_t>& keys,
                                  const std::vector<uint32_t>& values,
                                  uint32_t key_space,
                                  int num_threads,
                                  int iters) {
  const size_t n = keys.size();
  const size_t shards = static_cast<size_t>(num_threads);
  
  const size_t chunk = (n + shards - 1) / shards;
  
  // pre-build per-thread partitions, so a thread is never on the same cache line as another
  std::vector<std::vector<size_t>> global_partitions;
  global_partitions.reserve(shards);
  for (size_t t = 0; t < shards; ++t) {
    size_t first = t * chunk;
    size_t last  = std::min(first + chunk, n);
    std::vector<size_t> part;
    part.reserve(last - first);
    for (size_t i = first; i < last; ++i) part.push_back(i);
    global_partitions.emplace_back(std::move(part));
  }

  // one master array, never touched concurrently
  std::vector<uint64_t> master(static_cast<size_t>(key_space));
  
  for (int iter = 0; iter < iters; ++iter) {
    // reset master
    std::fill(master.begin(), master.end(), 0);
    
    // give each thread its own local counter bank – completely private memory
    alignas(64) std::vector<std::vector<uint64_t>> per_thread(shards);
    for (auto& buf : per_thread) buf.assign(key_space, 0);

    // the workers only touch their own buffer
    auto worker = [&](size_t tid) {
      auto& cnt = per_thread[tid];
      const auto& idx = global_partitions[tid];
      for (size_t off = idx.size(); off--;) {
        const size_t i  = idx[off];
        cnt[keys[i]]   += values[i];
      }
    };

    std::vector<std::thread> workers;
    workers.reserve(shards);
    for (size_t t = 0; t < shards; ++t) workers.emplace_back(worker, t);
    for (auto& w : workers) w.join();

    // reduce thread-local buffers back into master – again in parallel, each thread a slice of keys
    auto reducer = [&](size_t tid) {
      // pick a stripe to avoid write sharing
      for (size_t key = tid; key < key_space; key += shards) {
        uint64_t x = 0;
        for (const auto& buf : per_thread) x += buf[key];
        master[key] = x;
      }
    };
    workers.clear();
    for (size_t t = 0; t < shards; ++t) workers.emplace_back(reducer, t);
    for (auto& w : workers) w.join();
  }
  
  return checksum_u64(master);
}