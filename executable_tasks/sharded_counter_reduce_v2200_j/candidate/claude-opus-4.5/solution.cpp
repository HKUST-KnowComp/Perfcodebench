#include "interface.h"

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
  const std::size_t nt = static_cast<std::size_t>(num_threads);
  const std::size_t n = keys.size();
  const uint32_t* __restrict kptr = keys.data();
  const uint32_t* __restrict vptr = values.data();

  std::vector<uint64_t> counts(ks);
  std::vector<std::vector<uint64_t>> local_counts(nt);
  for (std::size_t t = 0; t < nt; ++t) {
    local_counts[t].resize(ks);
  }

  uint64_t hash = 0;
  const std::size_t chunk = n / nt;

  for (int iter = 0; iter < iters; ++iter) {
    // Clear local counts
    for (std::size_t t = 0; t < nt; ++t) {
      std::fill(local_counts[t].begin(), local_counts[t].end(), 0);
    }

    std::vector<std::thread> threads;
    threads.reserve(nt);

    for (std::size_t t = 0; t < nt; ++t) {
      const std::size_t begin = t * chunk;
      const std::size_t end = (t + 1 == nt) ? n : begin + chunk;
      uint64_t* __restrict lc = local_counts[t].data();
      threads.emplace_back([kptr, vptr, lc, begin, end]() {
        for (std::size_t i = begin; i < end; ++i) {
          lc[static_cast<std::size_t>(kptr[i])] += static_cast<uint64_t>(vptr[i]);
        }
      });
    }

    for (std::thread& th : threads) {
      th.join();
    }

    // Merge local counts into global counts
    std::fill(counts.begin(), counts.end(), 0);
    for (std::size_t t = 0; t < nt; ++t) {
      const uint64_t* __restrict lc = local_counts[t].data();
      for (std::size_t k = 0; k < ks; ++k) {
        counts[k] += lc[k];
      }
    }

    hash = checksum_u64(counts);
  }
  return hash;
}
