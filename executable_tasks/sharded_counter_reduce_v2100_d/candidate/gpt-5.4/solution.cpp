#include "interface.h"

#include <cstdint>
#include <thread>
#include <vector>

namespace {

static inline uint64_t checksum_u64(const std::vector<uint64_t>& counts) {
  uint64_t hash = 1469598103934665603ULL;
  const std::size_t n = counts.size();
  for (std::size_t i = 0; i < n; ++i) {
    hash ^= counts[i];
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

  if (iters <= 0) {
    std::vector<uint64_t> empty_counts(ks, 0);
    return checksum_u64(empty_counts);
  }

  if (num_threads <= 1 || n == 0 || ks == 0) {
    std::vector<uint64_t> counts(ks, 0);
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
      std::fill(counts.begin(), counts.end(), 0);
      for (std::size_t i = 0; i < n; ++i) {
        counts[static_cast<std::size_t>(keys[i])] += static_cast<uint64_t>(values[i]);
      }
      hash = checksum_u64(counts);
    }
    return hash;
  }

  const int threads_count = num_threads;
  std::vector<std::vector<uint64_t> > local_counts;
  local_counts.reserve(static_cast<std::size_t>(threads_count));
  for (int t = 0; t < threads_count; ++t) {
    local_counts.emplace_back(ks, 0);
  }

  std::vector<uint64_t> counts(ks, 0);
  std::vector<std::thread> threads;
  threads.reserve(static_cast<std::size_t>(threads_count));

  std::vector<std::size_t> begins(static_cast<std::size_t>(threads_count));
  std::vector<std::size_t> ends(static_cast<std::size_t>(threads_count));
  {
    const std::size_t base = n / static_cast<std::size_t>(threads_count);
    const std::size_t rem = n % static_cast<std::size_t>(threads_count);
    std::size_t pos = 0;
    for (int t = 0; t < threads_count; ++t) {
      const std::size_t len = base + (static_cast<std::size_t>(t) < rem ? 1u : 0u);
      begins[static_cast<std::size_t>(t)] = pos;
      pos += len;
      ends[static_cast<std::size_t>(t)] = pos;
    }
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (int t = 0; t < threads_count; ++t) {
      std::vector<uint64_t>& lc = local_counts[static_cast<std::size_t>(t)];
      std::fill(lc.begin(), lc.end(), 0);
    }

    threads.clear();
    for (int t = 0; t < threads_count; ++t) {
      const std::size_t begin = begins[static_cast<std::size_t>(t)];
      const std::size_t end = ends[static_cast<std::size_t>(t)];
      threads.emplace_back([&, t, begin, end]() {
        std::vector<uint64_t>& lc = local_counts[static_cast<std::size_t>(t)];
        for (std::size_t i = begin; i < end; ++i) {
          lc[static_cast<std::size_t>(keys[i])] += static_cast<uint64_t>(values[i]);
        }
      });
    }
    for (std::thread& th : threads) {
      th.join();
    }

    std::fill(counts.begin(), counts.end(), 0);
    for (int t = 0; t < threads_count; ++t) {
      const std::vector<uint64_t>& lc = local_counts[static_cast<std::size_t>(t)];
      for (std::size_t k = 0; k < ks; ++k) {
        counts[k] += lc[k];
      }
    }

    hash = checksum_u64(counts);
  }

  return hash;
}
