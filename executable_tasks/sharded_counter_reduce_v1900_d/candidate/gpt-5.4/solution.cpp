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

struct alignas(64) ThreadLocalCounts {
  std::vector<uint64_t> counts;
};

}  // namespace

uint64_t sharded_counter_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int num_threads,
    int iters) {
  const std::size_t n = keys.size();
  const std::size_t ks = static_cast<std::size_t>(key_space);

  if (values.size() != n) {
    return 0;
  }
  if (iters <= 0) {
    return checksum_u64(std::vector<uint64_t>(ks, 0));
  }
  if (num_threads <= 0) {
    num_threads = 1;
  }
  if (n == 0) {
    return checksum_u64(std::vector<uint64_t>(ks, 0));
  }

  const int actual_threads = static_cast<int>(n < static_cast<std::size_t>(num_threads)
                                                  ? static_cast<int>(n)
                                                  : num_threads);

  std::vector<uint64_t> final_counts(ks, 0);
  std::vector<ThreadLocalCounts> locals(static_cast<std::size_t>(actual_threads));
  for (int t = 0; t < actual_threads; ++t) {
    locals[static_cast<std::size_t>(t)].counts.assign(ks, 0);
  }

  std::vector<std::thread> threads;
  threads.reserve(static_cast<std::size_t>(actual_threads));

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (int t = 0; t < actual_threads; ++t) {
      std::vector<uint64_t>& local = locals[static_cast<std::size_t>(t)].counts;
      std::fill(local.begin(), local.end(), 0);
    }

    const std::size_t chunk = n / static_cast<std::size_t>(actual_threads);
    const std::size_t rem = n % static_cast<std::size_t>(actual_threads);

    threads.clear();
    for (int t = 0; t < actual_threads; ++t) {
      const std::size_t tt = static_cast<std::size_t>(t);
      const std::size_t begin = tt * chunk + (tt < rem ? tt : rem);
      const std::size_t end = begin + chunk + (tt < rem ? 1 : 0);
      threads.emplace_back([&, t, begin, end]() {
        std::vector<uint64_t>& local = locals[static_cast<std::size_t>(t)].counts;
        for (std::size_t i = begin; i < end; ++i) {
          local[static_cast<std::size_t>(keys[i])] += static_cast<uint64_t>(values[i]);
        }
      });
    }
    for (std::thread& th : threads) {
      th.join();
    }

    std::fill(final_counts.begin(), final_counts.end(), 0);
    for (int t = 0; t < actual_threads; ++t) {
      const std::vector<uint64_t>& local = locals[static_cast<std::size_t>(t)].counts;
      for (std::size_t k = 0; k < ks; ++k) {
        final_counts[k] += local[k];
      }
    }

    hash = checksum_u64(final_counts);
  }

  return hash;
}
