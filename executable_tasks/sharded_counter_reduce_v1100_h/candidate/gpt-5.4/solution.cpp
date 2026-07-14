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

  if (ks == 0) {
    uint64_t hash = 1469598103934665603ULL;
    for (int iter = 0; iter < iters; ++iter) {
    }
    return hash;
  }

  if (iters <= 0) {
    return 0;
  }

  if (num_threads <= 1 || n == 0) {
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

  int threads_to_use = num_threads;
  if (static_cast<std::size_t>(threads_to_use) > n) {
    threads_to_use = static_cast<int>(n);
  }
  if (threads_to_use <= 1) {
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

  std::vector<uint64_t> counts(ks, 0);
  std::vector<std::vector<uint64_t>> locals(
      static_cast<std::size_t>(threads_to_use), std::vector<uint64_t>(ks, 0));
  std::vector<std::thread> threads;
  threads.reserve(static_cast<std::size_t>(threads_to_use));

  const std::size_t base = n / static_cast<std::size_t>(threads_to_use);
  const std::size_t rem = n % static_cast<std::size_t>(threads_to_use);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (int t = 0; t < threads_to_use; ++t) {
      std::vector<uint64_t>& local = locals[static_cast<std::size_t>(t)];
      std::fill(local.begin(), local.end(), 0);
    }

    threads.clear();
    std::size_t begin = 0;
    for (int t = 0; t < threads_to_use; ++t) {
      const std::size_t len = base + (static_cast<std::size_t>(t) < rem ? 1u : 0u);
      const std::size_t end = begin + len;
      threads.emplace_back([&, t, begin, end]() {
        std::vector<uint64_t>& local = locals[static_cast<std::size_t>(t)];
        for (std::size_t i = begin; i < end; ++i) {
          local[static_cast<std::size_t>(keys[i])] += static_cast<uint64_t>(values[i]);
        }
      });
      begin = end;
    }

    for (std::thread& th : threads) {
      th.join();
    }

    std::fill(counts.begin(), counts.end(), 0);
    for (int t = 0; t < threads_to_use; ++t) {
      const std::vector<uint64_t>& local = locals[static_cast<std::size_t>(t)];
      for (std::size_t k = 0; k < ks; ++k) {
        counts[k] += local[k];
      }
    }

    hash = checksum_u64(counts);
  }

  return hash;
}
