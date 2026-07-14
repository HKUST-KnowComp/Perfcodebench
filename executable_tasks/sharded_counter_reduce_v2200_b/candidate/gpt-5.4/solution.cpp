#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <thread>
#include <vector>

namespace {

static inline uint64_t checksum_u64(const std::vector<uint64_t>& counts) {
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

  if (values.size() != n) {
    return 0;
  }
  if (iters <= 0) {
    return 0;
  }
  if (ks == 0) {
    return checksum_u64(std::vector<uint64_t>());
  }

  unsigned hw = std::thread::hardware_concurrency();
  if (hw == 0) hw = 1;
  int threads = num_threads;
  if (threads <= 0) threads = 1;
  if (static_cast<unsigned>(threads) > hw * 4u) threads = static_cast<int>(hw * 4u);
  if (n == 0) threads = 1;
  if (static_cast<std::size_t>(threads) > n && n != 0) threads = static_cast<int>(n);
  if (threads <= 0) threads = 1;

  std::vector<uint64_t> counts(ks, 0);
  std::vector<std::vector<uint64_t>> locals;
  locals.resize(static_cast<std::size_t>(threads));
  for (int t = 0; t < threads; ++t) {
    locals[static_cast<std::size_t>(t)].assign(ks, 0);
  }

  std::vector<std::thread> workers;
  workers.reserve(static_cast<std::size_t>(threads));

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (int t = 0; t < threads; ++t) {
      std::fill(locals[static_cast<std::size_t>(t)].begin(),
                locals[static_cast<std::size_t>(t)].end(), 0);
    }

    const std::size_t chunk = n / static_cast<std::size_t>(threads);
    const std::size_t rem = n % static_cast<std::size_t>(threads);

    workers.clear();
    for (int t = 0; t < threads; ++t) {
      const std::size_t tid = static_cast<std::size_t>(t);
      const std::size_t begin = tid * chunk + (tid < rem ? tid : rem);
      const std::size_t end = begin + chunk + (tid < rem ? 1 : 0);
      workers.emplace_back([&, begin, end, tid]() {
        std::vector<uint64_t>& local = locals[tid];
        for (std::size_t i = begin; i < end; ++i) {
          local[static_cast<std::size_t>(keys[i])] += static_cast<uint64_t>(values[i]);
        }
      });
    }
    for (std::thread& th : workers) {
      th.join();
    }

    std::fill(counts.begin(), counts.end(), 0);
    for (int t = 0; t < threads; ++t) {
      const std::vector<uint64_t>& local = locals[static_cast<std::size_t>(t)];
      for (std::size_t k = 0; k < ks; ++k) {
        counts[k] += local[k];
      }
    }

    hash = checksum_u64(counts);
  }

  return hash;
}
