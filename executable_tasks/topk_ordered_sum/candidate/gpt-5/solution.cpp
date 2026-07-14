#include "interface.h"

#include <algorithm>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int /*iters*/) {
  const std::size_t n = scores.size();
  if (k <= 0 || n == 0) return 0;
  std::size_t kk = static_cast<std::size_t>(k);
  if (kk >= n) {
    uint64_t sum = 0;
    for (std::size_t i = 0; i < n; ++i) sum += static_cast<uint64_t>(scores[i]);
    return sum;
  }

  // Heuristic: for relatively small k, a fixed-size min-heap avoids copying the whole array.
  if (kk <= 2048) {
    std::vector<uint32_t> heap;
    heap.reserve(kk);
    std::size_t i = 0;
    for (; i < kk; ++i) heap.push_back(scores[i]);
    std::make_heap(heap.begin(), heap.end(), std::greater<uint32_t>()); // min-heap
    for (; i < n; ++i) {
      uint32_t v = scores[i];
      if (v > heap.front()) {
        std::pop_heap(heap.begin(), heap.end(), std::greater<uint32_t>());
        heap.back() = v;
        std::push_heap(heap.begin(), heap.end(), std::greater<uint32_t>());
      }
    }
    uint64_t sum = 0;
    for (std::size_t j = 0; j < kk; ++j) sum += static_cast<uint64_t>(heap[j]);
    return sum;
  } else {
    // For larger k, nth_element on a copy is typically faster.
    std::vector<uint32_t> tmp(scores.begin(), scores.end());
    std::nth_element(tmp.begin(), tmp.begin() + static_cast<std::ptrdiff_t>(kk), tmp.end(), std::greater<uint32_t>());
    uint64_t sum = 0;
    for (std::size_t i = 0; i < kk; ++i) sum += static_cast<uint64_t>(tmp[i]);
    return sum;
  }
}
