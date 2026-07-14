#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t triangle_count_checksum(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& edges,
    int iters) {
  uint64_t hash = 0;
  if (offsets.size() < 2) return hash;
  const uint32_t n = static_cast<uint32_t>(offsets.size() - 1u);
  const uint32_t* offsets_data = offsets.data();
  const uint32_t* edges_data = edges.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t count = 0;
    for (uint32_t u = 0; u < n; ++u) {
      uint32_t u_start = offsets_data[u];
      uint32_t u_end = offsets_data[u + 1];
      for (uint32_t i = u_start; i < u_end; ++i) {
        uint32_t v = edges_data[i];
        if (v <= u) continue;

        const uint32_t* v_begin = edges_data + offsets_data[v];
        const uint32_t* v_end = edges_data + offsets_data[v + 1];
        const uint32_t* v_ptr = v_begin;

        for (uint32_t p = i + 1; p < u_end; ++p) {
          uint32_t w = edges_data[p];
          while (v_ptr < v_end && *v_ptr < w) {
            ++v_ptr;
          }
          if (v_ptr < v_end && *v_ptr == w) {
            ++count;
          }
        }
      }
    }
    hash = 1469598103934665603ULL;
    hash ^= count;
    hash *= 1099511628211ULL;
  }
  return hash;
}