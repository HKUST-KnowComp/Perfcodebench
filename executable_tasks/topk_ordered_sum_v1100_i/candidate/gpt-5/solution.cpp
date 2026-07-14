#include "interface.h"

#include <algorithm>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int /*iters*/) {
  if (k <= 0) return 0;
  const std::size_t n = scores.size();
  std::size_t kk = static_cast<std::size_t>(k);
  if (kk >= n) {
    uint64_t sum = 0;
    const uint32_t* p = scores.data();
    const uint32_t* end = p + n;
    for (; p != end; ++p) sum += static_cast<uint64_t>(*p);
    return sum;
  }

  // Use a min-heap when k is relatively small, otherwise use nth_element.
  if (kk <= 1024 || kk * 16 <= n) {
    // Min-heap of the top k elements.
    std::vector<uint32_t> heap;
    heap.reserve(kk);
    for (std::size_t i = 0; i < kk; ++i) heap.push_back(scores[i]);
    std::make_heap(heap.begin(), heap.end(), std::greater<uint32_t>());

    for (std::size_t i = kk; i < n; ++i) {
      uint32_t x = scores[i];
      if (x > heap.front()) {
        std::pop_heap(heap.begin(), heap.end(), std::greater<uint32_t>());
        heap.back() = x;
        std::push_heap(heap.begin(), heap.end(), std::greater<uint32_t>());
      }
    }

    uint64_t sum = 0;
    for (std::size_t i = 0; i < kk; ++i) sum += static_cast<uint64_t>(heap[i]);
    return sum;
  } else {
    // Partition so that the first k elements are the largest k (unsorted among themselves).
    std::vector<uint32_t> tmp(scores);
    auto first = tmp.begin();
    auto nth = first + static_cast<std::ptrdiff_t>(kk);
    std::nth_element(first, nth, tmp.end(), std::greater<uint32_t>());

    uint64_t sum = 0;
    for (std::size_t i = 0; i < kk; ++i) sum += static_cast<uint64_t>(tmp[i]);
    return sum;
  }
}
