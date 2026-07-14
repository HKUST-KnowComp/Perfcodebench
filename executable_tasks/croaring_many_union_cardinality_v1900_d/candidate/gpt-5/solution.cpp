#include "interface.h"

#include <queue>
#include <cstddef>

struct HeapNode {
  uint32_t value;
  uint32_t shard;
};

struct MinByValue {
  bool operator()(const HeapNode& a, const HeapNode& b) const {
    if (a.value != b.value) return a.value > b.value; // min-heap by value
    return a.shard > b.shard; // tie-breaker for determinism
  }
};

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  const size_t shard_count = offsets.size() == 0 ? 0 : (offsets.size() - 1);
  if (shard_count == 0) return 0;

  // Fast path for a single shard: count unique in a sorted array
  if (shard_count == 1) {
    const uint32_t begin = offsets[0];
    const uint32_t end = offsets[1];
    if (begin >= end) return 0;
    const uint32_t* data = values.data();
    uint64_t cnt = 1; // at least one element
    uint32_t prev = data[begin];
    for (uint32_t i = begin + 1; i < end; ++i) {
      uint32_t v = data[i];
      if (v != prev) {
        ++cnt;
        prev = v;
      }
    }
    return cnt;
  }

  const uint32_t* data = values.data();

  // Maintain per-shard indices and ends
  std::vector<uint32_t> idx(shard_count);
  std::vector<uint32_t> end(shard_count);

  std::priority_queue<HeapNode, std::vector<HeapNode>, MinByValue> heap;
  heap = std::priority_queue<HeapNode, std::vector<HeapNode>, MinByValue>(MinByValue(), {});
  // Initialize heap with the first distinct value from each shard
  for (size_t s = 0; s < shard_count; ++s) {
    uint32_t b = offsets[s];
    uint32_t e = offsets[s + 1];
    idx[s] = b;
    end[s] = e;
    if (b < e) {
      uint32_t v = data[b];
      // Skip duplicates within the shard for this starting value
      uint32_t j = b + 1;
      while (j < e && data[j] == v) {
        ++j;
      }
      idx[s] = j; // next candidate index for this shard
      heap.push(HeapNode{v, static_cast<uint32_t>(s)});
    }
  }

  uint64_t count = 0;
  uint32_t last = 0;
  bool have_last = false;

  while (!heap.empty()) {
    HeapNode node = heap.top();
    heap.pop();
    uint32_t v = node.value;
    if (!have_last || v != last) {
      ++count;
      last = v;
      have_last = true;
    }
    // Advance this shard to its next distinct value and push if available
    uint32_t s = node.shard;
    uint32_t i = idx[s];
    uint32_t e = end[s];
    if (i < e) {
      uint32_t nv = data[i];
      uint32_t j = i + 1;
      while (j < e && data[j] == nv) {
        ++j;
      }
      idx[s] = j;
      heap.push(HeapNode{nv, s});
    }
  }

  return count;
}
