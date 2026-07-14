#include "interface.h"

#include <algorithm>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
  const std::size_t n = scores.size();
  if (iters <= 0) return 0;

  // Normalize k to valid range [0, n]
  std::size_t m = 0;
  if (k > 0) {
    m = static_cast<std::size_t>(k);
    if (m > n) m = n;
  }

  // Heuristic threshold: for small/medium k use a min-heap; otherwise use nth_element
  const std::size_t HEAP_THRESHOLD = 1024;

  uint64_t sum = 0;

  // Preallocate buffers once to avoid repeated allocations across iterations
  std::vector<uint32_t> tmp;
  tmp.reserve(n);
  std::vector<uint32_t> heap;
  heap.reserve(m);

  for (int iter = 0; iter < iters; ++iter) {
    if (m == 0) {
      sum = 0;
      continue;
    }

    if (m < n && m <= HEAP_THRESHOLD) {
      // Min-heap approach: O(n log k), no full-array copy
      heap.clear();
      // Build initial heap from first m elements and compute initial sum
      uint64_t local_sum = 0;
      heap.insert(heap.end(), scores.begin(), scores.begin() + m);
      for (std::size_t i = 0; i < m; ++i) local_sum += heap[i];
      std::make_heap(heap.begin(), heap.end(), std::greater<uint32_t>()); // min-heap

      for (std::size_t i = m; i < n; ++i) {
        uint32_t s = scores[i];
        // If current score exceeds min of heap, replace and adjust sum
        if (s > heap.front()) {
          uint32_t old_min = heap.front();
          std::pop_heap(heap.begin(), heap.end(), std::greater<uint32_t>());
          heap.back() = s;
          std::push_heap(heap.begin(), heap.end(), std::greater<uint32_t>());
          local_sum += static_cast<uint64_t>(s) - static_cast<uint64_t>(old_min);
        }
      }
      sum = local_sum;
    } else {
      // nth_element approach: average O(n), requires a copy
      tmp = scores; // uses reserved capacity to avoid reallocations
      if (m == n) {
        uint64_t local_sum = 0;
        for (std::size_t i = 0; i < n; ++i) local_sum += static_cast<uint64_t>(tmp[i]);
        sum = local_sum;
      } else {
        std::nth_element(tmp.begin(), tmp.begin() + static_cast<std::ptrdiff_t>(m), tmp.end(), std::greater<uint32_t>());
        uint64_t local_sum = 0;
        for (std::size_t i = 0; i < m; ++i) local_sum += static_cast<uint64_t>(tmp[i]);
        sum = local_sum;
      }
    }
  }

  return sum;
}
