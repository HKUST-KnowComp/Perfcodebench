#include "interface.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <queue>
#include <vector>

namespace {

struct Graph {
  std::vector<uint32_t> offsets;
  std::vector<uint32_t> edges;
};

Graph make_graph(uint32_t nodes, uint32_t degree) {
  Graph g;
  g.offsets.resize(static_cast<std::size_t>(nodes) + 1u);
  g.edges.resize(static_cast<std::size_t>(nodes) * static_cast<std::size_t>(degree));
  uint64_t state = 0x9e3779b97f4a7c15ULL;
  std::size_t edge_pos = 0;
  for (uint32_t u = 0; u < nodes; ++u) {
    g.offsets[u] = static_cast<uint32_t>(edge_pos);
    for (uint32_t j = 0; j < degree; ++j) {
      state = state * 6364136223846793005ULL + 1442695040888963407ULL;
      uint32_t v = static_cast<uint32_t>((state + static_cast<uint64_t>(u) * 1315423911ULL +
                                          static_cast<uint64_t>(j) * 2654435761ULL) % nodes);
      g.edges[edge_pos++] = v;
    }
  }
  g.offsets[nodes] = static_cast<uint32_t>(edge_pos);
  return g;
}

uint64_t checksum_levels(const std::vector<int32_t>& levels) {
  uint64_t hash = 1469598103934665603ULL;
  for (int32_t level : levels) {
    hash ^= static_cast<uint64_t>(static_cast<uint32_t>(level + 1));
    hash *= 1099511628211ULL;
  }
  return hash;
}

uint64_t expected_checksum(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& edges,
    uint32_t source) {
  const uint32_t n = static_cast<uint32_t>(offsets.size() - 1u);
  std::vector<int32_t> levels(n, -1);
  std::queue<uint32_t> q;
  levels[source] = 0;
  q.push(source);
  while (!q.empty()) {
    const uint32_t u = q.front();
    q.pop();
    const int32_t next_level = levels[u] + 1;
    for (uint32_t idx = offsets[u]; idx < offsets[u + 1u]; ++idx) {
      const uint32_t v = edges[idx];
      if (levels[v] == -1) {
        levels[v] = next_level;
        q.push(v);
      }
    }
  }
  return checksum_levels(levels);
}

}  // namespace

int main() {
  constexpr uint32_t kNodes = 65536;
  constexpr uint32_t kDegree = 12;
  constexpr int kIters = 4;
  const Graph graph = make_graph(kNodes, kDegree);
  const uint64_t expected = expected_checksum(graph.offsets, graph.edges, 0u);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = bfs_level_checksum(graph.offsets, graph.edges, 0u, kIters);
  const auto end = std::chrono::steady_clock::now();

  const bool ok = actual == expected;
  const long long elapsed_ns =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
  std::cout << std::boolalpha;
  std::cout << "{"
            << "\"ok\":" << ok << ","
            << "\"elapsed_ns\":" << elapsed_ns << ","
            << "\"expected_checksum\":" << expected << ","
            << "\"actual_checksum\":" << actual << "}\n";
  return ok ? 0 : 1;
}
