#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

struct Entry {
  int32_t score;
  int id;
};

static inline bool better_entry(const Entry& a, const Entry& b) {
  return (a.score > b.score) || (a.score == b.score && a.id < b.id);
}

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
  if (beams <= 0 || vocab <= 0 || topk <= 0 || iters <= 0) {
    return 0;
  }

  if (topk > vocab) topk = vocab;

  std::vector<Entry> heap;
  heap.reserve(static_cast<std::size_t>(topk));
  std::vector<Entry> selected;
  selected.reserve(static_cast<std::size_t>(topk));

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;

    for (int b = 0; b < beams; ++b) {
      const std::size_t base = static_cast<std::size_t>(b) * static_cast<std::size_t>(vocab);
      heap.clear();

      for (int v = 0; v < vocab; ++v) {
        Entry cur{logits[base + static_cast<std::size_t>(v)], v};
        if (static_cast<int>(heap.size()) < topk) {
          heap.push_back(cur);
          std::push_heap(heap.begin(), heap.end(), better_entry);
        } else if (better_entry(cur, heap.front())) {
          std::pop_heap(heap.begin(), heap.end(), better_entry);
          heap.back() = cur;
          std::push_heap(heap.begin(), heap.end(), better_entry);
        }
      }

      selected = heap;
      std::sort(selected.begin(), selected.end(), better_entry);

      for (int k = 0; k < topk; ++k) {
        const Entry& e = selected[static_cast<std::size_t>(k)];
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(e.score)));
        hash = mix(hash, static_cast<uint64_t>(e.id));
      }
    }
  }
  return hash;
}
