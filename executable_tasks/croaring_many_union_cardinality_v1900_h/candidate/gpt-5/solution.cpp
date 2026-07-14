#include "interface.h"

#include <cstdint>
#include <vector>
#include <queue>

struct HeapNode {
  uint32_t value;
  uint32_t shard;
};

struct MinCmp {
  bool operator()(const HeapNode& a, const HeapNode& b) const {
    // Min-heap by value
    return a.value > b.value;
  }
};

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  const size_t nshards = offsets.size() > 0 ? offsets.size() - 1 : 0;
  if (nshards == 0) {
    return 0;
  }

  // Fast path for a single shard: count unique values in a sorted array.
  if (nshards == 1) {
    uint32_t begin = offsets[0];
    uint32_t end = offsets[1];
    if (begin >= end) return 0;
    uint64_t count = 1;
    uint32_t last = values[begin];
    for (uint32_t i = begin + 1; i < end; ++i) {
      uint32_t v = values[i];
      if (v != last) {
        ++count;
        last = v;
      }
    }
    return count;
  }

  // Initialize per-shard cursors and heap with the first element of each non-empty shard.
  std::vector<uint32_t> idx(nshards);
  std::vector<uint32_t> end(nshards);
  std::priority_queue<HeapNode, std::vector<HeapNode>, MinCmp> heap;

  for (uint32_t s = 0; s < nshards; ++s) {
    uint32_t b = offsets[s];
    uint32_t e = offsets[s + 1];
    idx[s] = b;
    end[s] = e;
    if (b < e) {
      heap.push(HeapNode{values[b], s});
    }
  }

  if (heap.empty()) return 0;

  uint64_t count = 0;
  uint32_t last = 0;
  bool have_last = false;

  while (!heap.empty()) {
    HeapNode n = heap.top();
    heap.pop();
    uint32_t v = n.value;
    if (!have_last || v != last) {
      ++count;
      last = v;
      have_last = true;
    }
    uint32_t s = n.shard;
    uint32_t i = ++idx[s];
    if (i < end[s]) {
      heap.push(HeapNode{values[i], s});
    }
  }

  return count;
}
