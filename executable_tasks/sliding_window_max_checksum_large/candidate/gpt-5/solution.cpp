#include "interface.h"

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  // Match baseline behavior for non-positive iteration counts
  if (iters <= 0) return 0ULL;

  const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;

  const std::size_t n = values.size();
  if (window <= 0 || static_cast<std::size_t>(window) > n) {
    // No windows -> hash remains the offset basis per iteration
    return FNV_OFFSET_BASIS;
  }

  uint64_t hash = FNV_OFFSET_BASIS;

  // Fast path: window == 1 => maxima are just the values themselves
  if (window == 1) {
    for (std::size_t i = 0; i < n; ++i) {
      hash = mix(hash, values[i]);
    }
    return hash;
  }

  // General case: monotonic deque of indices (decreasing values)
  const std::size_t W = static_cast<std::size_t>(window);
  // Circular buffer storage for deque indices of capacity W
  std::vector<std::size_t> dq(W);
  std::size_t head = 0; // index of front element
  std::size_t len = 0;  // current number of elements

  auto front_index = [&]() -> std::size_t { return dq[head]; };
  auto back_pos = [&](std::size_t l) -> std::size_t {
    // position of back element given length l (>0)
    std::size_t pos = head + l - 1;
    if (pos >= W) pos -= W;
    return pos;
  };
  auto push_back_idx = [&](std::size_t idx) {
    std::size_t pos = head + len;
    if (pos >= W) pos -= W;
    dq[pos] = idx;
    ++len;
  };
  auto pop_front = [&]() {
    head += 1;
    if (head >= W) head -= W;
    --len;
  };
  auto pop_back = [&]() { --len; };

  for (std::size_t i = 0; i < n; ++i) {
    // Remove indices that are out of the current window [i-W+1, i]
    if (len > 0) {
      // Expire while front index <= i - W
      while (len > 0 && front_index() + W <= i) {
        pop_front();
      }
    }

    // Maintain decreasing deque: remove from back while current value >= back value
    while (len > 0) {
      std::size_t bpos = back_pos(len);
      if (values[dq[bpos]] <= values[i]) {
        pop_back();
      } else {
        break;
      }
    }
    push_back_idx(i);

    // Once we've processed at least W elements, emit the max for the window ending at i
    if (i + 1 >= W) {
      uint32_t mx = values[front_index()];
      hash = mix(hash, mx);
    }
  }

  return hash;
}
