#include "interface.h"

#include <algorithm>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int /*iters*/) {
  if (k <= 0 || scores.empty()) {
    return 0;
  }

  const std::size_t n = scores.size();
  std::size_t kk = static_cast<std::size_t>(k);
  if (kk >= n) {
    uint64_t total = 0;
    for (std::size_t i = 0; i < n; ++i) total += scores[i];
    return total;
  }

  // Maintain a min-heap of the top-k elements and a running sum.
  uint64_t sum = 0;
  std::vector<uint32_t> heap;
  heap.reserve(kk);
  auto comp = std::greater<uint32_t>(); // makes a min-heap with push_heap/pop_heap

  for (std::size_t i = 0; i < n; ++i) {
    uint32_t v = scores[i];
    if (heap.size() < kk) {
      heap.push_back(v);
      std::push_heap(heap.begin(), heap.end(), comp);
      sum += v;
    } else if (v > heap.front()) {
      uint32_t smallest = heap.front();
      std::pop_heap(heap.begin(), heap.end(), comp); // moves smallest to back
      heap.back() = v; // replace smallest with new value
      std::push_heap(heap.begin(), heap.end(), comp);
      sum += static_cast<uint64_t>(v) - static_cast<uint64_t>(smallest);
    }
  }

  return sum;
}
