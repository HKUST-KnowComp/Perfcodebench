#include "interface.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <utility>
#include <vector>

namespace {

struct Graph {
  std::vector<uint32_t> offsets;
  std::vector<uint32_t> edges;
};

Graph make_graph(uint32_t nodes) {
  std::vector<std::pair<uint32_t, uint32_t>> undirected;
  undirected.reserve(static_cast<std::size_t>(nodes) * 16u);
  for (uint32_t u = 0; u + 2u < nodes; ++u) {
    undirected.push_back({u, u + 1u});
    undirected.push_back({u, u + 2u});
    undirected.push_back({u + 1u, u + 2u});
    if (u + 8u < nodes) undirected.push_back({u, u + 8u});
    if (u + 16u < nodes) undirected.push_back({u, u + 16u});
    if ((u & 3u) == 0u && u + 33u < nodes) undirected.push_back({u, u + 33u});
  }
  std::sort(undirected.begin(), undirected.end());
  undirected.erase(std::unique(undirected.begin(), undirected.end()), undirected.end());

  std::vector<std::vector<uint32_t>> adj(nodes);
  for (const auto& e : undirected) {
    adj[e.first].push_back(e.second);
    adj[e.second].push_back(e.first);
  }
  for (auto& nbrs : adj) {
    std::sort(nbrs.begin(), nbrs.end());
  }

  Graph g;
  g.offsets.resize(static_cast<std::size_t>(nodes) + 1u);
  std::size_t pos = 0;
  for (uint32_t u = 0; u < nodes; ++u) {
    g.offsets[u] = static_cast<uint32_t>(pos);
    pos += adj[u].size();
  }
  g.offsets[nodes] = static_cast<uint32_t>(pos);
  g.edges.resize(pos);
  pos = 0;
  for (uint32_t u = 0; u < nodes; ++u) {
    for (uint32_t v : adj[u]) g.edges[pos++] = v;
  }
  return g;
}

uint64_t expected_checksum(const std::vector<uint32_t>& offsets, const std::vector<uint32_t>& edges) {
  uint64_t count = 0;
  const uint32_t n = static_cast<uint32_t>(offsets.size() - 1u);
  for (uint32_t u = 0; u < n; ++u) {
    for (uint32_t i = offsets[u]; i < offsets[u + 1u]; ++i) {
      const uint32_t v = edges[i];
      if (v <= u) continue;
      uint32_t p = i + 1u;
      uint32_t q = offsets[v];
      while (p < offsets[u + 1u] && q < offsets[v + 1u]) {
        const uint32_t a = edges[p];
        const uint32_t b = edges[q];
        if (a <= v) {
          ++p;
        } else if (b <= v) {
          ++q;
        } else if (a == b) {
          ++count;
          ++p;
          ++q;
        } else if (a < b) {
          ++p;
        } else {
          ++q;
        }
      }
    }
  }
  uint64_t hash = 1469598103934665603ULL;
  hash ^= count;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

int main() {
  constexpr uint32_t kNodes = 24576;
  constexpr int kIters = 2;
  const Graph graph = make_graph(kNodes);
  const uint64_t expected = expected_checksum(graph.offsets, graph.edges);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = triangle_count_checksum(graph.offsets, graph.edges, kIters);
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
