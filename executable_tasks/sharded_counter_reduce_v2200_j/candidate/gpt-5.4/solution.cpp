#include "interface.h"

#include <atomic>
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
  const std::size_t n = keys.size();
  std::vector<uint64_t> counts(static_cast<std::size_t>(key_space), 0);

  if (iters <= 0) {
    return 0;
  }

  if (key_space == 0) {
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
      hash = checksum_u64(counts);
    }
    return hash;
  }

  if (n == 0 || num_threads <= 1) {
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

  int workers = num_threads;
  if (workers < 1) workers = 1;
  if (static_cast<std::size_t>(workers) > n) workers = static_cast<int>(n);
  if (workers <= 1) {
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

  const std::size_t ks = static_cast<std::size_t>(key_space);
  std::vector<std::vector<uint64_t>> locals(static_cast<std::size_t>(workers),
                                            std::vector<uint64_t>(ks, 0));
  std::vector<std::size_t> begins(static_cast<std::size_t>(workers));
  std::vector<std::size_t> ends(static_cast<std::size_t>(workers));

  const std::size_t base = n / static_cast<std::size_t>(workers);
  const std::size_t rem = n % static_cast<std::size_t>(workers);
  std::size_t pos = 0;
  for (int t = 0; t < workers; ++t) {
    const std::size_t len = base + (static_cast<std::size_t>(t) < rem ? 1u : 0u);
    begins[static_cast<std::size_t>(t)] = pos;
    ends[static_cast<std::size_t>(t)] = pos + len;
    pos += len;
  }

  std::atomic<int> phase{0};
  std::atomic<int> done_count{0};
  std::atomic<bool> stop{false};
  std::vector<std::thread> threads;
  threads.reserve(static_cast<std::size_t>(workers));

  for (int t = 0; t < workers; ++t) {
    threads.emplace_back([&, t]() {
      const std::size_t tid = static_cast<std::size_t>(t);
      std::vector<uint64_t>& local = locals[tid];
      int seen_phase = 0;
      for (;;) {
        int p;
        while ((p = phase.load(std::memory_order_acquire)) == seen_phase) {
          if (stop.load(std::memory_order_acquire)) {
            return;
          }
          std::this_thread::yield();
        }
        if (stop.load(std::memory_order_acquire)) {
          return;
        }
        seen_phase = p;

        std::fill(local.begin(), local.end(), 0);
        const std::size_t begin = begins[tid];
        const std::size_t end = ends[tid];
        for (std::size_t i = begin; i < end; ++i) {
          local[static_cast<std::size_t>(keys[i])] += static_cast<uint64_t>(values[i]);
        }

        done_count.fetch_add(1, std::memory_order_release);
      }
    });
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    done_count.store(0, std::memory_order_relaxed);
    phase.fetch_add(1, std::memory_order_release);

    while (done_count.load(std::memory_order_acquire) != workers) {
      std::this_thread::yield();
    }

    std::fill(counts.begin(), counts.end(), 0);
    for (int t = 0; t < workers; ++t) {
      const std::vector<uint64_t>& local = locals[static_cast<std::size_t>(t)];
      for (std::size_t k = 0; k < ks; ++k) {
        counts[k] += local[k];
      }
    }

    hash = checksum_u64(counts);
  }

  stop.store(true, std::memory_order_release);
  phase.fetch_add(1, std::memory_order_release);
  for (std::thread& th : threads) {
    th.join();
  }

  return hash;
}
