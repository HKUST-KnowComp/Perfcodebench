#include "interface.h"

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <mutex>
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

class ThreadPoolReducer {
 public:
  ThreadPoolReducer(const std::vector<uint32_t>& keys,
                    const std::vector<uint32_t>& values,
                    uint32_t key_space,
                    int num_threads)
      : keys_(keys),
        values_(values),
        key_space_(static_cast<std::size_t>(key_space)),
        num_threads_(num_threads > 0 ? num_threads : 1),
        locals_(static_cast<std::size_t>(num_threads_),
                std::vector<uint64_t>(static_cast<std::size_t>(key_space), 0)) {
    const std::size_t n = keys_.size();
    ranges_begin_.resize(static_cast<std::size_t>(num_threads_));
    ranges_end_.resize(static_cast<std::size_t>(num_threads_));

    const std::size_t chunk = n / static_cast<std::size_t>(num_threads_);
    for (int t = 0; t < num_threads_; ++t) {
      const std::size_t begin = static_cast<std::size_t>(t) * chunk;
      const std::size_t end = (t + 1 == num_threads_) ? n : begin + chunk;
      ranges_begin_[static_cast<std::size_t>(t)] = begin;
      ranges_end_[static_cast<std::size_t>(t)] = end;
    }

    workers_.reserve(static_cast<std::size_t>(num_threads_));
    for (int t = 0; t < num_threads_; ++t) {
      workers_.emplace_back(&ThreadPoolReducer::worker_loop, this, t);
    }
  }

  ~ThreadPoolReducer() {
    {
      std::lock_guard<std::mutex> lock(mu_);
      stop_ = true;
      ++job_epoch_;
    }
    cv_start_.notify_all();
    for (std::thread& th : workers_) {
      th.join();
    }
  }

  void run_iteration(std::vector<uint64_t>& counts) {
    {
      std::lock_guard<std::mutex> lock(mu_);
      finished_ = 0;
      ++job_epoch_;
    }
    cv_start_.notify_all();

    {
      std::unique_lock<std::mutex> lock(mu_);
      cv_done_.wait(lock, [&] { return finished_ == num_threads_; });
    }

    std::fill(counts.begin(), counts.end(), 0);

    const std::size_t ks = key_space_;
    for (int t = 0; t < num_threads_; ++t) {
      const std::vector<uint64_t>& local = locals_[static_cast<std::size_t>(t)];
      for (std::size_t k = 0; k < ks; ++k) {
        counts[k] += local[k];
      }
    }
  }

 private:
  void worker_loop(int tid) {
    int seen_epoch = 0;
    const std::size_t t = static_cast<std::size_t>(tid);
    std::vector<uint64_t>& local = locals_[t];
    const std::size_t begin = ranges_begin_[t];
    const std::size_t end = ranges_end_[t];

    for (;;) {
      {
        std::unique_lock<std::mutex> lock(mu_);
        cv_start_.wait(lock, [&] { return stop_ || job_epoch_ != seen_epoch; });
        if (stop_) {
          return;
        }
        seen_epoch = job_epoch_;
      }

      std::fill(local.begin(), local.end(), 0);
      for (std::size_t i = begin; i < end; ++i) {
        local[static_cast<std::size_t>(keys_[i])] +=
            static_cast<uint64_t>(values_[i]);
      }

      {
        std::lock_guard<std::mutex> lock(mu_);
        ++finished_;
        if (finished_ == num_threads_) {
          cv_done_.notify_one();
        }
      }
    }
  }

  const std::vector<uint32_t>& keys_;
  const std::vector<uint32_t>& values_;
  std::size_t key_space_;
  int num_threads_;

  std::vector<std::thread> workers_;
  std::vector<std::vector<uint64_t>> locals_;
  std::vector<std::size_t> ranges_begin_;
  std::vector<std::size_t> ranges_end_;

  std::mutex mu_;
  std::condition_variable cv_start_;
  std::condition_variable cv_done_;
  int job_epoch_ = 0;
  int finished_ = 0;
  bool stop_ = false;
};

}  // namespace

uint64_t sharded_counter_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int num_threads,
    int iters) {
  std::vector<uint64_t> counts(static_cast<std::size_t>(key_space), 0);
  if (iters <= 0) {
    return 0;
  }

  ThreadPoolReducer reducer(keys, values, key_space, num_threads);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    reducer.run_iteration(counts);
    hash = checksum_u64(counts);
  }
  return hash;
}
