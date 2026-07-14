#include "interface.h"

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <thread>
#include <vector>

namespace {

constexpr std::size_t STRIPE = 64;

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
  const std::size_t nkeys = keys.size();
  const std::size_t ksize = static_cast<std::size_t>(key_space);
  std::vector<uint64_t> counts(ksize, 0);

  const std::size_t stripes = (ksize + STRIPE - 1) / STRIPE;
  std::vector<std::mutex> stripe_mutex(stripes);

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(counts.begin(), counts.end(), 0);

    const std::size_t chunk = (nkeys + num_threads - 1) / static_cast<std::size_t>(num_threads);
    std::vector<std::thread> threads;
    threads.reserve(static_cast<std::size_t>(num_threads));

    for (int t = 0; t < num_threads; ++t) {
      const std::size_t begin = t * chunk;
      if (begin >= nkeys) break;
      const std::size_t end = std::min(begin + chunk, nkeys);

      threads.emplace_back([&, begin, end]() {
        std::vector<uint64_t> local(ksize, 0);
        for (std::size_t i = begin; i < end; ++i) {
          local[static_cast<std::size_t>(keys[i])] += static_cast<uint64_t>(values[i]);
        }
        for (std::size_t s = 0; s < stripes; ++s) {
          const std::size_t base = s * STRIPE;
          const std::size_t top = std::min(base + STRIPE, ksize);
          bool any = false;
          for (std::size_t k = base; k < top; ++k) {
            if (local[k] != 0) {
              any = true;
              break;
            }
          }
          if (!any) continue;
          std::lock_guard<std::mutex> lock(stripe_mutex[s]);
          for (std::size_t k = base; k < top; ++k) {
            counts[k] += local[k];
          }
        }
      });
    }

    for (std::thread& th : threads) {
      th.join();
    }
  }

  return checksum_u64(counts);
}