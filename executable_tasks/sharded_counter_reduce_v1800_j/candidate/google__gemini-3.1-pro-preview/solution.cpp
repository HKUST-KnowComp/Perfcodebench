#include "interface.h"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>
#include <memory>

namespace {

struct Barrier {
  std::mutex mtx;
  std::condition_variable cv;
  int count;
  int initial_count;
  int generation;

  explicit Barrier(int n) : count(n), initial_count(n), generation(0) {}

  void wait() {
    std::unique_lock<std::mutex> lock(mtx);
    int gen = generation;
    if (--count == 0) {
      generation++;
      count = initial_count;
      cv.notify_all();
    } else {
      cv.wait(lock, [this, gen] { return gen != generation; });
    }
  }
};

uint64_t checksum_u64(const std::unique_ptr<std::atomic<uint64_t>[]>& counts, uint32_t key_space) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t i = 0; i < key_space; ++i) {
    hash ^= counts[i].load(std::memory_order_relaxed);
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
  
  if (num_threads <= 0) return 0;

  auto counts = std::make_unique<std::atomic<uint64_t>[]>(key_space);
  for (uint32_t i = 0; i < key_space; ++i) counts[i].store(0, std::memory_order_relaxed);

  Barrier barrier(num_threads);
  uint64_t final_hash = 0;
  std::vector<std::thread> threads;
  threads.reserve(num_threads);

  const std::size_t total_size = keys.size();
  const std::size_t chunk = total_size / static_cast<std::size_t>(num_threads);

  for (int t = 0; t < num_threads; ++t) {
    threads.emplace_back([&, t]() {
      const std::size_t begin = static_cast<std::size_t>(t) * chunk;
      const std::size_t end = (t + 1 == num_threads) ? total_size : begin + chunk;

      for (int iter = 0; iter < iters; ++iter) {
        // 1. Reset counts (only one thread needs to do this, but others must wait)
        if (t == 0) {
          for (uint32_t i = 0; i < key_space; ++i) {
            counts[i].store(0, std::memory_order_relaxed);
          }
        }
        barrier.wait();

        // 2. Perform updates using atomics
        for (std::size_t i = begin; i < end; ++i) {
          counts[keys[i]].fetch_add(values[i], std::memory_order_relaxed);
        }
        barrier.wait();

        // 3. Compute checksum (only one thread needs to do this)
        if (t == 0) {
          final_hash = checksum_u64(counts, key_space);
        }
        barrier.wait();
      }
    });
  }

  for (auto& th : threads) {
    th.join();
  }

  return final_hash;
}
