#include "interface.h"

#include <cstdint>
#include <vector>
#include <queue>

struct Node {
  uint32_t value;
  uint32_t shard;
};

struct NodeGreater {
  bool operator()(const Node& a, const Node& b) const noexcept {
    return a.value > b.value;
  }
};

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  const size_t shards = offsets.empty() ? 0 : (offsets.size() - 1);
  if (shards == 0) {
    return 0;
  }

  // Fast path for a single shard: count unique consecutive values
  if (shards == 1) {
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

  // Multiway merge using a min-heap over shard heads
  std::vector<uint32_t> idx(shards);
  std::vector<uint32_t> end(shards);
  std::vector<Node> heapbuf;
  heapbuf.reserve(shards);

  for (size_t s = 0; s < shards; ++s) {
    uint32_t b = offsets[s];
    uint32_t e = offsets[s + 1];
    idx[s] = b;
    end[s] = e;
    if (b < e) {
      heapbuf.push_back(Node{values[b], static_cast<uint32_t>(s)});
    }
  }

  if (heapbuf.empty()) return 0;

  std::priority_queue<Node, std::vector<Node>, NodeGreater> pq(NodeGreater(), std::move(heapbuf));

  uint64_t count = 0;
  uint32_t last_value = 0;
  bool has_last = false;

  while (!pq.empty()) {
    Node n = pq.top();
    pq.pop();

    if (!has_last || n.value != last_value) {
      last_value = n.value;
      has_last = true;
      ++count;
    }

    uint32_t s = n.shard;
    uint32_t i = idx[s] + 1;
    const uint32_t e = end[s];
    // Skip duplicates within this shard equal to the popped value
    while (i < e && values[i] == n.value) {
      ++i;
    }
    idx[s] = i;
    if (i < e) {
      pq.push(Node{values[i], s});
    }
  }

  return count;
}
