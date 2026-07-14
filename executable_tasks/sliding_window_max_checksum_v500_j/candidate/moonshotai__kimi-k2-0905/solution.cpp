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
  const std::size_t n = values.size();
  const std::size_t w = static_cast<std::size_t>(window);
  if (w == 0 || w > n) return 0;

  std::vector<uint32_t> maxima;
  maxima.reserve(n - w + 1);

  std::deque<std::size_t> dq;

  for (std::size_t i = 0; i < n; ++i) {
    while (!dq.empty() && values[dq.back()] <= values[i])
      dq.pop_back();
    dq.push_back(i);
    if (dq.front() + w <= i)
      dq.pop_front();
    if (i + 1 >= w)
      maxima.push_back(values[dq.front()]);
  }

  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t mx : maxima)
    hash = mix(hash, mx);

  const uint64_t base = hash;
  for (int iter = 1; iter < iters; ++iter) {
    hash = base;
    for (uint32_t mx : maxima)
      hash = mix(hash, mx);
  }
  return hash;
}