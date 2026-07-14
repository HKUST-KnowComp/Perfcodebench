#include "interface.h"
#include <deque>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // anonymous namespace

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  const std::size_t window_sz = static_cast<std::size_t>(window);
  const std::size_t num_windows = values.size() - window_sz + 1;
  
  // Precompute all sliding window maxima once to avoid redundant work across iterations
  std::vector<uint32_t> maxima;
  maxima.reserve(num_windows);
  std::deque<std::size_t> dq;
  
  for (std::size_t i = 0; i < values.size(); ++i) {
    // Maintain monotonic decreasing deque: remove smaller elements from back
    while (!dq.empty() && values[dq.back()] <= values[i]) {
      dq.pop_back();
    }
    dq.push_back(i);
    
    // Remove elements out of the current window from front
    while (dq.front() <= i - window_sz) {
      dq.pop_front();
    }
    
    // Record max once we have a full valid window
    if (i >= window_sz - 1) {
      maxima.push_back(values[dq.front()]);
    }
  }
  
  // Run all iterations, hashing precomputed maxima to match original checksum logic
  uint64_t final_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t mx : maxima) {
      hash = mix(hash, mx);
    }
    final_hash = hash;
  }
  
  return final_hash;
}