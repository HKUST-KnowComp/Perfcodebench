#include "interface.h"

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
  if (iters <= 0) return 0;

  const std::size_t n = keys.size();
  const std::size_t ks = static_cast<std::size_t>(key_space);

  std::vector<uint64_t> counts(ks, 0);

  if (ks == 0) {
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
      hash = checksum_u64(counts);
    }
    return hash;
  }

  if (num_threads <= 1 || n == 0) {
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

  int actual_threads = num_threads;
  if (static_cast<std::size_t>(actual_threads) > n) {
    actual_threads = static_cast<int>(n);
    if (actual_threads <= 0) actual_threads = 1;
  }

  std::vector<std::vector<uint64_t>> locals(
      static_cast<std::size_t>(actual_threads), std::vector<uint64_t>(ks, 0));
  std::vector<std::thread> threads;
  threads.reserve(static_cast<std::size_t>(actual_threads));

  uint64_t hash = 0;
  const std::size_t chunk = n / static_cast<std::size_t>(actual_threads);

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(counts.begin(), counts.end(), 0);
    for (int t = 0; t < actual_threads; ++t) {
      std::fill(locals[static_cast<std::size_t>(t)].begin(),
                locals[static_cast<std::size_t>(t)].end(), 0);
    }

    threads.clear();
    for (int t = 0; t < actual_threads; ++t) {
      const std::size_t begin = static_cast<std::size_t>(t) * chunk;
      const std::size_t end = (t + 1 == actual_threads) ? n : begin + chunk;
      threads.emplace_back([&, t, begin, end]() {
        std::vector<uint64_t>& local = locals[static_cast<std::size_t>(t)];
        for (std::size_t i = begin; i < end; ++i) {
          local[static_cast<std::size_t>(keys[i])] += static_cast<uint64_t>(values[i]);
        }
      });
    }
    for (std::thread& th : threads) {
      th.join();
    }

    for (int t = 0; t < actual_threads; ++t) {
      const std::vector<uint64_t>& local = locals[static_cast<std::size_t>(t)];
      for (std::size_t k = 0; k < ks; ++k) {
        counts[k] += local[k];
      }
    }

    hash = checksum_u64(counts);
  }

  return hash;
}
