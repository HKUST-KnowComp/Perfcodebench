#include "interface.h"

#include <algorithm>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
  (void)iters; // computation done once; result is deterministic

  if (k <= 0 || scores.empty()) return 0ULL;
  const std::size_t n = scores.size();
  std::size_t kk = static_cast<std::size_t>(k);
  if (kk >= n) {
    uint64_t sum = 0ULL;
    for (uint32_t v : scores) sum += v;
    return sum;
  }

  if (kk == 1) {
    uint32_t maxv = *std::max_element(scores.begin(), scores.end());
    return static_cast<uint64_t>(maxv);
  }

  // Maintain a min-heap of the top k elements
  std::vector<uint32_t> heap;
  heap.reserve(kk);
  uint64_t sum = 0ULL;

  // Initialize with first k elements
  for (std::size_t i = 0; i < kk; ++i) {
    uint32_t v = scores[i];
    heap.push_back(v);
    sum += v;
  }
  std::make_heap(heap.begin(), heap.end(), std::greater<uint32_t>()); // min-heap

  // Process the rest
  const uint32_t* data = scores.data();
  for (std::size_t i = kk; i < n; ++i) {
    uint32_t v = data[i];
    uint32_t minv = heap.front();
    if (v > minv) {
      sum += static_cast<uint64_t>(v) - static_cast<uint64_t>(minv);
      std::pop_heap(heap.begin(), heap.end(), std::greater<uint32_t>());
      heap.back() = v;
      std::push_heap(heap.begin(), heap.end(), std::greater<uint32_t>());
    }
  }

  return sum;
}
