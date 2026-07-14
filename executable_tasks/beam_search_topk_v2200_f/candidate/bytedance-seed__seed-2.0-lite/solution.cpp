#include "interface.h"

#include <algorithm>
#include <utility>
#include <vector>

namespace {

bool better(const std::pair<int32_t, int>& a, const std::pair<int32_t, int>& b) {
  return a.first > b.first || (a.first == b.first && a.second < b.second);
}

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
  std::vector<std::pair<int32_t, int>> min_heap;
  min_heap.reserve(static_cast<std::size_t>(topk));
  const auto is_worse = [](const std::pair<int32_t, int>& a, const std::pair<int32_t, int>& b) {
    return better(b, a);
  };
  const int32_t* logits_data = logits.data();
  uint64_t hash = 0;
  const std::size_t vocab_sz = static_cast<std::size_t>(vocab);
  const std::size_t topk_sz = static_cast<std::size_t>(topk);

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int b = 0; b < beams; ++b) {
      min_heap.clear();
      const std::size_t base = static_cast<std::size_t>(b) * vocab_sz;
      for (int v = 0; v < vocab; ++v) {
        const int32_t l = logits_data[base + static_cast<std::size_t>(v)];
        std::pair<int32_t, int> p = {l, v};
        if (min_heap.size() < topk_sz) {
          min_heap.push_back(p);
          std::push_heap(min_heap.begin(), min_heap.end(), is_worse);
        } else if (better(p, min_heap[0])) {
          std::pop_heap(min_heap.begin(), min_heap.end(), is_worse);
          min_heap.pop_back();
          min_heap.push_back(p);
          std::push_heap(min_heap.begin(), min_heap.end(), is_worse);
        }
      }
      std::sort(min_heap.begin(), min_heap.end(), better);
      for (int k = 0; k < topk; ++k) {
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(min_heap[static_cast<std::size_t>(k)].first)));
        hash = mix(hash, static_cast<uint64_t>(min_heap[static_cast<std::size_t>(k)].second));
      }
    }
  }
  return hash;
}