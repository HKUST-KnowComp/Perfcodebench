#include "interface.h"
#include <queue>
#include <cstdint>
#include <vector>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::priority_queue<uint32_t, std::vector<uint32_t>, std::greater<uint32_t>> min_heap;
    uint64_t current_sum = 0;
    const size_t k_size = static_cast<size_t>(k);
    for (uint32_t s : scores) {
      if (min_heap.size() < k_size) {
        min_heap.push(s);
        current_sum += s;
      } else if (s > min_heap.top()) {
        current_sum -= min_heap.top();
        min_heap.pop();
        min_heap.push(s);
        current_sum += s;
      }
    }
    sum = current_sum;
  }
  return sum;
}