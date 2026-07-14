#include "interface.h"

#include <algorithm>
#include <cstdint>
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
  const std::size_t ks = static_cast<std::size_t>(key_space);
  const std::size_t n_threads = static_cast<std::size_t>(num_threads);
  const std::size_t total_keys = keys.size();
  const std::size_t chunk = total_keys / n_threads;

  std::vector<uint64_t> local_counts(n_threads * ks, 0);
  std::vector<uint64_t> counts(ks);

  const uint32_t* keys_ptr = keys.data();
  const uint32_t* values_ptr = values.data();
  uint64_t* local_ptr = local_counts.data();

  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(local_ptr, local_ptr + n_threads * ks, 0);

    std::vector<std::thread> threads;
    threads.reserve(n_threads);

    for (std::size_t t = 0; t < n_threads; ++t) {
      const std::size_t begin = t * chunk;
      const std::size_t end = (t + 1 == n_threads) ? total_keys : begin + chunk;
      const std::size_t offset = t * ks;

      threads.emplace_back([begin, end, offset, keys_ptr, values_ptr, local_ptr]() {
        for (std::size_t i = begin; i < end; ++i) {
          local_ptr[offset + static_cast<std::size_t>(keys_ptr[i])] += static_cast<uint64_t>(values_ptr[i]);
        }
      });
    }

    for (std::thread& th : threads) {
      th.join();
    }

    std::fill(counts.begin(), counts.end(), 0);
    for (std::size_t t = 0; t < n_threads; ++t) {
      const std::size_t offset = t * ks;
      for (std::size_t k = 0; k < ks; ++k) {
        counts[k] += local_ptr[offset + k];
      }
    }

    hash = checksum_u64(counts);
  }

  return hash;
}