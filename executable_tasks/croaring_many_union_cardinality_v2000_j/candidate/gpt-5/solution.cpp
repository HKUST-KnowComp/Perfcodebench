#include "interface.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace {
struct Node {
  uint32_t val;
  uint32_t idx;  // current index into values
  uint32_t end;  // end index (exclusive)
};

struct MinCmp {
  bool operator()(const Node &a, const Node &b) const noexcept {
    return a.val > b.val; // min-heap
  }
};
} // namespace

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  const uint32_t* vals = values.data();
  const std::size_t shards = offsets.size() > 0 ? offsets.size() - 1 : 0;

  // Build initial heap with first element of each non-empty shard
  std::vector<Node> heap;
  heap.reserve(shards);
  for (std::size_t s = 0; s < shards; ++s) {
    uint32_t begin = offsets[s];
    uint32_t end = offsets[s + 1];
    if (begin >= end) continue; // empty shard
    // Push first value from shard
    Node n;
    n.val = vals[begin];
    n.idx = begin + 1;
    n.end = end;
    heap.push_back(n);
  }
  MinCmp cmp;
  std::make_heap(heap.begin(), heap.end(), cmp);

  uint64_t count = 0;
  uint32_t last = 0;
  bool has_last = false;

  while (!heap.empty()) {
    std::pop_heap(heap.begin(), heap.end(), cmp);
    Node n = heap.back();
    heap.pop_back();

    const uint32_t v = n.val;
    if (!has_last || v != last) {
      ++count;
      last = v;
      has_last = true;
    }

    // Advance within this shard, skipping duplicates equal to v
    uint32_t i = n.idx;
    const uint32_t e = n.end;
    while (i < e) {
      uint32_t nv = vals[i++];
      if (nv != v) {
        n.val = nv;
        n.idx = i;
        heap.push_back(n);
        std::push_heap(heap.begin(), heap.end(), cmp);
        break;
      }
    }
    // If i == e or we only saw duplicates equal to v, this shard is exhausted and not reinserted.
  }

  return count;
}
