#include "interface.h"
#include <deque>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  if (values.empty() || window <= 0) return 0;
  if (window > (int)values.size()) return 0;

  uint64_t final_hash = 0;
  
  // Pre-calculate the sliding window maxima for one iteration to avoid redundant work
  // since the input 'values' and 'window' do not change across 'iters'.
  std::vector<uint32_t> max_values;
  max_values.reserve(values.size() - window + 1);
  
  std::deque<int> dq;
  for (int i = 0; i < (int)values.size(); ++i) {
    // Remove indices that are out of the current window
    if (!dq.empty() && dq.front() <= i - window) {
      dq.pop_front();
    }
    // Remove elements smaller than the current element from the back
    while (!dq.empty() && values[dq.back()] <= values[i]) {
      dq.pop_back();
    }
    dq.push_back(i);
    
    // The front of the deque is the index of the maximum element for the current window
    if (i >= window - 1) {
      max_values.push_back(values[dq.front()]);
    }
  }

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t mx : max_values) {
      hash = mix(hash, mx);
    }
    final_hash = hash; // The original code resets hash every iter and returns the last one
  }
  
  return final_hash;
}